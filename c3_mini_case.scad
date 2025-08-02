// Carenuity C3 Mini v2.2.1 Case - 3D Printable
// Designed for ESP32-C3 IoT Development Board
// Board dimensions: 34.3 x 25.4 mm

// Main board dimensions (from specifications)
board_length = 34.3;   // Board length in mm
board_width = 25.4;    // Board width in mm  
board_thickness = 1.6; // PCB thickness in mm

// Case dimensions
case_wall = 2.0;       // Wall thickness (thinner for compact design)
case_floor = 2.0;      // Bottom thickness
case_ceiling = 2.0;    // Top thickness
case_clearance = 4;    // Space above components

// Component heights (estimated for C3 Mini)
esp32_height = 3;      // ESP32-C3 chip height
usb_connector_height = 3.5; // USB-C connector
led_height = 2;        // WS2812B RGB LED
pin_header_height = 8.5; // Female pin headers (when soldered)

// Calculate internal dimensions
internal_length = board_length + 3; // Compact clearance
internal_width = board_width + 3;
internal_height = max(esp32_height, usb_connector_height, pin_header_height) + case_clearance;

// External case dimensions
external_length = internal_length + (case_wall * 2);
external_width = internal_width + (case_wall * 2);
external_height = internal_height + case_floor + case_ceiling;

// Mounting post dimensions
post_diameter = 5;
post_height = case_floor + 1.5;
screw_hole_diameter = 2.5; // M2.5 screw (smaller for compact board)

// Connector cutout dimensions
usb_cutout_width = 12;
usb_cutout_height = 5;
pin_header_width = 20;
pin_header_height = 3;

module main_case() {
    difference() {
        // Outer case
        cube([external_length, external_width, external_height]);
        
        // Internal cavity
        translate([case_wall, case_wall, case_floor]) {
            cube([internal_length, internal_width, internal_height + 1]);
        }
        
        // USB-C connector cutout (end of board)
        translate([-1, case_wall + (internal_width - usb_cutout_width)/2, case_floor + 1]) {
            cube([case_wall + 2, usb_cutout_width, usb_cutout_height]);
        }
        
        // Pin header access (both sides)
        translate([case_wall + 5, -1, case_floor + 1]) {
            cube([pin_header_width, case_wall + 2, pin_header_height]);
        }
        translate([case_wall + 5, external_width - case_wall - 1, case_floor + 1]) {
            cube([pin_header_width, case_wall + 2, pin_header_height]);
        }
        
        // Antenna area (keep clear from metal)
        translate([case_wall + 25, case_wall + 5, case_floor]) {
            cube([7, 15, internal_height + 1]);
        }
        
        // Status LED window
        translate([case_wall + 15, case_wall + 12, external_height - case_ceiling]) {
            cylinder(d=3, h=case_ceiling + 1);
        }
        
        // Ventilation slots (minimal for compact design)
        for(x = [case_wall + 8:6:case_wall + 25]) {
            translate([x, case_wall + 2, external_height - case_ceiling]) {
                cube([3, 1, case_ceiling + 1]);
            }
            translate([x, external_width - case_wall - 3, external_height - case_ceiling]) {
                cube([3, 1, case_ceiling + 1]);
            }
        }
    }
    
    // Mounting posts for PCB (positioned for compact board)
    translate([case_wall + 2.5, case_wall + 2.5, case_floor]) {
        mounting_post();
    }
    translate([case_wall + internal_length - 2.5, case_wall + 2.5, case_floor]) {
        mounting_post();
    }
    translate([case_wall + 2.5, case_wall + internal_width - 2.5, case_floor]) {
        mounting_post();
    }
    translate([case_wall + internal_length - 2.5, case_wall + internal_width - 2.5, case_floor]) {
        mounting_post();
    }
    
    // Cable management clips
    translate([case_wall + 8, case_wall, case_floor + 6]) {
        cable_clip();
    }
    translate([case_wall + 20, case_wall, case_floor + 6]) {
        cable_clip();
    }
}

module mounting_post() {
    difference() {
        cylinder(d=post_diameter, h=post_height);
        cylinder(d=screw_hole_diameter, h=post_height + 1);
    }
}

module cable_clip() {
    difference() {
        cube([3, 2, 3]);
        translate([1.5, 1, 1.5]) {
            rotate([90, 0, 0]) {
                cylinder(d=2, h=3);
            }
        }
    }
}

module case_lid() {
    difference() {
        cube([external_length, external_width, case_ceiling + 1.5]);
        
        // Inner recess to fit on case
        translate([case_wall/2, case_wall/2, case_ceiling]) {
            cube([external_length - case_wall, external_width - case_wall, 2]);
        }
        
        // Ventilation slots
        for(x = [case_wall + 8:6:case_wall + 25]) {
            translate([x, case_wall + 2, -1]) {
                cube([3, 1, case_ceiling + 3]);
            }
            translate([x, external_width - case_wall - 3, -1]) {
                cube([3, 1, case_ceiling + 3]);
            }
        }
        
        // Status LED window
        translate([case_wall + 15, case_wall + 12, -1]) {
            cylinder(d=3, h=case_ceiling + 3);
        }
        
        // WiFi antenna area (no obstruction)
        translate([case_wall + 25, case_wall + 5, -1]) {
            cube([7, 15, case_ceiling + 3]);
        }
        
        // Label recess
        translate([case_wall + 5, case_wall + 8, -0.5]) {
            cube([15, 8, 1]);
        }
    }
}

module compact_case_with_features() {
    union() {
        main_case();
        
        // Desk stand feet
        translate([3, 3, 0]) {
            desk_foot();
        }
        translate([external_length - 3, 3, 0]) {
            desk_foot();
        }
        translate([3, external_width - 3, 0]) {
            desk_foot();
        }
        translate([external_length - 3, external_width - 3, 0]) {
            desk_foot();
        }
    }
}

module desk_foot() {
    difference() {
        cylinder(d=6, h=2);
        translate([0, 0, 0.5]) {
            cylinder(d=4, h=2);
        }
    }
}

// Wall mount bracket (separate part)
module wall_mount_bracket() {
    difference() {
        cube([external_length + 10, 8, 15]);
        
        // Mounting holes for case
        translate([5, 4, 5]) {
            rotate([90, 0, 0]) {
                cylinder(d=2.5, h=8);
            }
        }
        translate([external_length + 5, 4, 5]) {
            rotate([90, 0, 0]) {
                cylinder(d=2.5, h=8);
            }
        }
        
        // Wall mounting holes
        translate([external_length/2 - 10, 4, 10]) {
            rotate([90, 0, 0]) {
                cylinder(d=4, h=8);
            }
        }
        translate([external_length/2 + 10, 4, 10]) {
            rotate([90, 0, 0]) {
                cylinder(d=4, h=8);
            }
        }
        
        // Cable management
        translate([external_length/2, 8, 2]) {
            rotate([90, 0, 0]) {
                cylinder(d=8, h=8);
            }
        }
    }
}

// Sensor shield adapter (for add-on boards)
module sensor_shield_adapter() {
    difference() {
        cube([external_length, external_width, 3]);
        
        // Cutouts for original case mounting
        translate([case_wall + 2.5, case_wall + 2.5, -1]) {
            cylinder(d=2.5, h=5);
        }
        translate([case_wall + internal_length - 2.5, case_wall + 2.5, -1]) {
            cylinder(d=2.5, h=5);
        }
        translate([case_wall + 2.5, case_wall + internal_width - 2.5, -1]) {
            cylinder(d=2.5, h=5);
        }
        translate([case_wall + internal_length - 2.5, case_wall + internal_width - 2.5, -1]) {
            cylinder(d=2.5, h=5);
        }
        
        // Sensor board mounting area
        translate([case_wall + 8, case_wall + 5, 1]) {
            cube([20, 15, 3]);
        }
    }
    
    // Sensor mounting posts
    translate([case_wall + 10, case_wall + 7, 3]) {
        cylinder(d=3, h=2);
    }
    translate([case_wall + 26, case_wall + 7, 3]) {
        cylinder(d=3, h=2);
    }
    translate([case_wall + 10, case_wall + 18, 3]) {
        cylinder(d=3, h=2);
    }
    translate([case_wall + 26, case_wall + 18, 3]) {
        cylinder(d=3, h=2);
    }
}

// Assembly view
// Main case bottom
compact_case_with_features();

// Lid (translate for separate printing)
translate([external_length + 8, 0, 0]) {
    case_lid();
}

// Wall mount bracket (translate for separate printing)
translate([0, external_width + 8, 0]) {
    wall_mount_bracket();
}

// Sensor shield adapter (translate for separate printing)
translate([external_length + 8, external_width + 8, 0]) {
    sensor_shield_adapter();
}

// Print settings and information
echo("=== CARENUITY C3 MINI CASE ===");
echo("Designed for Carenuity C3 Mini v2.2.1 ESP32-C3 IoT Board");
echo("");
echo("=== PRINT SETTINGS ===");
echo("Layer height: 0.15-0.2mm (fine detail for compact case)");
echo("Infill: 15-20%");
echo("Support: No support needed");
echo("Print orientation: As shown");
echo("Material: PLA or PETG");
echo("");
echo("=== CASE DIMENSIONS ===");
echo(str("External: ", external_length, " x ", external_width, " x ", external_height, " mm"));
echo(str("Internal: ", internal_length, " x ", internal_width, " x ", internal_height, " mm"));
echo(str("Board space: ", board_length, " x ", board_width, " mm"));
echo("");
echo("=== FEATURES ===");
echo("✓ Compact design for IoT applications");
echo("✓ USB-C connector access");
echo("✓ Pin header access on both sides");
echo("✓ WiFi antenna area kept clear");
echo("✓ Status LED visibility");
echo("✓ Wall mount bracket included");
echo("✓ Sensor shield adapter for expansions");
echo("✓ Cable management clips");
echo("");
echo("=== ASSEMBLY NOTES ===");
echo("1. Use M2.5 x 8mm screws for PCB mounting");
echo("2. Keep antenna area free from metal objects");
echo("3. RGB LED should be visible through window");
echo("4. Optional: Add sensor shields via adapter");
echo("5. Wall mount with 6mm wall plugs");
echo("");
echo("=== IOT APPLICATIONS ===");
echo("• Environmental monitoring");
echo("• Smart home sensors");
echo("• WiFi/Bluetooth projects");
echo("• Battery-powered IoT devices");
echo("• Temperature/humidity logging");
echo("• Remote control applications");