// ---------------------------------------------------------------------------
// Carenuity C3-Mini v2.2.3 – Web-Light-Control + mDNS + Speed + Kilometerzähler
// Verbesserte Version mit Bugfixes, Verbesserungen und Odometer
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

/********** Odometer **********/
constexpr uint32_t ODOMETER_SAVE_INTERVAL = 100; // Speichere alle 100m
constexpr float    MIN_SPEED_FOR_ODOMETER = 0.5; // Mindestgeschwindigkeit für Kilometerzählung

/********** Globals **********/
volatile uint32_t pulseCount     = 0;   // Impulse im aktuellen 1-s-Fenster
volatile uint32_t lastPulseTime  = 0;   // Für Debouncing
volatile uint8_t  stationCount   = 0;   // verbundene Clients
unsigned long     lastDisconnect = 0;   // Zeitstempel für Auto-OFF
double            kmh            = 0.0;
unsigned long     lastCalc       = 0;
bool              lightState     = false; // Lichtstatus merken
bool              powerState     = false; // Power-Status merken

// Odometer Variablen
double            totalKm        = 0.0;   // Gesamt-Kilometer
double            tripKm         = 0.0;   // Trip-Kilometer (zurücksetzbar)
double            lastSavedKm    = 0.0;   // Letzter gespeicherter KM-Stand
unsigned long     lastOdometerUpdate = 0; // Zeitstempel für Odometer-Update

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

/********** Kilometerzähler aktualisieren **********/
void updateOdometer() {
  unsigned long now = millis();
  
  // Nur wenn sich bewegt wird (Mindestgeschwindigkeit)
  if (kmh >= MIN_SPEED_FOR_ODOMETER && (now - lastOdometerUpdate >= 1000)) {
    // Zurückgelegte Strecke in der letzten Sekunde (in km)
    double distanceKm = kmh / 3600.0; // km/h / 3600 = km/s
    
    totalKm += distanceKm;
    tripKm += distanceKm;
    
    // Speichere regelmäßig den Gesamtstand
    if (totalKm - lastSavedKm >= (ODOMETER_SAVE_INTERVAL / 1000.0)) {
      prefs.putDouble("totalKm", totalKm);
      lastSavedKm = totalKm;
      Serial.printf("Odometer gespeichert: %.3f km\n", totalKm);
    }
    
    lastOdometerUpdate = now;
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
        // Odometer beim Disconnect speichern
        prefs.putDouble("totalKm", totalKm);
        prefs.putDouble("tripKm", tripKm);
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

/********** HTTP-Handler: Root → Web-UI mit Kilometerzähler **********/
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
        .button { display: inline-block; padding: 10px 20px; margin: 5px; text-decoration: none; border-radius: 5px; color: white; font-weight: bold; text-align: center; }
        .btn-on { background: #4CAF50; }
        .btn-off { background: #f44336; }
        .btn-reset { background: #ff9800; font-size: 12px; padding: 5px 10px; }
        .speed { font-size: 24px; font-weight: bold; color: #333; text-align: center; margin: 20px 0; }
        .status { margin: 10px 0; padding: 10px; background: #e7f3ff; border-radius: 5px; }
        .odometer { margin: 10px 0; padding: 15px; background: #f0f8ff; border-radius: 5px; border-left: 4px solid #2196F3; }
        .odometer h3 { margin: 0 0 10px 0; color: #1976D2; }
        .km-display { display: flex; justify-content: space-between; margin: 5px 0; }
        .km-label { font-weight: bold; }
        .km-value { color: #333; }
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
        
        <div class='speed'>
            🏃 <span id='speed'>--</span> km/h
        </div>
        
        <div class='odometer'>
            <h3>📊 Kilometerzähler</h3>
            <div class='km-display'>
                <span class='km-label'>Gesamt:</span>
                <span class='km-value' id='totalKm'>-- km</span>
            </div>
            <div class='km-display'>
                <span class='km-label'>Trip:</span>
                <span class='km-value' id='tripKm'>-- km</span>
            </div>
            <div style='text-align: center; margin-top: 10px;'>
                <a href='/trip/reset' class='button btn-reset' onclick='return confirm("Trip-Zähler zurücksetzen?")'>Trip Reset</a>
            </div>
        </div>
        
        <p style='text-align: center;'>
            <a href='/light?state=on' class='button btn-on'>Licht AN</a>
            <a href='/light?state=off' class='button btn-off'>Licht AUS</a>
        </p>
        
        <p style='text-align: center; margin-top: 15px;'>
            <a href='/config' style='color: #666; text-decoration: none; font-size: 12px;'>⚙️ Konfiguration</a>
        </p>
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
                    document.getElementById('totalKm').innerText = data.totalKm + ' km';
                    document.getElementById('tripKm').innerText = data.tripKm + ' km';
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
    prefs.putBool("lightState", lightState);
    server.send(200, "text/plain", "Light ON");
    Serial.println("Licht AN");
  }
  else if (state == "off") {
    digitalWrite(LIGHT_PIN, LOW);
    lightState = false;
    prefs.putBool("lightState", lightState);
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

/********** HTTP-Handler: Trip-Zähler zurücksetzen **********/
void handleTripReset() {
  tripKm = 0.0;
  prefs.putDouble("tripKm", tripKm);
  Serial.println("Trip-Zähler zurückgesetzt");
  
  // Redirect zur Hauptseite
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "Trip Reset - Redirecting...");
}

/********** HTTP-Handler: Status-JSON mit Kilometerzähler **********/
void handleStatus() {
  char json[300];
  snprintf(json, sizeof(json), 
    "{\"speed\":%.1f,\"light\":%s,\"power\":%s,\"clients\":%u,\"totalKm\":\"%.3f\",\"tripKm\":\"%.3f\"}",
    kmh, 
    lightState ? "true" : "false",
    powerState ? "true" : "false",
    stationCount,
    totalKm,
    tripKm
  );
  server.send(200, "application/json", json);
}

/********** HTTP-Handler: Aktuelle Geschwindigkeit (Kompatibilität) **********/
void handleSpeed() {
  char buf[16];
  snprintf(buf, sizeof(buf), "%.1f", kmh);
  server.send(200, "text/plain", buf);
}

/********** HTTP-Handler: Odometer-Daten als JSON **********/
void handleOdometer() {
  char json[150];
  snprintf(json, sizeof(json), 
    "{\"totalKm\":%.3f,\"tripKm\":%.3f,\"lastSaved\":%.3f}",
    totalKm, tripKm, lastSavedKm
  );
  server.send(200, "application/json", json);
}

/********** HTTP-Handler: Konfiguration anzeigen **********/
void handleConfig() {
  char html[1200];
  snprintf(html, sizeof(html), R"(
<!DOCTYPE html>
<html>
<head><meta charset='utf-8'><title>Konfiguration</title>
<style>
body { font-family: Arial, sans-serif; margin: 20px; background: #f0f0f0; }
.container { max-width: 500px; margin: 0 auto; background: white; padding: 20px; border-radius: 10px; }
.config-section { margin: 20px 0; padding: 15px; background: #f9f9f9; border-radius: 5px; }
h3 { color: #333; margin-top: 0; }
ul { list-style-type: none; padding: 0; }
li { margin: 8px 0; padding: 5px; background: white; border-radius: 3px; }
.back-link { display: inline-block; margin-top: 20px; color: #2196F3; text-decoration: none; }
</style>
</head>
<body>
<div class='container'>
<h1>🛴 Scooter Konfiguration</h1>

<div class='config-section'>
<h3>🌐 WLAN Einstellungen</h3>
<ul>
<li><strong>SSID:</strong> %s</li>
<li><strong>Kanal:</strong> %u</li>
<li><strong>Versteckt:</strong> %s</li>
<li><strong>mDNS:</strong> %s.local</li>
</ul>
</div>

<div class='config-section'>
<h3>⚙️ Hardware Konfiguration</h3>
<ul>
<li><strong>Radumfang:</strong> %.2f m</li>
<li><strong>Impulse/Umdrehung:</strong> %u</li>
<li><strong>Auto-OFF Delay:</strong> %u ms</li>
<li><strong>Debounce Zeit:</strong> %u μs</li>
</ul>
</div>

<div class='config-section'>
<h3>📊 Kilometerzähler</h3>
<ul>
<li><strong>Gesamt-KM:</strong> %.3f km</li>
<li><strong>Trip-KM:</strong> %.3f km</li>
<li><strong>Speicher-Intervall:</strong> %u m</li>
<li><strong>Min. Geschwindigkeit:</strong> %.1f km/h</li>
</ul>
</div>

<a href='/' class='back-link'>← Zurück zur Hauptseite</a>
</div>
</body>
</html>
)", AP_SSID, AP_CHANNEL, AP_HIDDEN ? "Ja" : "Nein", MDNS_NAME, 
   WHEEL_CIRC_M, PULSES_PER_REV, OFF_DELAY_MS, DEBOUNCE_TIME_US,
   totalKm, tripKm, ODOMETER_SAVE_INTERVAL, MIN_SPEED_FOR_ODOMETER);
  server.send(200, "text/html; charset=utf-8", html);
}

/********** Setup mit verbesserter Initialisierung **********/
void setup() {
  // Serieller Monitor früh starten
  Serial.begin(115200);
  delay(100);
  Serial.println("\n=== Scooter Control v2.2.3 mit Kilometerzähler gestartet ===");

  // Preferences initialisieren
  prefs.begin("scooter", false);
  
  // Odometer-Daten laden
  totalKm = prefs.getDouble("totalKm", 0.0);
  tripKm = prefs.getDouble("tripKm", 0.0);
  lastSavedKm = totalKm;
  Serial.printf("Odometer geladen: Gesamt=%.3f km, Trip=%.3f km\n", totalKm, tripKm);
  
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
    MDNS.addServiceTxt("http", "tcp", "version", "v2.2.3");
  } else {
    Serial.println("WARNUNG: mDNS Start fehlgeschlagen");
  }

  // HTTP-Routen mit Kilometerzähler
  server.on("/",         HTTP_GET, handleRoot);
  server.on("/light",    HTTP_GET, handleLight);
  server.on("/speed",    HTTP_GET, handleSpeed);
  server.on("/status",   HTTP_GET, handleStatus);
  server.on("/config",   HTTP_GET, handleConfig);
  server.on("/odometer", HTTP_GET, handleOdometer);
  server.on("/trip/reset", HTTP_GET, handleTripReset);
  
  // 404-Handler
  server.onNotFound([]() {
    server.send(404, "text/plain", "404 - Seite nicht gefunden");
  });
  
  server.begin();
  Serial.println("HTTP-Server gestartet auf Port 80");
  Serial.println("=== Setup abgeschlossen ===\n");
}

/********** Loop mit Kilometerzähler **********/
void loop() {
  // HTTP-Server bedienen
  server.handleClient();

  // Auto-OFF nach OFF_DELAY_MS mit verbesserter Logik
  if (stationCount == 0 && lastDisconnect != 0 && powerState &&
      (millis() - lastDisconnect >= OFF_DELAY_MS)) {
    digitalWrite(MOS_PIN, LOW);
    powerState = false;
    lastDisconnect = 0;
    // Odometer beim Power-OFF speichern
    prefs.putDouble("totalKm", totalKm);
    prefs.putDouble("tripKm", tripKm);
    Serial.println("Gnadenfrist abgelaufen → Power OFF, Odometer gespeichert");
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
      double rpm = (p * 60.0) / PULSES_PER_REV;
      kmh = (rpm * WHEEL_CIRC_M * 60.0) / 1000.0;
      
      Serial.printf("Impulse: %u, RPM: %.1f, Geschwindigkeit: %.1f km/h\n", 
                    p, rpm, kmh);
    } else {
      // Keine Impulse = Stillstand
      if (kmh > 0.1) {
        Serial.println("Stillstand erkannt → 0 km/h");
      }
      kmh = 0.0;
    }
    
    lastCalc = now;
  }
  
  // Kilometerzähler aktualisieren
  updateOdometer();
  
  // mDNS aktualisieren
  MDNS.update();
}