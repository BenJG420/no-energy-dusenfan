# 3D-Druck Gehäuse für Motor-Controller Board

## Über dieses Gehäuse

Dieses 3D-druckbare Gehäuse wurde speziell für Motor-Controller-Boards mit folgenden Eigenschaften entwickelt:
- Dual H-Bridge Motor Driver
- Zwei 330µF Kondensatoren
- Grüne Schraubklemmblöcke für Motoranschlüsse
- Kompatibel mit L298N-ähnlichen Boards

## Dateien

- `motor_controller_case.scad` - Komplette Ansicht aller Teile
- `motor_controller_case_parts.scad` - Einzelne Teile zum separaten Drucken

## Gehäuse-Eigenschaften

### Abmessungen
- **Externe Maße:** 74 x 65 x 17 mm
- **Interne Maße:** 69 x 60 x 15 mm
- **Wandstärke:** 2,5 mm

### Features
- ✅ Aussparungen für Schraubklemmblöcke
- ✅ Belüftungslöcher im Deckel
- ✅ Montageposten für PCB-Befestigung
- ✅ Gummifüße für stabilen Stand
- ✅ Beschriftungsfeld im Deckel
- ✅ Kabelauslässe für alle Anschlüsse

## 3D-Druck Einstellungen

### Empfohlene Druckparameter
```
Schichthöhe:     0.2 mm
Füllung:         20%
Druckgeschwindigkeit: 50 mm/s
Stützstrukturen: NICHT erforderlich
Haftung:         Brim empfohlen
```

### Material
- **PLA:** Ideal für den Hausgebrauch
- **PETG:** Bessere Hitzebeständigkeit
- **ABS:** Für professionelle Anwendungen

### Druckreihenfolge
1. **Gehäuse-Unterteil** (motor_controller_case_parts.scad - Teil 1)
2. **Deckel** (motor_controller_case_parts.scad - Teil 2) 
3. **Gummifüße** (motor_controller_case_parts.scad - Teil 3, optional)

## Benötigte Schrauben

### PCB-Montage
- **4x M3 x 10mm** Senkkopfschrauben oder Zylinderkopfschrauben
- Alternativ: Selbstschneidende Schrauben 2,9 x 9,5mm

### Gehäuse-Verschluss (optional)
- **4x M3 x 16mm** Schrauben für permanenten Verschluss
- Oder Snap-Fit Verschluss verwenden

## Montage-Anleitung

### Schritt 1: Vorbereitung
1. Alle gedruckten Teile auf Qualität prüfen
2. Löcher bei Bedarf nachbohren (3,2mm für M3)
3. Grate entfernen

### Schritt 2: PCB einsetzen
1. Board vorsichtig in das Gehäuse einsetzen
2. Darauf achten, dass die Schraubklemmblöcke durch die Aussparungen ragen
3. Board mit M3 Schrauben an den Montagepfosten befestigen

### Schritt 3: Kabelanschlüsse
1. Motorkabel an die grünen Schraubklemmen anschließen
2. Stromversorgung anschließen
3. Steuersignale anschließen

### Schritt 4: Gehäuse schließen
1. Deckel aufsetzen
2. Bei Bedarf mit Schrauben fixieren
3. Gummifüße aufkleben oder einclipsen

## Anpassungen

### Abmessungen ändern
Falls Ihr Board andere Maße hat, bearbeiten Sie folgende Werte in der .scad Datei:

```scad
board_length = 65;     // Platinenlänge in mm
board_width = 56;      // Platinenbreite in mm
capacitor_height = 12; // Kondensator-Höhe
terminal_height = 8;   // Klemmenblock-Höhe
```

### Zusätzliche Aussparungen
Für weitere Anschlüsse können Sie zusätzliche Cutouts hinzufügen:

```scad
// Beispiel für LED-Aussparung
translate([position_x, position_y, position_z]) {
    cylinder(d=led_diameter, h=wall_thickness + 2);
}
```

## Fehlerbehebung

### Problem: Board passt nicht
- **Lösung:** Überprüfen Sie die Abmessungen und passen Sie sie in der .scad Datei an

### Problem: Schraubklemmblöcke zu eng
- **Lösung:** Vergrößern Sie `terminal_width` und `terminal_height`

### Problem: Deckel sitzt nicht richtig
- **Lösung:** Prüfen Sie die Druckqualität und entfernen Sie Stützreste

### Problem: Montageposten zu lose/fest
- **Lösung:** Anpassen von `screw_hole_diameter` (Standard: 3.2mm für M3)

## Sicherheitshinweise

⚠️ **Wichtige Hinweise:**
- Gehäuse bietet keinen Schutz vor Hochspannung
- Für Spannungen über 24V zusätzliche Isolierung verwenden
- Belüftung nicht blockieren
- Bei hohen Strömen (>10A) zusätzliche Kühlung vorsehen

## Wartung

- Belüftungslöcher regelmäßig von Staub befreien
- Bei Bedarf mit Druckluft reinigen
- Schrauben nach längerer Nutzung nachziehen

## Erweiterungen

### Mögliche Upgrades:
- **Kühlrippen** für bessere Wärmeableitung
- **Status-LEDs** mit entsprechenden Lichtleitern
- **DIN-Rail Montage** für industrielle Anwendungen
- **Wasserdichte Version** mit Dichtungen

## Support

Bei Fragen oder Problemen:
1. Überprüfen Sie die Abmessungen Ihres Boards
2. Kontrollieren Sie die Druckeinstellungen
3. Messen Sie die gedruckten Teile nach

## Lizenz

Dieses Design steht unter Creative Commons Attribution 4.0 License.
Sie dürfen es verwenden, modifizieren und weitergeben.