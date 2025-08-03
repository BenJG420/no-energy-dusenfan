// 120mm PC Fan Intake Piece
// Designed for standard 120mm PC fans (120x120x25mm mounting)

// Parameters
fan_size = 120;           // Fan dimension (120mm x 120mm)
fan_thickness = 25;       // Standard fan thickness
mounting_hole_spacing = 105; // Standard 120mm fan mounting holes
mounting_hole_diameter = 4.5; // For M4 screws with clearance
intake_length = 60;       // Length of intake funnel
intake_outer_diameter = 140; // Outer diameter of intake opening
wall_thickness = 2.5;     // Wall thickness for strength
corner_radius = 5;        // Rounded corners for better airflow

module fan_intake_piece() {
    difference() {
        union() {
            // Main funnel body - tapered from large opening to fan size
            hull() {
                // Large intake opening
                translate([0, 0, intake_length])
                    cylinder(d=intake_outer_diameter, h=1, center=true);
                
                // Fan mounting end - rounded square
                translate([0, 0, 0])
                    linear_extrude(height=1, center=true)
                    offset(r=corner_radius)
                    square([fan_size-2*corner_radius, fan_size-2*corner_radius], center=true);
            }
            
            // Fan mounting flange
            translate([0, 0, -wall_thickness/2])
                linear_extrude(height=wall_thickness)
                offset(r=corner_radius)
                square([fan_size-2*corner_radius, fan_size-2*corner_radius], center=true);
        }
        
        // Hollow interior - airflow channel
        hull() {
            // Interior opening at intake
            translate([0, 0, intake_length])
                cylinder(d=intake_outer_diameter-2*wall_thickness, h=1, center=true);
            
            // Interior opening at fan end
            translate([0, 0, wall_thickness])
                linear_extrude(height=1, center=true)
                offset(r=corner_radius-wall_thickness)
                square([fan_size-2*corner_radius-2*wall_thickness, 
                       fan_size-2*corner_radius-2*wall_thickness], center=true);
        }
        
        // Fan mounting holes
        for(x = [-1, 1]) {
            for(y = [-1, 1]) {
                translate([x * mounting_hole_spacing/2, y * mounting_hole_spacing/2, -wall_thickness-1])
                    cylinder(d=mounting_hole_diameter, h=wall_thickness+2);
            }
        }
        
        // Fan blade clearance area
        translate([0, 0, wall_thickness])
            cylinder(d=fan_size-10, h=fan_thickness+1);
    }
}

// Generate the intake piece
fan_intake_piece();