// ---------------------------------------------------------------------------
// Carenuity C3-Mini v2.2.2 – Web-Light-Control + mDNS + Speed-Messung
// Verbesserte Version mit Bugfixes und Verbesserungen
// ---------------------------------------------------------------------------
#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <WebServer.h>   // leichter HTTP-Server
#include <ESPmDNS.h>     // mDNS-Responder
#include <Preferences.h> // Für persistente Einstellungen

/********** WLAN & mDNS **********/
constexpr char AP_SSID[]    = "420";         // WLAN-Name
constexpr char AP_PASS[]    = "12345677";    // WLAN-Passwort
constexpr bool AP_HIDDEN    = true;          // Verstecktes WLAN
constexpr uint8_t AP_CHANNEL= 1;             // WLAN-Kanal
const char* MDNS_NAME      = "ben";          // → ben.local

/********** Pins **********/
constexpr uint8_t MOS_PIN    = 2;   // Gate MOSFET Scooter Power
constexpr uint8_t LIGHT_PIN  = 4;   // Gate MOSFET Licht
constexpr uint8_t RPM_PIN    = 3;   // Hall-Sensor (Interrupt)

/********** Auto-OFF **********/
constexpr uint32_t OFF_DELAY_MS = 10000; // 10 s nach letztem Disconnect

/********** Rad-Kalibrierung **********/
constexpr uint8_t  PULSES_PER_REV = 2;   // Hall-Impulse pro Radumdrehung
constexpr float    WHEEL_CIRC_M   = 0.68f; // Radumfang in Metern

/********** Debouncing **********/
constexpr uint32_t DEBOUNCE_TIME_US = 1000; // 1ms Debouncing für Hall-Sensor

/********** Globals **********/
volatile uint32_t pulseCount     = 0;   // Impulse im aktuellen 1-s-Fenster
volatile uint32_t lastPulseTime  = 0;   // Für Debouncing
volatile uint8_t  stationCount   = 0;   // verbundene Clients
unsigned long     lastDisconnect = 0;   // Zeitstempel für Auto-OFF
double            kmh            = 0.0;
unsigned long     lastCalc       = 0;
bool              lightState     = false; // Lichtstatus merken
bool              powerState     = false; // Power-Status merken

WebServer server(80);                  // HTTP-Server auf Port 80
Preferences prefs;                     // Für persistente Einstellungen

/********** Interrupt: Hall-Sensor zählt Impulse mit Debouncing **********/
void IRAM_ATTR onPulse() {
  uint32_t currentTime = micros();
  if (currentTime - lastPulseTime > DEBOUNCE_TIME_US) {
    pulseCount++;
    lastPulseTime = currentTime;
  }
}

/********** WLAN Event: Client connect/disconnect **********/
void onWiFiEvent(WiFiEvent_t evt, WiFiEventInfo_t info) {
  switch (evt) {
    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
      stationCount++;
      if (!powerState) {
        digitalWrite(MOS_PIN, HIGH);
        powerState = true;
        Serial.printf("Client verbunden (%u) → Power ON\n", stationCount);
      }
      lastDisconnect = 0;
      break;
      
    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
      if (stationCount > 0) stationCount--;
      Serial.printf("Client getrennt (%u)\n", stationCount);
      if (stationCount == 0) {
        lastDisconnect = millis();
      }
      break;
      
    case ARDUINO_EVENT_WIFI_AP_START:
      Serial.println("Access Point gestartet");
      break;
      
    case ARDUINO_EVENT_WIFI_AP_STOP:
      Serial.println("Access Point gestoppt");
      break;
      
    default:
      break;
  }
}

/********** HTTP-Handler: Root → Web-UI mit verbessertem Design **********/
void handleRoot() {
  const char* html = R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset='utf-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1'>
    <title>Scooter Control</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; background: #f0f0f0; }
        .container { max-width: 400px; margin: 0 auto; background: white; padding: 20px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
        h1 { color: #333; text-align: center; }
        .button { display: inline-block; padding: 10px 20px; margin: 5px; text-decoration: none; border-radius: 5px; color: white; font-weight: bold; }
        .btn-on { background: #4CAF50; }
        .btn-off { background: #f44336; }
        .speed { font-size: 24px; font-weight: bold; color: #333; text-align: center; margin: 20px 0; }
        .status { margin: 10px 0; padding: 10px; background: #e7f3ff; border-radius: 5px; }
    </style>
</head>
<body>
    <div class='container'>
        <h1>🛴 Scooter Control</h1>
        <div class='status'>
            <p>💡 Licht: <span id='lightStatus'>--</span></p>
            <p>⚡ Power: <span id='powerStatus'>--</span></p>
            <p>📶 Clients: <span id='clientCount'>--</span></p>
        </div>
        <p style='text-align: center;'>
            <a href='/light?state=on' class='button btn-on'>Licht AN</a>
            <a href='/light?state=off' class='button btn-off'>Licht AUS</a>
        </p>
        <div class='speed'>
            🏃 <span id='speed'>--</span> km/h
        </div>
    </div>
    <script>
        function updateStatus() {
            fetch('/status')
                .then(r => r.json())
                .then(data => {
                    document.getElementById('speed').innerText = data.speed;
                    document.getElementById('lightStatus').innerText = data.light ? 'AN' : 'AUS';
                    document.getElementById('powerStatus').innerText = data.power ? 'AN' : 'AUS';
                    document.getElementById('clientCount').innerText = data.clients;
                })
                .catch(e => console.log('Fehler:', e));
        }
        setInterval(updateStatus, 1000);
        updateStatus(); // Sofort laden
    </script>
</body>
</html>
)";
  server.send(200, "text/html; charset=utf-8", html);
}

/********** HTTP-Handler: Licht schalten mit verbesserter Logik **********/
void handleLight() {
  String state = server.arg("state");
  if (state == "on") {
    digitalWrite(LIGHT_PIN, HIGH);
    lightState = true;
    prefs.putBool("lightState", lightState); // Persistieren
    server.send(200, "text/plain", "Light ON");
    Serial.println("Licht AN");
  }
  else if (state == "off") {
    digitalWrite(LIGHT_PIN, LOW);
    lightState = false;
    prefs.putBool("lightState", lightState); // Persistieren
    server.send(200, "text/plain", "Light OFF");
    Serial.println("Licht AUS");
  }
  else if (state == "toggle") {
    lightState = !lightState;
    digitalWrite(LIGHT_PIN, lightState ? HIGH : LOW);
    prefs.putBool("lightState", lightState);
    server.send(200, "text/plain", lightState ? "Light ON" : "Light OFF");
    Serial.printf("Licht %s\n", lightState ? "AN" : "AUS");
  }
  else {
    server.send(400, "text/plain", "Bad Request: use ?state=on|off|toggle");
  }
}

/********** HTTP-Handler: Status-JSON für verbesserte Web-UI **********/
void handleStatus() {
  char json[200];
  snprintf(json, sizeof(json), 
    "{\"speed\":%.1f,\"light\":%s,\"power\":%s,\"clients\":%u}",
    kmh, 
    lightState ? "true" : "false",
    powerState ? "true" : "false",
    stationCount
  );
  server.send(200, "application/json", json);
}

/********** HTTP-Handler: Aktuelle Geschwindigkeit (Kompatibilität) **********/
void handleSpeed() {
  char buf[16];
  snprintf(buf, sizeof(buf), "%.1f", kmh);
  server.send(200, "text/plain", buf);
}

/********** HTTP-Handler: Konfiguration anzeigen **********/
void handleConfig() {
  char html[1000];
  snprintf(html, sizeof(html), R"(
<!DOCTYPE html>
<html>
<head><meta charset='utf-8'><title>Konfiguration</title></head>
<body>
<h1>Scooter Konfiguration</h1>
<ul>
<li>SSID: %s</li>
<li>Kanal: %u</li>
<li>Versteckt: %s</li>
<li>mDNS: %s.local</li>
<li>Radumfang: %.2f m</li>
<li>Impulse/Umdrehung: %u</li>
<li>Auto-OFF Delay: %u ms</li>
</ul>
<p><a href='/'>← Zurück</a></p>
</body>
</html>
)", AP_SSID, AP_CHANNEL, AP_HIDDEN ? "Ja" : "Nein", MDNS_NAME, WHEEL_CIRC_M, PULSES_PER_REV, OFF_DELAY_MS);
  server.send(200, "text/html; charset=utf-8", html);
}

/********** Setup mit verbesserter Initialisierung **********/
void setup() {
  // Serieller Monitor früh starten
  Serial.begin(115200);
  delay(100);
  Serial.println("\n=== Scooter Control v2.2.2 gestartet ===");

  // Preferences initialisieren
  prefs.begin("scooter", false);
  
  // Pins initialisieren
  pinMode(MOS_PIN, OUTPUT);
  digitalWrite(MOS_PIN, LOW);
  powerState = false;
  
  pinMode(LIGHT_PIN, OUTPUT);
  // Lichtstatus aus Preferences laden
  lightState = prefs.getBool("lightState", false);
  digitalWrite(LIGHT_PIN, lightState ? HIGH : LOW);
  Serial.printf("Licht-Status geladen: %s\n", lightState ? "AN" : "AUS");
  
  pinMode(RPM_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RPM_PIN), onPulse, RISING);

  // WLAN-AP starten
  WiFi.onEvent(onWiFiEvent);
  
  // Erweiterte AP-Konfiguration
  wifi_config_t ap_config = {};
  strcpy((char*)ap_config.ap.ssid, AP_SSID);
  strcpy((char*)ap_config.ap.password, AP_PASS);
  ap_config.ap.ssid_len = strlen(AP_SSID);
  ap_config.ap.channel = AP_CHANNEL;
  ap_config.ap.authmode = WIFI_AUTH_WPA2_PSK;
  ap_config.ap.ssid_hidden = AP_HIDDEN ? 1 : 0;
  ap_config.ap.max_connection = 4;
  ap_config.ap.beacon_interval = 100;
  
  if (WiFi.softAP(AP_SSID, AP_PASS, AP_CHANNEL, AP_HIDDEN)) {
    Serial.printf("Hotspot '%s' läuft → IP: %s\n",
                  AP_SSID, WiFi.softAPIP().toString().c_str());
  } else {
    Serial.println("FEHLER: Hotspot konnte nicht gestartet werden!");
  }

  // mDNS-Responder für ben.local
  if (MDNS.begin(MDNS_NAME)) {
    Serial.printf("mDNS gestartet: http://%s.local\n", MDNS_NAME);
    MDNS.addService("http", "tcp", 80);
    MDNS.addServiceTxt("http", "tcp", "device", "scooter");
    MDNS.addServiceTxt("http", "tcp", "version", "v2.2.2");
  } else {
    Serial.println("WARNUNG: mDNS Start fehlgeschlagen");
  }

  // HTTP-Routen mit 404-Handler
  server.on("/",       HTTP_GET, handleRoot);
  server.on("/light",  HTTP_GET, handleLight);
  server.on("/speed",  HTTP_GET, handleSpeed);
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/config", HTTP_GET, handleConfig);
  
  // 404-Handler
  server.onNotFound([]() {
    server.send(404, "text/plain", "404 - Seite nicht gefunden");
  });
  
  server.begin();
  Serial.println("HTTP-Server gestartet auf Port 80");
  Serial.println("=== Setup abgeschlossen ===\n");
}

/********** Loop mit verbesserter Logik **********/
void loop() {
  // HTTP-Server bedienen
  server.handleClient();

  // Auto-OFF nach OFF_DELAY_MS mit verbesserter Logik
  if (stationCount == 0 && lastDisconnect != 0 && powerState &&
      (millis() - lastDisconnect >= OFF_DELAY_MS)) {
    digitalWrite(MOS_PIN, LOW);
    powerState = false;
    lastDisconnect = 0;
    Serial.println("Gnadenfrist abgelaufen → Power OFF");
  }

  // Geschwindigkeit berechnen & cachen (1× pro Sekunde)
  unsigned long now = millis();
  if (now - lastCalc >= 1000) {
    // Atomarer Zugriff auf pulseCount
    noInterrupts();
    uint32_t p = pulseCount;
    pulseCount = 0;
    interrupts();
    
    if (p > 0) {
      // Geschwindigkeitsberechnung mit verbesserter Formel
      // rpm = (Impulse pro Sekunde * 60) / Impulse pro Umdrehung
      double rpm = (p * 60.0) / PULSES_PER_REV;
      // km/h = (rpm * Radumfang in m * 60) / 1000
      kmh = (rpm * WHEEL_CIRC_M * 60.0) / 1000.0;
      
      Serial.printf("Impulse: %u, RPM: %.1f, Geschwindigkeit: %.1f km/h\n", 
                    p, rpm, kmh);
    } else {
      // Keine Impulse = Stillstand
      if (kmh > 0.1) { // Nur loggen wenn vorher Bewegung war
        Serial.println("Stillstand erkannt → 0 km/h");
      }
      kmh = 0.0;
    }
    
    lastCalc = now;
  }
  
  // mDNS aktualisieren
  MDNS.update();
}