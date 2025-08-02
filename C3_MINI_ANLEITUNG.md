# 3D-Druck Gehäuse für Carenuity C3 Mini v2.2.1

## 📱 KOMPAKTES IoT-GEHÄUSE

Dieses Gehäuse ist speziell für das **Carenuity C3 Mini v2.2.1 ESP32-C3 IoT Board** entwickelt - ein ultra-kompaktes WiFi + Bluetooth LE Development Board.

## 🔧 Board-Spezifikationen

### Technische Daten
- **Chip:** ESP32-C3 RISC-V Single-Core CPU (160 MHz)
- **Abmessungen:** 34.3 × 25.4 mm (sehr kompakt!)
- **Gewicht:** 26g
- **Flash:** 4 MB
- **Betriebsspannung:** 3.3V
- **Digital I/O:** 12 Pins
- **Konnektivität:** WiFi & Bluetooth LE

### Besondere Features des Boards
- **USB-C Anschluss** für Programmierung und Stromversorgung
- **WS2812B RGB LED** integriert
- **PCB Antenne** für WiFi/Bluetooth
- **Pin Headers** für Sensoren und Erweiterungen
- **Kompaktes Design** für IoT-Projekte

## 📦 Gehäuse-Features

### 🎯 Optimiert für IoT-Anwendungen:
- ✅ **Ultra-kompakt** - nur 41.3 x 31.4 x 16.5 mm
- ✅ **USB-C Zugang** für einfache Programmierung
- ✅ **Pin Header Zugang** auf beiden Seiten
- ✅ **WiFi-Antenne Bereich** frei von Metall
- ✅ **RGB LED Fenster** für Status-Anzeige
- ✅ **Wandmontage-Halterung** inklusive
- ✅ **Sensor-Shield Adapter** für Erweiterungen
- ✅ **Kabelmanagement-Clips** integriert

### Abmessungen
- **Externe Maße:** 41.3 x 31.4 x 16.5 mm
- **Interne Maße:** 37.3 x 27.4 x 12.5 mm
- **Board-Platz:** 34.3 x 25.4 mm (perfekt passend)
- **Gewicht:** ~25-35g gedruckt

## 🎨 Material-Empfehlungen

### ⭐ EMPFOHLEN für IoT-Projekte:
1. **PLA** - Ideal für Innenanwendungen
   - Einfach zu drucken
   - Ausreichend für IoT-Temperaturen
   - Günstig und umweltfreundlich

2. **PETG** - Für robuste Anwendungen
   - Höhere Temperaturbeständigkeit
   - Chemisch beständiger
   - Transparente Varianten verfügbar

### 🔄 Für spezielle Anwendungen:
- **ABS** - Outdoor-Anwendungen
- **TPU (flexible Bereiche)** - Stoßdämpfung
- **Transparent PETG** - LED-Effekte sichtbar

## 🖨️ 3D-Druck Einstellungen

### Druckparameter (Wichtig für kleine Details!)
```
Material:           PLA oder PETG
Schichthöhe:        0.15-0.2mm (feine Details!)
Füllung:            15-20%
Außenwände:         2-3 Perimeter
Druckgeschwindigkeit: 40-50 mm/s
Stützstrukturen:    NICHT erforderlich
```

### Qualitätseinstellungen
- **Düsengröße:** 0.4mm (Standard)
- **Erste Schicht:** Langsam und sorgfältig
- **Überhänge:** Alle Überhänge unter 45°
- **Brücken:** Kurze Distanzen, gut druckbar

## 🔨 Montage-Anleitung

### Schritt 1: Teile vorbereiten
1. **Alle Teile drucken:**
   - Gehäuse-Unterteil mit Füßen
   - Deckel
   - Wandmontage-Halterung (optional)
   - Sensor-Shield Adapter (optional)

2. **Nachbearbeitung:**
   - Grate entfernen
   - Löcher bei Bedarf nachbohren (2.5mm)
   - Oberflächen glätten

### Schritt 2: Board vorbereiten
1. **Carenuity C3 Mini** aus Metallrahmen lösen (Anleitung beachten!)
2. **Pin Headers** löten falls gewünscht
3. **Funktionstest** vor Einbau durchführen

### Schritt 3: Einbau
1. Board vorsichtig in Gehäuse einsetzen
2. **USB-C Anschluss** durch Aussparung führen
3. **Antenne-Bereich** frei lassen (kein Metall!)
4. Mit **M2.5 x 8mm Schrauben** befestigen

### Schritt 4: Anschlüsse
⚠️ **Vorsicht bei 3.3V Logik - nicht mit 5V verbinden!**

**Pin Header Anschlüsse:**
- **3.3V:** Stromversorgung für Sensoren
- **GND:** Masse
- **GPIO Pins:** Frei konfigurierbar
- **I2C:** Flexibel zuweisbar (z.B. GPIO8/GPIO10)

### Schritt 5: Gehäuse schließen
1. **RGB LED** durch Fenster sichtbar?
2. **Alle Anschlüsse** zugänglich?
3. Deckel aufsetzen und verschließen

## 📡 WiFi-Optimierung

### Antenne-Bereich (KRITISCH!)
- **Metall-freie Zone** um PCB-Antenne
- **Mindestabstand 5mm** zu metallischen Objekten
- **Gehäuse-Aussparung** für optimale Reichweite
- **Positionierung** beachten für beste Performance

### WiFi-Reichweite verbessern:
1. **Gehäuse-Orientierung** zur Basis-Station
2. **Externe Antenne** über U.FL-Anschluss (falls vorhanden)
3. **Metallfrei** in alle Richtungen
4. **Erhöhte Position** für bessere Abdeckung

## 🔌 Erweiterungen und Add-Ons

### Sensor-Shield Adapter
Mit dem mitgelieferten Adapter können Sie zusätzliche Sensorboards montieren:
- **Temperatur/Feuchtigkeit** (SHT40, DHT22)
- **Luftqualität** (CO2, TVOC)
- **Bewegung** (PIR, Radar)
- **Umwelt** (Licht, Druck)

### Wandmontage-System
- **Einfache Installation** mit 6mm Dübeln
- **Kabelführung** integriert
- **Sichere Befestigung** auch bei Vibrationen
- **Demontage** ohne Werkzeug möglich

### Kabelmanagement
- **Clips** für Sensor-Kabel
- **Zugentlastung** für USB-Kabel
- **Ordentliche Verlegung** auch bei vielen Anschlüssen

## 💡 IoT-Anwendungen

### Typische Projekte:
1. **Umwelt-Monitoring:**
   - Temperatur/Luftfeuchtigkeit
   - Luftqualität (CO2, TVOC)
   - Licht- und UV-Messung

2. **Smart Home Integration:**
   - MQTT-Sensoren
   - Home Assistant Integration
   - Apple HomeKit/Matter Support

3. **Sicherheits-Anwendungen:**
   - Bewegungsmelder
   - Tür/Fenster-Kontakte
   - Einbruchswarnung

4. **Energie-Monitoring:**
   - Stromverbrauchsmessung
   - Solar-Monitoring
   - Batterieüberwachung

5. **Industrielle Anwendungen:**
   - Maschinen-Überwachung
   - Predictive Maintenance
   - Datenlogger

## 📱 Software-Unterstützung

### Programming Platforms:
- **Arduino IDE** (empfohlen für Einsteiger)
- **ESP-IDF** (für Fortgeschrittene)
- **MicroPython** (Python-basiert)
- **ESPHome** (Home Assistant Integration)

### Carenuity Ecosystem:
- **Mini Sensor Shields** kompatibel
- **3-Minuten Setup** via QR-Code
- **Cloud Integration** verfügbar
- **OTA Updates** unterstützt

## 🔋 Stromversorgung

### USB-C Betrieb:
- **5V USB-C** für Entwicklung und Test
- **Dauerbetrieb** möglich
- **Geringe Stromaufnahme** (~80mA aktiv)

### Batterie-Betrieb:
- **3.3V LiPo** direkt anschließbar
- **Deep Sleep** für lange Laufzeit
- **Wake-up** via Timer oder Sensor
- **Energiespar-Modi** verfügbar

## 📊 Leistungsdaten

### Stromverbrauch:
- **Aktiv (WiFi):** ~80-120mA
- **Idle:** ~15-20mA  
- **Deep Sleep:** ~10µA
- **Ultra Deep Sleep:** ~2.5µA

### WiFi Performance:
- **Reichweite:** 50-100m (Freifeld)
- **Durchsatz:** bis 150 Mbps
- **Frequenz:** 2.4 GHz
- **Standards:** 802.11 b/g/n

## 🛠️ Wartung und Updates

### Regelmäßige Wartung:
- **Staub entfernen** aus Belüftungsschlitzen
- **Anschlüsse prüfen** auf Korrosion
- **WiFi-Performance** überwachen
- **Firmware Updates** via OTA

### Troubleshooting:
1. **Keine WiFi-Verbindung:**
   - Antenne-Bereich prüfen
   - Gehäuse-Position ändern
   - Channel-Einstellungen prüfen

2. **RGB LED nicht sichtbar:**
   - Fenster reinigen
   - LED-Pin Konfiguration prüfen
   - Transparente Filament verwenden

3. **Überhitzung:**
   - Belüftung verbessern
   - Stromverbrauch reduzieren
   - Umgebungstemperatur senken

## 🌡️ Betriebsbedingungen

### Umgebungsbedingungen:
- **Temperatur:** 0°C bis +70°C (PLA)
- **Luftfeuchtigkeit:** bis 85% rF
- **Schutzart:** IP20 (Indoor)
- **Vibration:** Begrenzt (IoT-Anwendungen)

### Für Outdoor-Anwendungen:
- **PETG oder ABS** verwenden
- **Zusätzliche Dichtungen** einbauen
- **UV-Schutz** vorsehen
- **Kondensatschutz** berücksichtigen

## 💰 Kostenschätzung

### Grundausstattung:
| Kategorie | Kosten |
|-----------|--------|
| Filament (30g PLA) | €1-2 |
| M2.5 Schrauben | €2-3 |
| Carenuity C3 Mini | €4-5 |
| **Gesamt Basic** | **€7-10** |

### Vollausstattung mit Sensoren:
| Kategorie | Kosten |
|-----------|--------|
| Grundausstattung | €7-10 |
| Temperatur/Feuchte Sensor | €3-8 |
| Wandmontage Hardware | €2-5 |
| **Gesamt erweitert** | **€12-23** |

## 🚀 Erste Schritte

### Quick Start:
1. **Board aus Rahmen lösen** (Video-Anleitung folgen)
2. **Arduino IDE** installieren und ESP32 Board Support hinzufügen
3. **Erstes Programm** mit Blink-LED hochladen
4. **WiFi-Connection** testen
5. **In Gehäuse einbauen** und erste IoT-Anwendung starten

### Community Support:
- **Carenuity Forum** für spezifische Fragen
- **ESP32 Arduino Community** für allgemeine Hilfe
- **Hackster.io** für Projektinspiration
- **GitHub** für Open Source Projekte

## 📝 Wichtige Hinweise

### Erste Verwendung:
- **Metallrahmen entfernen** vor Gehäuse-Einbau
- **Antistatic-Schutz** verwenden
- **Pin-Header** optional - je nach Anwendung
- **Test vor Einbau** empfohlen

### Lizenz und Rechtliches:
- Gehäuse-Design unter **Creative Commons CC BY-SA**
- **Carenuity™** ist Marke von ChipGlobe GmbH
- **ESP32-C3** ist Marke von Espressif Systems
- **Nur für private/experimentelle Nutzung**

Viel Erfolg mit Ihrem IoT-Projekt! 🌐📡