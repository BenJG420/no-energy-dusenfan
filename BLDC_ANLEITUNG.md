# 3D-Druck Gehäuse für BLDC Motor Controller

## ⚡ HOCHLEISTUNGS-CONTROLLER GEHÄUSE ⚡

Dieses Gehäuse ist speziell für **6-60V 400W BLDC (Brushless DC) Motor Controller** entwickelt worden, wie sie auf AliExpress verkauft werden.

## 🚨 WICHTIGE SICHERHEITSHINWEISE

⚠️ **HOCHSTROM-ANWENDUNG - BESONDERE VORSICHT ERFORDERLICH**

- **Bis zu 400W Leistung** - Massive Wärmeentwicklung möglich!
- **Spannungen bis 60V** - Gefährliche Spannung!
- **Ströme bis 15A** - Hohe Brandgefahr bei schlechten Verbindungen!
- **Dreiphasen-Ausgang** - Komplexe Verkabelung erforderlich!

## Board-Spezifikationen

### Abmessungen (laut Hersteller)
- **Platine:** 63 x 42.5 x 17mm
- **Betriebsspannung:** DC 6-60V 
- **Betriebsstrom:** ≤15A
- **Ausgangsleistung:** ≤400W
- **Geschwindigkeitssteuerung:** 0.1V-5V oder PWM

### Anschlüsse
- **VCC/GND:** Stromversorgung (6-60V)
- **MA/MB/MC:** 3-Phasen Motorausgang (U/V/W)
- **Hall-Sensoren:** 5V, HA, HB, HC, GND
- **Steuerung:** VR (Speed), ZF (Direction), EL (Enable)

## Gehäuse-Features

### Optimiert für Hochleistung
- ✅ **Verstärkte Kühlung** mit seitlichen Belüftungsschlitzen
- ✅ **Dickere Wände** (2.8mm) für bessere Wärmeableitung
- ✅ **Kühlrippen** am Boden des Gehäuses
- ✅ **Große Belüftungslöcher** im Deckel (4mm)
- ✅ **Wärmeableiter-Adapterplatte** inklusive
- ✅ **DIN-Rail Montage** möglich

### Abmessungen
- **Externe Maße:** 77 x 55 x 20 mm
- **Interne Maße:** 69 x 49 x 14 mm
- **Gewicht gedruckt:** ~80-120g (je nach Material)

## Material-Empfehlungen

### ⭐ EMPFOHLEN für Hochleistung:
1. **PETG** - Beste Wahl für 400W Controller
   - Temperaturbeständig bis 80°C
   - Chemisch beständig
   - Gute mechanische Eigenschaften

2. **ABS** - Für industrielle Anwendungen
   - Temperaturbeständig bis 100°C
   - Sehr robust
   - Nachbearbeitung möglich

### ❌ NICHT EMPFOHLEN:
- **PLA** - Nur für Tests unter 50W!
  - Erweicht ab 60°C
  - Ungeeignet für Hochleistung

## 3D-Druck Einstellungen (KRITISCH!)

### Druckparameter
```
Material:           PETG oder ABS
Schichthöhe:        0.2mm
Füllung:            25% (Minimum für Wärmeableitung!)
Außenwände:         3 Perimeter (Minimum)
Boden/Deckel:       5 Schichten
Druckgeschwindigkeit: 40 mm/s (langsamer für Qualität)
Stützstrukturen:    NICHT erforderlich
```

### Temperatureinstellungen
**PETG:**
- Extruder: 230-250°C
- Druckbett: 70-80°C

**ABS:**
- Extruder: 240-260°C  
- Druckbett: 90-100°C
- Gehäuse: Empfohlen

## Montage-Anleitung

### Schritt 1: Sicherheitsprüfung
1. **Stromversorgung trennen** - IMMER!
2. **Multimeter bereithalten** für Durchgangsprüfung
3. **Antistatik-Maßnahmen** beachten
4. **Arbeitsplatz räumen** - keine brennbaren Materialien

### Schritt 2: Controller vorbereiten
1. Board auf Beschädigungen prüfen
2. **MOSFETs identifizieren** (meist auf Rückseite)
3. **Wärmeleitpads bereitlegen** (1-2mm dick)
4. **Anschlusspläne studieren** - falsche Verkabelung = Defekt!

### Schritt 3: Wärmemanagement
1. **Wärmeleitpads** auf MOSFETs kleben
2. **Thermische Verbindung** zum Gehäuse herstellen
3. **Externe Wärmeableiter** bei >200W Dauerbetrieb

### Schritt 4: Einbau
1. Board vorsichtig einsetzen
2. **Alle Anschlüsse** durch Aussparungen führen
3. Mit **M3 x 12mm Schrauben** befestigen
4. **Anzugsmoment beachten** - nicht überdrehen!

### Schritt 5: Verkabelung
⚠️ **ACHTUNG: Falsche Verkabelung kann Controller zerstören!**

**Stromversorgung:**
- VCC: Positive Versorgung (6-60V)
- GND: Masse (dickste verfügbare Kabel!)

**Motoranschlüsse:**
- MA → Motor Phase U
- MB → Motor Phase V  
- MC → Motor Phase W

**Hall-Sensoren:**
- 5V → Hall-Versorgung
- HA/HB/HC → Hall-Signale
- GND → Hall-Masse

**Steuerung:**
- VR: Geschwindigkeit (0-5V oder Potentiometer)
- ZF: Richtung (GND oder 5V)
- EL: Enable (GND=Stop, 5V=Start)

### Schritt 6: Test vor Verschluss
1. **Sichtprüfung** aller Verbindungen
2. **Durchgangsprüfung** mit Multimeter
3. **Testlauf ohne Last** bei niedriger Spannung
4. **Temperatur überwachen** in ersten 10 Minuten

### Schritt 7: Gehäuse schließen
1. Deckel aufsetzen
2. **Belüftung freihalten** - niemals blockieren!
3. Mit M3 x 20mm Schrauben fixieren

## Wärmemanagement - KRITISCH!

### Kühlungsmaßnahmen erforderlich bei:
- **>100W Dauerbetrieb:** Zusätzlicher Lüfter
- **>200W Dauerbetrieb:** Externer Kühlkörper
- **>300W Dauerbetrieb:** Aktive Wasserkühlung
- **Umgebungstemperatur >30°C:** Immer zusätzliche Kühlung

### Temperaturüberwachung
- **MOSFETs:** Max. 85°C
- **Gehäuse:** Max. 60°C fühlbar
- **Bei Überhitzung:** Sofort abschalten!

## Fehlerdiagnose

### Problem: Controller startet nicht
- **Lösung:** Hall-Sensor Reihenfolge prüfen
- **Achtung:** Falsche Reihenfolge kann Schäden verursachen!

### Problem: Motor läuft unrund
- **Lösung:** Phasenfolge U/V/W überprüfen
- **Tipp:** Eine Phase vertauschen

### Problem: Überhitzung
- **Sofortmaßnahme:** Leistung reduzieren
- **Langfristig:** Kühlung verbessern

### Problem: Hoher Strom ohne Last
- **GEFAHR:** Sofort abschalten!
- **Ursache:** Meist falsche Hall-Verkabelung

## Erweiterte Features

### DIN-Rail Montage
Das Gehäuse kann optional mit DIN-Rail Clips ausgestattet werden:
1. Clips separat drucken
2. Mit M3 Schrauben am Gehäuse befestigen
3. Auf Standard 35mm DIN-Rail montieren

### Externe Kühlung
Bei hohen Dauerlasten:
1. **Kühlkörper-Adapter** verwenden
2. **Standard PC-Kühlkörper** montierbar
3. **Lüfter** für Zwangsbelüftung

### Status-LEDs
- **LED-Fenster** im Gehäuse vorhanden
- **Status-LED** des Controllers sichtbar
- **Zusätzliche LEDs** nachrüstbar

## Sicherheits-Checkliste

### Vor Inbetriebnahme:
- [ ] Alle Verbindungen fest
- [ ] Keine Kurzschlüsse vorhanden
- [ ] Belüftung frei
- [ ] Sicherung in Hauptleitung
- [ ] Notausschalter erreichbar
- [ ] Thermometer bereit

### Während Betrieb:
- [ ] Temperatur überwachen
- [ ] Ungewöhnliche Geräusche beachten
- [ ] Vibrationsfrei montiert
- [ ] Niemals unbeaufsichtigt >100W

### Bei Problemen:
- [ ] Sofort Spannung trennen
- [ ] Abkühlen lassen
- [ ] Ursache ermitteln
- [ ] Nicht weiter betreiben bis Fehler behoben

## Gesetzliche Hinweise

⚠️ **WICHTIG:**
- Controller ist **NICHT CE-zertifiziert**
- **Nur für Experimentierbetrieb** geeignet
- **Nicht für kommerzielle Anwendungen**
- **EMV-Störungen** möglich
- **Eigenverantwortung** des Nutzers

## Wartung

### Regelmäßig prüfen:
- **Belüftung** von Staub befreien
- **Schraubverbindungen** nachziehen
- **Wärmeleitpads** erneuern (jährlich bei Hochlast)
- **Anschlüsse** auf Korrosion prüfen

## Support und Haftung

**WICHTIGER HINWEIS:**  
Dieses Gehäuse ist für **Hobbyanwendungen und Experimente** gedacht. Der Nutzer trägt die **volle Verantwortung** für sichere Installation und Betrieb.

**Keine Haftung** für:
- Sachschäden durch Überhitzung
- Personenschäden durch unsachgemäße Nutzung  
- Folgeschäden durch Fehlfunktionen
- EMV-Störungen oder Interferenzen

**Bei Unsicherheiten:**
- Fachmann konsultieren
- Niedrigere Leistung verwenden
- Zusätzliche Sicherheitsmaßnahmen treffen