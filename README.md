# 120mm PC-Lüfter Ansaugstück und Diffusor

Dieses Projekt enthält 3D-Druckvorlagen für einen 120mm PC-Lüfter:
- **Ansaugstück (Intake)**: Verbessert die Luftzufuhr zum Lüfter
- **Diffusor**: Reduziert Geräusche und verteilt die Luft gleichmäßig

## 📁 Dateien

### STL-Dateien (3D-Druck bereit)
- `120mm_fan_intake.stl` - Ansaugstück
- `120mm_fan_diffuser.stl` - Diffusor

### Quellcode
- `120mm_fan_intake.scad` - OpenSCAD Quellcode für Ansaugstück
- `120mm_fan_diffuser.scad` - OpenSCAD Quellcode für Diffusor
- `generate_stl.py` - Python Alternative (benötigt cadquery)

## 🔧 Technische Spezifikationen

### Ansaugstück
- **Kompatibilität**: Standard 120mm PC-Lüfter (120x120x25mm)
- **Montagelöcher**: 105mm Abstand, 4.5mm Durchmesser (für M4 Schrauben)
- **Ansaugöffnung**: 140mm Durchmesser
- **Länge**: 60mm
- **Wandstärke**: 2.5mm
- **Material**: PLA/PETG empfohlen

### Diffusor
- **Kompatibilität**: Standard 120mm PC-Lüfter (120x120x25mm)
- **Montagelöcher**: 105mm Abstand, 4.5mm Durchmesser (für M4 Schrauben)
- **Ausgangsöffnung**: 160mm Durchmesser
- **Länge**: 50mm
- **Wandstärke**: 2.0mm
- **Besonderheit**: Integriertes Luftleitgitter zur Strömungsoptimierung

## 🖨️ 3D-Druck Einstellungen

### Empfohlene Einstellungen
- **Schichthöhe**: 0.2mm - 0.3mm
- **Infill**: 15-25%
- **Wanddicke**: 2-3 Perimeter
- **Support**: Nicht erforderlich
- **Druckgeschwindigkeit**: 50-80 mm/s
- **Düsentemperatur**: 
  - PLA: 200-220°C
  - PETG: 230-250°C
- **Heizbett**: 
  - PLA: 60°C
  - PETG: 80°C

### Druckorientierung
- **Ansaugstück**: Lüfterseite nach unten (Montageflansch auf dem Druckbett)
- **Diffusor**: Lüfterseite nach unten (Montageflansch auf dem Druckbett)

### Material-Empfehlungen
1. **PLA**: Einfach zu drucken, für den Innenbereich geeignet
2. **PETG**: Höhere Temperaturbeständigkeit, mechanisch stabiler
3. **ABS**: Für industrielle Anwendungen (mit Gehäuse drucken)

## 🔩 Montage

### Benötigte Schrauben
- 4x M4 Schrauben (Länge: 30-40mm)
- Optional: 4x M4 Unterlegscheiben

### Montageschritte

#### Ansaugstück
1. Ansaugstück mit der Flanschseite gegen den Lüfter ausrichten
2. M4 Schrauben durch die Montagelöcher des Ansaugstücks führen
3. Schrauben in die Lüftergewinde eindrehen
4. Gleichmäßig anziehen (nicht überdrehen!)

#### Diffusor
1. Diffusor mit der Flanschseite gegen die Ausgangsseite des Lüfters ausrichten
2. M4 Schrauben durch die Montagelöcher des Diffusors führen
3. Schrauben in die Lüftergewinde eindrehen
4. Gleichmäßig anziehen

### ⚠️ Wichtige Hinweise
- Vor der Montage prüfen, ob die Lüfterblätter frei drehen können
- Schrauben nicht zu fest anziehen (Gefahr von Rissen im 3D-Druck)
- Bei der ersten Montage Passform überprüfen

## 🎯 Anwendungsbereiche

### Ansaugstück
- Verbesserte Luftzufuhr bei beengten Platzverhältnissen
- Schutz vor Staub und größeren Partikeln
- Optimierte Strömungsführung

### Diffusor
- Geräuschreduzierung durch gleichmäßige Luftverteilung
- Verringerung von Turbulenzen
- Schutz nachgelagerter Komponenten

## 🛠️ Anpassungen

### Parameter ändern
Die OpenSCAD-Dateien enthalten übersichtlich strukturierte Parameter am Anfang der Datei:

```scad
fan_size = 120;              // Lüftergröße
intake_outer_diameter = 140;  // Ansaugdurchmesser
wall_thickness = 2.5;        // Wandstärke
```

### Modifikationen
- Ansaugdurchmesser anpassen für spezielle Anwendungen
- Wandstärke ändern für andere Materialien
- Länge der Bauteile modifizieren

## 📐 Abmessungen

### Ansaugstück
- Gesamtlänge: 60mm
- Lüfteranschluss: 120x120mm
- Ansaugöffnung: ⌀140mm
- Gewicht (PLA): ~85g

### Diffusor
- Gesamtlänge: 50mm
- Lüfteranschluss: 120x120mm
- Ausgangsöffnung: ⌀160mm
- Gewicht (PLA): ~70g

## 🔄 Wartung

- Regelmäßige Reinigung mit Druckluft
- Bei starker Verschmutzung mit warmen Seifenwasser reinigen
- Keine aggressiven Lösungsmittel verwenden
- Auf Risse oder Verschleiß prüfen

## 📝 Lizenz

Dieses Design steht unter Creative Commons Attribution-ShareAlike 4.0 International License.
Sie dürfen das Design frei verwenden, modifizieren und weitergeben.

## 🐛 Problembehebung

### Häufige Probleme
1. **Montagelöcher zu klein**: Mit 4.5mm Bohrer nachbearbeiten
2. **Lüfter passt nicht**: Parameter in SCAD-Datei anpassen
3. **Stringing**: Retraction-Einstellungen optimieren
4. **Warping**: Heizbett-Temperatur erhöhen

### Support
Bei Problemen oder Verbesserungsvorschlägen erstellen Sie bitte ein Issue im Repository.

---

**Viel Erfolg beim Drucken und Verwenden Ihrer neuen Lüfterzubehörteile! 🌪️** 
