// ---------------------------------------------------------------------------
// Carenuity C3-Mini v2.2.2 – Web-Light-Control + mDNS + Speed-Messung
// ---------------------------------------------------------------------------
#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <WebServer.h>   // leichter HTTP-Server
#include <ESPmDNS.h>     // mDNS-Responder

/********** WLAN & mDNS **********/
constexpr char AP_SSID[]    = "420";         // bleibt unverändert
constexpr char AP_PASS[]    = "12345677";
constexpr bool AP_HIDDEN    = true;
constexpr uint8_t AP_CHANNEL= 1;
const char* MDNS_NAME      = "ben";         // → ben.local

/********** Pins **********/
constexpr uint8_t MOS_PIN    = 2;   // Gate MOSFET Scooter Power
constexpr uint8_t LIGHT_PIN  = 4;   // Gate MOSFET Licht
constexpr uint8_t RPM_PIN    = 3;   // Hall-Sensor (Interrupt)

/********** Auto-OFF **********/
constexpr uint32_t OFF_DELAY_MS = 10000; // 10 s nach letztem Disconnect

/********** Rad-Kalibrierung **********/
constexpr uint8_t  PULSES_PER_REV = 2;   // Hall-Impulse pro Radumdrehung
constexpr float    WHEEL_CIRC_M   = 0.68f; // Radumfang in Metern

/********** Globals **********/
volatile uint32_t pulseCount     = 0;   // Impulse im aktuellen 1-s-Fenster
volatile uint8_t  stationCount   = 0;   // verbundene Clients
unsigned long     lastDisconnect = 0;   // Zeitstempel für Auto-OFF
double            kmh            = 0.0;
unsigned long     lastCalc       = 0;

WebServer server(80);                  // HTTP-Server auf Port 80

/********** Interrupt: Hall-Sensor zählt Impulse **********/
void IRAM_ATTR onPulse() {
  pulseCount++;
}

/********** WLAN Event: Client connect/disconnect **********/
void onWiFiEvent(WiFiEvent_t evt, WiFiEventInfo_t info) {
  if (evt == ARDUINO_EVENT_WIFI_AP_STACONNECTED) {
    stationCount++;
    digitalWrite(MOS_PIN, HIGH);      // Power ON beim ersten Client
    lastDisconnect = 0;
    Serial.printf("Client verbunden (%u) → Power ON\n", stationCount);
  }
  else if (evt == ARDUINO_EVENT_WIFI_AP_STADISCONNECTED) {
    if (stationCount > 0) stationCount--;
    Serial.printf("Client getrennt (%u)\n", stationCount);
    if (stationCount == 0) lastDisconnect = millis();
  }
}

/********** HTTP-Handler: Root → Web-UI **********/
void handleRoot() {
  const char* html =
    "<!DOCTYPE html><html><head><meta charset='utf-8'>"
    "<title>Scooter Control</title>"
    "</head><body>"
    "<h1>Scooter Control</h1>"
    "<p>"
      "<a href=\"/light?state=on\">Licht AN</a> | "
      "<a href=\"/light?state=off\">Licht AUS</a>"
    "</p>"
    "<p>Geschwindigkeit: <span id='speed'>--</span> km/h</p>"
    "<script>"
      "function fetchSpeed(){"
        "fetch('/speed').then(r=>r.text()).then(t=>{"
          "document.getElementById('speed').innerText = t;"
        "});"
      "}"
      "setInterval(fetchSpeed,1000);"
    "</script>"
    "</body></html>";
  server.send(200, "text/html; charset=utf-8", html);
}

/********** HTTP-Handler: Licht schalten **********/
void handleLight() {
  String state = server.arg("state");
  if (state == "on") {
    digitalWrite(LIGHT_PIN, HIGH);
    server.send(200, "text/plain", "Light ON");
    Serial.println("Licht AN");
  }
  else if (state == "off") {
    digitalWrite(LIGHT_PIN, LOW);
    server.send(200, "text/plain", "Light OFF");
    Serial.println("Licht AUS");
  }
  else {
    server.send(400, "text/plain", "Bad Request");
  }
}

/********** HTTP-Handler: Aktuelle Geschwindigkeit **********/
void handleSpeed() {
  char buf[16];
  snprintf(buf, sizeof(buf), "%.1f", kmh);
  server.send(200, "text/plain", buf);
}

/********** Setup **********/
void setup() {
  // Pins initialisieren
  pinMode(MOS_PIN, OUTPUT);
  digitalWrite(MOS_PIN, LOW);
  pinMode(LIGHT_PIN, OUTPUT);
  digitalWrite(LIGHT_PIN, LOW);
  pinMode(RPM_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RPM_PIN), onPulse, RISING);

  // Serieller Monitor
  Serial.begin(115200);
  delay(100);

  // WLAN-AP starten
  WiFi.onEvent(onWiFiEvent);
  WiFi.softAP(AP_SSID, AP_PASS, AP_CHANNEL, AP_HIDDEN);
  Serial.printf("Hotspot '%s' läuft → IP: %s\n",
                AP_SSID, WiFi.softAPIP().toString().c_str());

  // mDNS-Responder für ben.local
  if (MDNS.begin(MDNS_NAME)) {
    Serial.printf("mDNS gestartet: http://%s.local\n", MDNS_NAME);
    MDNS.addService("http", "tcp", 80);
  } else {
    Serial.println("mDNS Start fehlgeschlagen");
  }

  // HTTP-Routen
  server.on("/",       HTTP_GET, handleRoot);
  server.on("/light",  HTTP_GET, handleLight);
  server.on("/speed",  HTTP_GET, handleSpeed);
  server.begin();
  Serial.println("HTTP-Server gestartet auf Port 80");
}

/********** Loop **********/
void loop() {
  // HTTP-Server bedien
  server.handleClient();

  // Auto-OFF nach OFF_DELAY_MS
  if (stationCount == 0 && lastDisconnect != 0 &&
      (millis() - lastDisconnect >= OFF_DELAY_MS)) {
    digitalWrite(MOS_PIN, LOW);
    lastDisconnect = 0;
    Serial.println("Gnadenfrist abgelaufen → Power OFF");
  }

  // Geschwindigkeit berechnen & cachen (1× pro Sekunde)
  unsigned long now = millis();
  if (now - lastCalc >= 1000) {
    noInterrupts();
      uint32_t p = pulseCount;
      pulseCount = 0;
    interrupts();
    // rpm = Impulse/s * 60 / PPR
    double rpm = (p * 60.0) / PULSES_PER_REV;
    // km/h = rpm * Umfang(m) * 0.06
    kmh = rpm * WHEEL_CIRC_M * 0.06;
    Serial.printf("Aktuell: %.1f km/h\n", kmh);
    lastCalc = now;
  }
}