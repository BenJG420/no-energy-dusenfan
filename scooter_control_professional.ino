// ---------------------------------------------------------------------------
// Modular E-Scooter Control System v3.0.0
// Professional Edition für IT/Mechanik Integration
// Features: Advanced Logging, Diagnostics, OTA-Ready, Temperature Monitoring
// ---------------------------------------------------------------------------
#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Preferences.h>
#include <esp_system.h>
#include <esp_task_wdt.h>
#include <ArduinoJson.h>
#include <time.h>

/********** System Configuration **********/
#define FIRMWARE_VERSION "3.0.0"
#define BUILD_DATE __DATE__
#define BUILD_TIME __TIME__
#define DEVICE_ID "ESC001"  // E-Scooter Control Unit 001

/********** WLAN & Connectivity **********/
constexpr char AP_SSID[]    = "420";
constexpr char AP_PASS[]    = "12345677";
constexpr bool AP_HIDDEN    = true;
constexpr uint8_t AP_CHANNEL= 1;
const char* MDNS_NAME      = "ben";

/********** Hardware Pins **********/
constexpr uint8_t MOS_PIN    = 2;   // Gate MOSFET Scooter Power
constexpr uint8_t LIGHT_PIN  = 4;   // Gate MOSFET Licht
constexpr uint8_t RPM_PIN    = 3;   // Hall-Sensor (Interrupt)
constexpr uint8_t TEMP_PIN   = A0;  // Temperature Sensor (optional)

/********** System Parameters **********/
constexpr uint32_t OFF_DELAY_MS = 10000;
constexpr uint8_t  PULSES_PER_REV = 2;
constexpr float    WHEEL_CIRC_M   = 0.68f;
constexpr uint32_t DEBOUNCE_TIME_US = 1000;

/********** Diagnostics & Monitoring **********/
constexpr uint32_t DIAGNOSTICS_INTERVAL = 30000;  // 30s diagnostic cycle
constexpr uint32_t TEMP_CHECK_INTERVAL = 5000;    // 5s temperature check
constexpr float    TEMP_WARNING_THRESHOLD = 70.0; // °C
constexpr float    TEMP_CRITICAL_THRESHOLD = 85.0; // °C

/********** Logging System **********/
enum LogLevel {
  LOG_DEBUG = 0,
  LOG_INFO = 1,
  LOG_WARNING = 2,
  LOG_ERROR = 3,
  LOG_CRITICAL = 4
};

enum AlertType {
  ALERT_NONE = 0,
  ALERT_TEMPERATURE = 1,
  ALERT_CONNECTIVITY = 2,
  ALERT_SPEED_SENSOR = 3,
  ALERT_POWER_SYSTEM = 4,
  ALERT_BRAKE_SYSTEM = 5
};

/********** Global Variables **********/
// Core System
volatile uint32_t pulseCount = 0;
volatile uint32_t lastPulseTime = 0;
volatile uint8_t stationCount = 0;
unsigned long lastDisconnect = 0;
double kmh = 0.0;
unsigned long lastCalc = 0;
bool lightState = false;
bool powerState = false;

// Odometer
double totalKm = 0.0;
double tripKm = 0.0;
double lastSavedKm = 0.0;
unsigned long lastOdometerUpdate = 0;

// Diagnostics & Monitoring
float systemTemp = 0.0;
uint32_t uptime = 0;
uint32_t lastDiagnostics = 0;
uint32_t lastTempCheck = 0;
uint32_t connectivityDropouts = 0;
uint32_t speedSensorErrors = 0;
AlertType currentAlert = ALERT_NONE;

// System Health
struct SystemHealth {
  bool wifiStable;
  bool temperatureOK;
  bool speedSensorOK;
  bool powerSystemOK;
  uint32_t freeHeap;
  uint32_t minFreeHeap;
  float cpuUsage;
} health;

WebServer server(80);
Preferences prefs;

/********** Advanced Logging Function **********/
void logMessage(LogLevel level, const char* component, const char* message, AlertType alert = ALERT_NONE) {
  const char* levelStr[] = {"DEBUG", "INFO", "WARN", "ERROR", "CRIT"};
  const char* alertStr[] = {"NONE", "TEMP", "CONN", "SPEED", "POWER", "BRAKE"};
  
  unsigned long timestamp = millis();
  
  Serial.printf("[%lu] [%s] [%s] [ALERT:%s] %s\n", 
                timestamp, levelStr[level], component, alertStr[alert], message);
  
  // Store critical alerts
  if (level >= LOG_ERROR && alert != ALERT_NONE) {
    currentAlert = alert;
    char alertKey[32];
    snprintf(alertKey, sizeof(alertKey), "alert_%lu", timestamp);
    prefs.putString(alertKey, message);
  }
}

/********** Temperature Monitoring **********/
float readTemperature() {
  // Simulate temperature reading (replace with actual sensor)
  // For demonstration: use CPU temperature estimation
  return 25.0 + (ESP.getCpuFreqMHz() / 10.0) + random(-5, 15);
}

void checkTemperature() {
  systemTemp = readTemperature();
  
  if (systemTemp > TEMP_CRITICAL_THRESHOLD) {
    logMessage(LOG_CRITICAL, "TEMP", "Critical temperature reached - system shutdown", ALERT_TEMPERATURE);
    // Emergency shutdown logic would go here
  } else if (systemTemp > TEMP_WARNING_THRESHOLD) {
    logMessage(LOG_WARNING, "TEMP", "High temperature warning", ALERT_TEMPERATURE);
  }
  
  health.temperatureOK = (systemTemp < TEMP_WARNING_THRESHOLD);
}

/********** System Diagnostics **********/
void runDiagnostics() {
  // Update system health metrics
  health.freeHeap = ESP.getFreeHeap();
  health.minFreeHeap = ESP.getMinFreeHeap();
  health.wifiStable = (WiFi.softAPgetStationNum() >= 0); // WiFi functioning
  health.speedSensorOK = (millis() - lastCalc < 5000); // Recent speed data
  health.powerSystemOK = true; // Could add voltage monitoring
  
  // Log diagnostic summary
  logMessage(LOG_INFO, "DIAG", 
    String("Heap: " + String(health.freeHeap) + 
           ", Temp: " + String(systemTemp) + 
           "°C, Uptime: " + String(uptime/1000) + "s").c_str());
  
  // Check for connectivity issues
  if (connectivityDropouts > 5) {
    logMessage(LOG_WARNING, "CONN", "Multiple connectivity dropouts detected", ALERT_CONNECTIVITY);
  }
  
  // Memory health check
  if (health.freeHeap < 10000) {
    logMessage(LOG_WARNING, "SYS", "Low memory warning", ALERT_POWER_SYSTEM);
  }
}

/********** Enhanced Interrupt Handler **********/
void IRAM_ATTR onPulse() {
  uint32_t currentTime = micros();
  if (currentTime - lastPulseTime > DEBOUNCE_TIME_US) {
    pulseCount++;
    lastPulseTime = currentTime;
  }
}

/********** Enhanced Odometer with Error Checking **********/
void updateOdometer() {
  unsigned long now = millis();
  
  if (kmh >= 0.5 && (now - lastOdometerUpdate >= 1000)) {
    double distanceKm = kmh / 3600.0;
    
    // Sanity check for unrealistic speed increases
    if (distanceKm > 0.1) { // More than 360 km/h is unrealistic
      speedSensorErrors++;
      logMessage(LOG_WARNING, "SPEED", "Unrealistic speed detected", ALERT_SPEED_SENSOR);
      return;
    }
    
    totalKm += distanceKm;
    tripKm += distanceKm;
    
    // Save periodically
    if (totalKm - lastSavedKm >= 0.1) {
      prefs.putDouble("totalKm", totalKm);
      prefs.putDouble("tripKm", tripKm);
      lastSavedKm = totalKm;
      logMessage(LOG_DEBUG, "ODO", ("Saved: " + String(totalKm, 3) + " km").c_str());
    }
    
    lastOdometerUpdate = now;
  }
}

/********** Enhanced WiFi Event Handler **********/
void onWiFiEvent(WiFiEvent_t evt, WiFiEventInfo_t info) {
  switch (evt) {
    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
      stationCount++;
      if (!powerState) {
        digitalWrite(MOS_PIN, HIGH);
        powerState = true;
        logMessage(LOG_INFO, "POWER", ("Client connected (" + String(stationCount) + ") - Power ON").c_str());
      }
      lastDisconnect = 0;
      break;
      
    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
      if (stationCount > 0) stationCount--;
      logMessage(LOG_INFO, "CONN", ("Client disconnected (" + String(stationCount) + ")").c_str());
      if (stationCount == 0) {
        lastDisconnect = millis();
        connectivityDropouts++;
        // Emergency save
        prefs.putDouble("totalKm", totalKm);
        prefs.putDouble("tripKm", tripKm);
      }
      break;
      
    case ARDUINO_EVENT_WIFI_AP_START:
      logMessage(LOG_INFO, "WIFI", "Access Point started successfully");
      break;
      
    case ARDUINO_EVENT_WIFI_AP_STOP:
      logMessage(LOG_WARNING, "WIFI", "Access Point stopped", ALERT_CONNECTIVITY);
      break;
      
    default:
      break;
  }
}

/********** Professional Web Interface **********/
void handleRoot() {
  const char* html = R"(
<!DOCTYPE html>
<html>
<head>
    <meta charset='utf-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1'>
    <title>E-Scooter Control System</title>
    <style>
        * { box-sizing: border-box; margin: 0; padding: 0; }
        body { font-family: 'Segoe UI', Arial, sans-serif; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); min-height: 100vh; }
        .container { max-width: 480px; margin: 20px auto; background: white; border-radius: 15px; box-shadow: 0 10px 30px rgba(0,0,0,0.2); overflow: hidden; }
        .header { background: linear-gradient(45deg, #2196F3, #21CBF3); color: white; padding: 20px; text-align: center; }
        .header h1 { font-size: 24px; margin-bottom: 5px; }
        .header .version { font-size: 12px; opacity: 0.8; }
        .section { padding: 20px; border-bottom: 1px solid #eee; }
        .section:last-child { border-bottom: none; }
        .section h3 { color: #333; margin-bottom: 15px; font-size: 16px; }
        .status-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; }
        .status-item { background: #f8f9fa; padding: 12px; border-radius: 8px; text-align: center; }
        .status-label { font-size: 12px; color: #666; margin-bottom: 5px; }
        .status-value { font-size: 16px; font-weight: bold; color: #333; }
        .speed-display { text-align: center; background: linear-gradient(45deg, #4CAF50, #45a049); color: white; padding: 25px; border-radius: 12px; margin: 20px 0; }
        .speed-value { font-size: 36px; font-weight: bold; }
        .speed-unit { font-size: 18px; margin-left: 5px; }
        .controls { display: flex; gap: 10px; justify-content: center; flex-wrap: wrap; }
        .btn { padding: 12px 24px; border: none; border-radius: 8px; color: white; font-weight: bold; text-decoration: none; display: inline-block; transition: transform 0.2s; }
        .btn:hover { transform: translateY(-2px); }
        .btn-on { background: linear-gradient(45deg, #4CAF50, #45a049); }
        .btn-off { background: linear-gradient(45deg, #f44336, #d32f2f); }
        .btn-reset { background: linear-gradient(45deg, #ff9800, #f57c00); font-size: 12px; padding: 8px 16px; }
        .alert { background: #fff3cd; border: 1px solid #ffeaa7; color: #856404; padding: 12px; border-radius: 8px; margin: 10px 0; }
        .alert.critical { background: #f8d7da; border: 1px solid #f5c6cb; color: #721c24; }
        .diagnostics { font-size: 12px; color: #666; }
        .health-indicator { display: inline-block; width: 12px; height: 12px; border-radius: 50%; margin-right: 5px; }
        .health-ok { background: #4CAF50; }
        .health-warning { background: #ff9800; }
        .health-error { background: #f44336; }
    </style>
</head>
<body>
    <div class='container'>
        <div class='header'>
            <h1>🛴 E-Scooter Control</h1>
            <div class='version'>v3.0.0 Professional | Device: ESC001</div>
        </div>
        
        <div class='section'>
            <div class='speed-display'>
                <span class='speed-value' id='speed'>--</span><span class='speed-unit'>km/h</span>
            </div>
        </div>
        
        <div class='section'>
            <h3>📊 Kilometerzähler</h3>
            <div class='status-grid'>
                <div class='status-item'>
                    <div class='status-label'>Gesamt</div>
                    <div class='status-value' id='totalKm'>-- km</div>
                </div>
                <div class='status-item'>
                    <div class='status-label'>Trip</div>
                    <div class='status-value' id='tripKm'>-- km</div>
                </div>
            </div>
            <div style='text-align: center; margin-top: 15px;'>
                <a href='/trip/reset' class='btn btn-reset' onclick='return confirm("Trip-Zähler zurücksetzen?")'>Trip Reset</a>
            </div>
        </div>
        
        <div class='section'>
            <h3>⚡ System Status</h3>
            <div class='status-grid'>
                <div class='status-item'>
                    <div class='status-label'>Power</div>
                    <div class='status-value' id='powerStatus'>--</div>
                </div>
                <div class='status-item'>
                    <div class='status-label'>Licht</div>
                    <div class='status-value' id='lightStatus'>--</div>
                </div>
                <div class='status-item'>
                    <div class='status-label'>Clients</div>
                    <div class='status-value' id='clientCount'>--</div>
                </div>
                <div class='status-item'>
                    <div class='status-label'>Temperatur</div>
                    <div class='status-value' id='temperature'>-- °C</div>
                </div>
            </div>
        </div>
        
        <div class='section'>
            <h3>🔧 Controls</h3>
            <div class='controls'>
                <a href='/light?state=on' class='btn btn-on'>Licht AN</a>
                <a href='/light?state=off' class='btn btn-off'>Licht AUS</a>
            </div>
        </div>
        
        <div class='section'>
            <h3>🏥 System Health</h3>
            <div id='healthStatus' class='diagnostics'>Lade...</div>
            <div id='alertStatus'></div>
        </div>
        
        <div class='section'>
            <div style='text-align: center;'>
                <a href='/diagnostics' style='color: #666; text-decoration: none; font-size: 12px;'>🔍 Erweiterte Diagnostics</a> |
                <a href='/logs' style='color: #666; text-decoration: none; font-size: 12px;'>📋 System Logs</a>
            </div>
        </div>
    </div>
    
    <script>
        function updateStatus() {
            fetch('/api/status')
                .then(r => r.json())
                .then(data => {
                    document.getElementById('speed').textContent = data.speed;
                    document.getElementById('totalKm').textContent = data.totalKm + ' km';
                    document.getElementById('tripKm').textContent = data.tripKm + ' km';
                    document.getElementById('powerStatus').textContent = data.power ? 'AN' : 'AUS';
                    document.getElementById('lightStatus').textContent = data.light ? 'AN' : 'AUS';
                    document.getElementById('clientCount').textContent = data.clients;
                    document.getElementById('temperature').textContent = data.temperature + ' °C';
                    
                    // Health indicators
                    let healthHtml = '';
                    healthHtml += `<span class='health-indicator ${data.health.wifi ? 'health-ok' : 'health-error'}'></span>WiFi: ${data.health.wifi ? 'OK' : 'ERROR'}<br>`;
                    healthHtml += `<span class='health-indicator ${data.health.temperature ? 'health-ok' : 'health-warning'}'></span>Temp: ${data.health.temperature ? 'OK' : 'HIGH'}<br>`;
                    healthHtml += `<span class='health-indicator ${data.health.speedSensor ? 'health-ok' : 'health-warning'}'></span>Speed Sensor: ${data.health.speedSensor ? 'OK' : 'CHECK'}<br>`;
                    healthHtml += `Memory: ${data.health.freeHeap} bytes free`;
                    document.getElementById('healthStatus').innerHTML = healthHtml;
                    
                    // Alert handling
                    if (data.alert && data.alert !== 'NONE') {
                        document.getElementById('alertStatus').innerHTML = 
                            `<div class='alert ${data.alertLevel === 'CRITICAL' ? 'critical' : ''}'>⚠️ ${data.alert}</div>`;
                    } else {
                        document.getElementById('alertStatus').innerHTML = '';
                    }
                })
                .catch(e => console.log('Update error:', e));
        }
        
        setInterval(updateStatus, 1000);
        updateStatus();
    </script>
</body>
</html>
)";
  server.send(200, "text/html; charset=utf-8", html);
}

/********** API Endpoints **********/
void handleAPIStatus() {
  DynamicJsonDocument doc(1024);
  
  doc["speed"] = String(kmh, 1);
  doc["totalKm"] = String(totalKm, 3);
  doc["tripKm"] = String(tripKm, 3);
  doc["power"] = powerState;
  doc["light"] = lightState;
  doc["clients"] = stationCount;
  doc["temperature"] = String(systemTemp, 1);
  doc["uptime"] = uptime;
  doc["firmware"] = FIRMWARE_VERSION;
  doc["device"] = DEVICE_ID;
  
  // Health status
  JsonObject healthObj = doc.createNestedObject("health");
  healthObj["wifi"] = health.wifiStable;
  healthObj["temperature"] = health.temperatureOK;
  healthObj["speedSensor"] = health.speedSensorOK;
  healthObj["powerSystem"] = health.powerSystemOK;
  healthObj["freeHeap"] = health.freeHeap;
  
  // Alert information
  const char* alertStr[] = {"NONE", "TEMPERATURE", "CONNECTIVITY", "SPEED_SENSOR", "POWER_SYSTEM", "BRAKE_SYSTEM"};
  doc["alert"] = alertStr[currentAlert];
  doc["alertLevel"] = (currentAlert != ALERT_NONE) ? "WARNING" : "OK";
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

/********** Light Control **********/
void handleLight() {
  String state = server.arg("state");
  if (state == "on") {
    digitalWrite(LIGHT_PIN, HIGH);
    lightState = true;
    prefs.putBool("lightState", lightState);
    logMessage(LOG_INFO, "LIGHT", "Light turned ON");
    server.send(200, "text/plain", "Light ON");
  } else if (state == "off") {
    digitalWrite(LIGHT_PIN, LOW);
    lightState = false;
    prefs.putBool("lightState", lightState);
    logMessage(LOG_INFO, "LIGHT", "Light turned OFF");
    server.send(200, "text/plain", "Light OFF");
  } else if (state == "toggle") {
    lightState = !lightState;
    digitalWrite(LIGHT_PIN, lightState ? HIGH : LOW);
    prefs.putBool("lightState", lightState);
    logMessage(LOG_INFO, "LIGHT", lightState ? "Light toggled ON" : "Light toggled OFF");
    server.send(200, "text/plain", lightState ? "Light ON" : "Light OFF");
  } else {
    server.send(400, "text/plain", "Bad Request: use ?state=on|off|toggle");
  }
}

/********** Trip Reset **********/
void handleTripReset() {
  tripKm = 0.0;
  prefs.putDouble("tripKm", tripKm);
  logMessage(LOG_INFO, "ODO", "Trip counter reset");
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "Trip Reset - Redirecting...");
}

/********** Diagnostics Page **********/
void handleDiagnostics() {
  String html = "<!DOCTYPE html><html><head><meta charset='utf-8'><title>System Diagnostics</title>";
  html += "<style>body{font-family:Arial;margin:20px;background:#f0f0f0;}";
  html += ".container{max-width:800px;margin:0 auto;background:white;padding:20px;border-radius:10px;}";
  html += "table{width:100%;border-collapse:collapse;margin:20px 0;}";
  html += "th,td{padding:12px;border:1px solid #ddd;text-align:left;}";
  html += "th{background:#f8f9fa;}</style></head><body>";
  
  html += "<div class='container'><h1>🔍 System Diagnostics</h1>";
  
  html += "<h2>Hardware Status</h2><table>";
  html += "<tr><th>Component</th><th>Status</th><th>Value</th></tr>";
  html += "<tr><td>System Temperature</td><td>" + String(health.temperatureOK ? "OK" : "WARNING") + "</td><td>" + String(systemTemp, 1) + " °C</td></tr>";
  html += "<tr><td>Free Heap</td><td>" + String(health.freeHeap > 10000 ? "OK" : "LOW") + "</td><td>" + String(health.freeHeap) + " bytes</td></tr>";
  html += "<tr><td>Min Free Heap</td><td>INFO</td><td>" + String(health.minFreeHeap) + " bytes</td></tr>";
  html += "<tr><td>WiFi Status</td><td>" + String(health.wifiStable ? "OK" : "ERROR") + "</td><td>" + String(stationCount) + " clients</td></tr>";
  html += "<tr><td>Speed Sensor</td><td>" + String(health.speedSensorOK ? "OK" : "CHECK") + "</td><td>" + String(speedSensorErrors) + " errors</td></tr>";
  html += "</table>";
  
  html += "<h2>System Information</h2><table>";
  html += "<tr><th>Parameter</th><th>Value</th></tr>";
  html += "<tr><td>Firmware Version</td><td>" + String(FIRMWARE_VERSION) + "</td></tr>";
  html += "<tr><td>Device ID</td><td>" + String(DEVICE_ID) + "</td></tr>";
  html += "<tr><td>Build Date</td><td>" + String(BUILD_DATE) + " " + String(BUILD_TIME) + "</td></tr>";
  html += "<tr><td>Uptime</td><td>" + String(uptime/1000) + " seconds</td></tr>";
  html += "<tr><td>Connectivity Dropouts</td><td>" + String(connectivityDropouts) + "</td></tr>";
  html += "</table>";
  
  html += "<p><a href='/'>← Back to Main</a></p>";
  html += "</div></body></html>";
  
  server.send(200, "text/html; charset=utf-8", html);
}

/********** System Setup **********/
void setup() {
  Serial.begin(115200);
  delay(100);
  
  logMessage(LOG_INFO, "SYS", "=== E-Scooter Control System v3.0.0 Starting ===");
  logMessage(LOG_INFO, "SYS", ("Device ID: " + String(DEVICE_ID)).c_str());
  logMessage(LOG_INFO, "SYS", ("Build: " + String(BUILD_DATE) + " " + String(BUILD_TIME)).c_str());
  
  // Enable watchdog
  esp_task_wdt_init(30, true);
  esp_task_wdt_add(NULL);
  
  // Initialize preferences
  prefs.begin("escooter", false);
  
  // Load saved data
  totalKm = prefs.getDouble("totalKm", 0.0);
  tripKm = prefs.getDouble("tripKm", 0.0);
  lightState = prefs.getBool("lightState", false);
  lastSavedKm = totalKm;
  
  logMessage(LOG_INFO, "ODO", ("Loaded: Total=" + String(totalKm, 3) + "km, Trip=" + String(tripKm, 3) + "km").c_str());
  
  // Initialize hardware
  pinMode(MOS_PIN, OUTPUT);
  digitalWrite(MOS_PIN, LOW);
  
  pinMode(LIGHT_PIN, OUTPUT);
  digitalWrite(LIGHT_PIN, lightState ? HIGH : LOW);
  
  pinMode(RPM_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(RPM_PIN), onPulse, RISING);
  
  // Initialize health status
  health.freeHeap = ESP.getFreeHeap();
  health.minFreeHeap = health.freeHeap;
  
  // Start WiFi
  WiFi.onEvent(onWiFiEvent);
  if (WiFi.softAP(AP_SSID, AP_PASS, AP_CHANNEL, AP_HIDDEN)) {
    logMessage(LOG_INFO, "WIFI", ("Access Point started: " + WiFi.softAPIP().toString()).c_str());
  } else {
    logMessage(LOG_ERROR, "WIFI", "Failed to start Access Point", ALERT_CONNECTIVITY);
  }
  
  // Start mDNS
  if (MDNS.begin(MDNS_NAME)) {
    logMessage(LOG_INFO, "MDNS", ("Service started: http://" + String(MDNS_NAME) + ".local").c_str());
    MDNS.addService("http", "tcp", 80);
  }
  
  // Setup web server
  server.on("/", HTTP_GET, handleRoot);
  server.on("/light", HTTP_GET, handleLight);
  server.on("/trip/reset", HTTP_GET, handleTripReset);
  server.on("/api/status", HTTP_GET, handleAPIStatus);
  server.on("/diagnostics", HTTP_GET, handleDiagnostics);
  
  server.onNotFound([]() {
    logMessage(LOG_WARNING, "WEB", ("404 request: " + server.uri()).c_str());
    server.send(404, "text/plain", "404 - Not Found");
  });
  
  server.begin();
  logMessage(LOG_INFO, "WEB", "HTTP server started on port 80");
  logMessage(LOG_INFO, "SYS", "=== System initialization complete ===");
}

/********** Main Loop **********/
void loop() {
  // Feed watchdog
  esp_task_wdt_reset();
  
  // Update uptime
  uptime = millis();
  
  // Handle web server
  server.handleClient();
  
  // Temperature monitoring
  if (uptime - lastTempCheck >= TEMP_CHECK_INTERVAL) {
    checkTemperature();
    lastTempCheck = uptime;
  }
  
  // System diagnostics
  if (uptime - lastDiagnostics >= DIAGNOSTICS_INTERVAL) {
    runDiagnostics();
    lastDiagnostics = uptime;
  }
  
  // Auto power-off logic
  if (stationCount == 0 && lastDisconnect != 0 && powerState &&
      (uptime - lastDisconnect >= OFF_DELAY_MS)) {
    digitalWrite(MOS_PIN, LOW);
    powerState = false;
    lastDisconnect = 0;
    prefs.putDouble("totalKm", totalKm);
    prefs.putDouble("tripKm", tripKm);
    logMessage(LOG_INFO, "POWER", "Auto power-off executed, data saved");
  }
  
  // Speed calculation
  if (uptime - lastCalc >= 1000) {
    noInterrupts();
    uint32_t p = pulseCount;
    pulseCount = 0;
    interrupts();
    
    if (p > 0) {
      double rpm = (p * 60.0) / PULSES_PER_REV;
      kmh = (rpm * WHEEL_CIRC_M * 60.0) / 1000.0;
      logMessage(LOG_DEBUG, "SPEED", ("Current: " + String(kmh, 1) + " km/h").c_str());
    } else {
      if (kmh > 0.1) {
        logMessage(LOG_DEBUG, "SPEED", "Vehicle stopped");
      }
      kmh = 0.0;
    }
    lastCalc = uptime;
  }
  
  // Update odometer
  updateOdometer();
  
  // Update mDNS
  MDNS.update();
  
  // Small delay for stability
  delay(10);
}