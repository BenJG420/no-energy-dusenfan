// BLDC Motor Controller Case - 3D Printable
// Designed for 6-60V 400W BLDC Three Phase Brushless Motor Controller
// Board dimensions: 63 x 42.5 x 17mm (approximate)

// Main board dimensions (based on specifications)
board_length = 63;     // Board length in mm
board_width = 43;      // Board width in mm (42.5 rounded up)
board_thickness = 1.6; // PCB thickness in mm

// Case dimensions
case_wall = 2.8;       // Wall thickness (thicker for heat dissipation)
case_floor = 3;        // Bottom thickness (thicker for stability)
case_ceiling = 3;      // Top thickness
case_clearance = 4;    // Space above components

// Component heights (estimated for BLDC controller)
mosfet_height = 6;     // Power MOSFETs height
capacitor_height = 8;  // Electrolytic capacitors
connector_height = 10; // Terminal blocks and connectors
heatsink_clearance = 5;// Extra space for heat dissipation

// Calculate internal dimensions
internal_length = board_length + 6; // More clearance for heat management
internal_width = board_width + 6;
internal_height = max(mosfet_height, capacitor_height, connector_height) + case_clearance;

// External case dimensions
external_length = internal_length + (case_wall * 2);
external_width = internal_width + (case_wall * 2);
external_height = internal_height + case_floor + case_ceiling;

// Mounting post dimensions
post_diameter = 7;     // Slightly larger for BLDC controller
post_height = case_floor + 3;
screw_hole_diameter = 3.2; // M3 screw

// Connector cutout dimensions
power_terminal_width = 15;
power_terminal_height = 8;
control_terminal_width = 25;
control_terminal_height = 6;
motor_terminal_width = 18;
motor_terminal_height = 8;

module main_case() {
    difference() {
        // Outer case
        cube([external_length, external_width, external_height]);
        
        // Internal cavity
        translate([case_wall, case_wall, case_floor]) {
            cube([internal_length, internal_width, internal_height + 1]);
        }
        
        // Power input terminals (back side)
        translate([-1, case_wall + 8, case_floor + 3]) {
            cube([case_wall + 2, power_terminal_width, power_terminal_height]);
        }
        
        // Motor output terminals (front side)  
        translate([external_length - case_wall - 1, case_wall + 12, case_floor + 3]) {
            cube([case_wall + 2, motor_terminal_width, motor_terminal_height]);
        }
        
        // Control connector (left side)
        translate([case_wall + 10, -1, case_floor + 2]) {
            cube([control_terminal_width, case_wall + 2, control_terminal_height]);
        }
        
        // Hall sensor connector (right side)
        translate([case_wall + 30, external_width - case_wall - 1, case_floor + 2]) {
            cube([15, case_wall + 2, 6]);
        }
        
        // Heat dissipation vents on sides
        for(z = [case_floor + 5:4:case_floor + 12]) {
            for(y = [10:8:external_width-10]) {
                // Left side vents
                translate([-1, y, z]) {
                    cube([case_wall + 2, 4, 2]);
                }
                // Right side vents
                translate([external_length - case_wall - 1, y, z]) {
                    cube([case_wall + 2, 4, 2]);
                }
            }
        }
        
        // Top ventilation holes (larger for better cooling)
        for(x = [12:8:external_length-12]) {
            for(y = [12:8:external_width-12]) {
                translate([x, y, external_height - case_ceiling]) {
                    cylinder(d=4, h=case_ceiling + 1);
                }
            }
        }
        
        // Status LED window (if present)
        translate([external_length - 15, external_width - 15, external_height - case_ceiling]) {
            cylinder(d=5, h=case_ceiling + 1);
        }
    }
    
    // Mounting posts for PCB (positioned for typical BLDC controller layout)
    translate([case_wall + 4, case_wall + 4, case_floor]) {
        mounting_post();
    }
    translate([case_wall + internal_length - 4, case_wall + 4, case_floor]) {
        mounting_post();
    }
    translate([case_wall + 4, case_wall + internal_width - 4, case_floor]) {
        mounting_post();
    }
    translate([case_wall + internal_length - 4, case_wall + internal_width - 4, case_floor]) {
        mounting_post();
    }
    
    // Heat dissipation ribs on bottom
    for(x = [case_wall + 10:6:external_length - case_wall - 10]) {
        translate([x, case_wall, 0]) {
            cube([2, external_width - (case_wall * 2), 2]);
        }
    }
}

module mounting_post() {
    difference() {
        cylinder(d=post_diameter, h=post_height);
        cylinder(d=screw_hole_diameter, h=post_height + 1);
    }
}

module case_lid() {
    difference() {
        cube([external_length, external_width, case_ceiling + 3]);
        
        // Inner recess to fit on case
        translate([case_wall/2, case_wall/2, case_ceiling]) {
            cube([external_length - case_wall, external_width - case_wall, 4]);
        }
        
        // Large ventilation holes for heat dissipation
        for(x = [12:8:external_length-12]) {
            for(y = [12:8:external_width-12]) {
                translate([x, y, -1]) {
                    cylinder(d=4, h=case_ceiling + 5);
                }
            }
        }
        
        // Label recess
        translate([external_length/2 - 25, external_width/2 - 8, -1]) {
            cube([50, 16, 1.5]);
        }
        
        // Heat sink mounting holes (if external heat sink needed)
        translate([external_length/2 - 15, external_width/2 - 10, -1]) {
            cylinder(d=4, h=case_ceiling + 5);
        }
        translate([external_length/2 + 15, external_width/2 - 10, -1]) {
            cylinder(d=4, h=case_ceiling + 5);
        }
        translate([external_length/2 - 15, external_width/2 + 10, -1]) {
            cylinder(d=4, h=case_ceiling + 5);
        }
        translate([external_length/2 + 15, external_width/2 + 10, -1]) {
            cylinder(d=4, h=case_ceiling + 5);
        }
        
        // Status LED window
        translate([external_length - 15, external_width - 15, -1]) {
            cylinder(d=5, h=case_ceiling + 5);
        }
    }
}

module case_bottom_with_features() {
    union() {
        main_case();
        
        // DIN rail mounting clips (optional)
        translate([external_length/2 - 20, 0, 0]) {
            din_rail_clip();
        }
        translate([external_length/2 + 20, 0, 0]) {
            din_rail_clip();
        }
        
        // Rubber feet positions
        translate([8, 8, 0]) {
            rubber_foot();
        }
        translate([external_length - 8, 8, 0]) {
            rubber_foot();
        }
        translate([8, external_width - 8, 0]) {
            rubber_foot();
        }
        translate([external_length - 8, external_width - 8, 0]) {
            rubber_foot();
        }
    }
}

module din_rail_clip() {
    // Simple DIN rail mounting clip
    difference() {
        cube([35, 8, 4]);
        translate([5, 2, -1]) {
            cube([25, 4, 6]);
        }
        // Mounting holes
        translate([7.5, 4, -1]) {
            cylinder(d=3.2, h=6);
        }
        translate([27.5, 4, -1]) {
            cylinder(d=3.2, h=6);
        }
    }
}

module rubber_foot() {
    difference() {
        cylinder(d=14, h=4);
        translate([0, 0, 1]) {
            cylinder(d=10, h=4);
        }
    }
}

// Heat sink adapter (separate part)
module heat_sink_adapter() {
    difference() {
        cube([40, 30, 5]);
        
        // Mounting holes to match lid
        translate([7.5, 7.5, -1]) {
            cylinder(d=3.2, h=7);
        }
        translate([32.5, 7.5, -1]) {
            cylinder(d=3.2, h=7);
        }
        translate([7.5, 22.5, -1]) {
            cylinder(d=3.2, h=7);
        }
        translate([32.5, 22.5, -1]) {
            cylinder(d=3.2, h=7);
        }
        
        // Heat sink mounting pattern
        for(x = [10:5:30]) {
            for(y = [10:5:20]) {
                translate([x, y, -1]) {
                    cylinder(d=2.5, h=7);
                }
            }
        }
    }
}

// Assembly view
// Main case bottom
case_bottom_with_features();

// Lid (translate for separate printing)
translate([external_length + 15, 0, 0]) {
    case_lid();
}

// Heat sink adapter (translate for separate printing)
translate([0, external_width + 15, 0]) {
    heat_sink_adapter();
}

// Print settings and information
echo("=== BLDC CONTROLLER CASE ===");
echo("Designed for 6-60V 400W BLDC Controller");
echo("");
echo("=== PRINT SETTINGS ===");
echo("Layer height: 0.2mm");
echo("Infill: 25% (higher for heat dissipation)");
echo("Support: No support needed");
echo("Print orientation: As shown");
echo("Material: PETG or ABS recommended for heat resistance");
echo("");
echo("=== CASE DIMENSIONS ===");
echo(str("External: ", external_length, " x ", external_width, " x ", external_height, " mm"));
echo(str("Internal: ", internal_length, " x ", internal_width, " x ", internal_height, " mm"));
echo("");
echo("=== IMPORTANT SAFETY NOTES ===");
echo("1. This controller can handle up to 400W - ensure adequate cooling!");
echo("2. Add external heat sink for continuous high-power operation");
echo("3. Use PETG or ABS filament for better heat resistance");
echo("4. Ensure all ventilation holes remain unobstructed");
echo("5. Mount securely - high current applications!");
echo("");
echo("=== ASSEMBLY NOTES ===");
echo("1. Use M3 x 12mm screws for PCB mounting");
echo("2. Consider thermal pads between MOSFETs and case");
echo("3. Use M3 x 20mm screws for lid attachment");
echo("4. Optional: Add external heat sink via adapter plate");