// Motor Controller Case - 3D Printable
// Designed for dual H-bridge motor driver with 330µF capacitors
// Compatible with boards similar to L298N replacements

// Main dimensions (adjust based on your specific board)
board_length = 65;     // Board length in mm
board_width = 56;      // Board width in mm
board_thickness = 1.6; // PCB thickness in mm

// Case dimensions
case_wall = 2.5;       // Wall thickness
case_floor = 2;        // Bottom thickness
case_ceiling = 2;      // Top thickness
case_clearance = 3;    // Space above components

// Component heights (estimated)
capacitor_height = 12; // 330µF capacitor height
terminal_height = 8;   // Green terminal block height
chip_height = 4;       // IC height

// Calculate internal dimensions
internal_length = board_length + 4; // Small clearance around board
internal_width = board_width + 4;
internal_height = max(capacitor_height, terminal_height) + case_clearance;

// External case dimensions
external_length = internal_length + (case_wall * 2);
external_width = internal_width + (case_wall * 2);
external_height = internal_height + case_floor + case_ceiling;

// Mounting post dimensions
post_diameter = 6;
post_height = case_floor + 2;
screw_hole_diameter = 3.2; // M3 screw

// Terminal block cutout dimensions
terminal_width = 20;
terminal_depth = 12;

module main_case() {
    difference() {
        // Outer case
        cube([external_length, external_width, external_height]);
        
        // Internal cavity
        translate([case_wall, case_wall, case_floor]) {
            cube([internal_length, internal_width, internal_height + 1]);
        }
        
        // Terminal block cutouts (left side)
        translate([-1, case_wall + 8, case_floor + 2]) {
            cube([case_wall + 2, terminal_width, terminal_height + 2]);
        }
        
        // Terminal block cutouts (right side)
        translate([external_length - case_wall - 1, case_wall + 8, case_floor + 2]) {
            cube([case_wall + 2, terminal_width, terminal_height + 2]);
        }
        
        // Power input connector cutout (back)
        translate([case_wall + 15, -1, case_floor + 2]) {
            cube([15, case_wall + 2, 8]);
        }
        
        // Control signal connector cutout (front)
        translate([case_wall + 25, external_width - case_wall - 1, case_floor + 2]) {
            cube([12, case_wall + 2, 6]);
        }
        
        // Ventilation holes on top
        for(x = [15:10:external_length-15]) {
            for(y = [15:10:external_width-15]) {
                translate([x, y, external_height - case_ceiling]) {
                    cylinder(d=3, h=case_ceiling + 1);
                }
            }
        }
    }
    
    // Mounting posts for PCB
    translate([case_wall + 5, case_wall + 5, case_floor]) {
        mounting_post();
    }
    translate([case_wall + internal_length - 5, case_wall + 5, case_floor]) {
        mounting_post();
    }
    translate([case_wall + 5, case_wall + internal_width - 5, case_floor]) {
        mounting_post();
    }
    translate([case_wall + internal_length - 5, case_wall + internal_width - 5, case_floor]) {
        mounting_post();
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
        cube([external_length, external_width, case_ceiling + 2]);
        
        // Inner recess to fit on case
        translate([case_wall/2, case_wall/2, case_ceiling]) {
            cube([external_length - case_wall, external_width - case_wall, 3]);
        }
        
        // Ventilation holes
        for(x = [15:10:external_length-15]) {
            for(y = [15:10:external_width-15]) {
                translate([x, y, -1]) {
                    cylinder(d=3, h=case_ceiling + 4);
                }
            }
        }
        
        // Label recess
        translate([external_length/2 - 20, external_width/2 - 5, -0.5]) {
            cube([40, 10, 1]);
        }
    }
}

module case_bottom_with_feet() {
    union() {
        main_case();
        
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

module rubber_foot() {
    difference() {
        cylinder(d=12, h=3);
        translate([0, 0, 1]) {
            cylinder(d=8, h=3);
        }
    }
}

// Assembly view (comment out parts you don't want to print)
// Main case bottom
case_bottom_with_feet();

// Lid (translate for separate printing)
translate([external_length + 10, 0, 0]) {
    case_lid();
}

// Print settings information
echo("=== PRINT SETTINGS ===");
echo("Layer height: 0.2mm");
echo("Infill: 20%");
echo("Support: No support needed");
echo("Print orientation: As shown");
echo("");
echo("=== CASE DIMENSIONS ===");
echo(str("External: ", external_length, " x ", external_width, " x ", external_height, " mm"));
echo(str("Internal: ", internal_length, " x ", internal_width, " x ", internal_height, " mm"));
echo("");
echo("=== ASSEMBLY NOTES ===");
echo("1. Use M3 x 10mm screws for PCB mounting");
echo("2. Use M3 x 16mm screws for lid attachment (optional)");
echo("3. Consider adding rubber feet for desk protection");