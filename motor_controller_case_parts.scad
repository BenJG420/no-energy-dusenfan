// Motor Controller Case - Individual Parts for 3D Printing
// Choose which part to generate by uncommenting the desired section

// Common dimensions and modules
board_length = 65;
board_width = 56;
board_thickness = 1.6;

case_wall = 2.5;
case_floor = 2;
case_ceiling = 2;
case_clearance = 3;

capacitor_height = 12;
terminal_height = 8;
chip_height = 4;

internal_length = board_length + 4;
internal_width = board_width + 4;
internal_height = max(capacitor_height, terminal_height) + case_clearance;

external_length = internal_length + (case_wall * 2);
external_width = internal_width + (case_wall * 2);
external_height = internal_height + case_floor + case_ceiling;

post_diameter = 6;
post_height = case_floor + 2;
screw_hole_diameter = 3.2;

terminal_width = 20;
terminal_depth = 12;

module main_case() {
    difference() {
        cube([external_length, external_width, external_height]);
        
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
        
        // Power input connector cutout
        translate([case_wall + 15, -1, case_floor + 2]) {
            cube([15, case_wall + 2, 8]);
        }
        
        // Control signal connector cutout
        translate([case_wall + 25, external_width - case_wall - 1, case_floor + 2]) {
            cube([12, case_wall + 2, 6]);
        }
    }
    
    // Mounting posts
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

module rubber_foot() {
    difference() {
        cylinder(d=12, h=3);
        translate([0, 0, 1]) {
            cylinder(d=8, h=3);
        }
    }
}

// UNCOMMENT THE PART YOU WANT TO PRINT:

// Part 1: Case Bottom
main_case();

// Part 2: Case Lid (uncomment to print lid)
// case_lid();

// Part 3: Rubber Feet (uncomment to print 4 feet)
// for(i = [0:3]) {
//     translate([i * 15, 0, 0]) {
//         rubber_foot();
//     }
// }