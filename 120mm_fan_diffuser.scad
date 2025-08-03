// 120mm PC Fan Diffuser
// Designed to mount on exhaust side of 120mm PC fans
// Reduces noise and distributes airflow evenly

// Parameters
fan_size = 120;           // Fan dimension (120mm x 120mm)
fan_thickness = 25;       // Standard fan thickness
mounting_hole_spacing = 105; // Standard 120mm fan mounting holes
mounting_hole_diameter = 4.5; // For M4 screws with clearance
diffuser_length = 50;     // Length of diffuser section
diffuser_outer_diameter = 160; // Final diffuser opening diameter
wall_thickness = 2.0;     // Wall thickness
corner_radius = 5;        // Rounded corners
grid_spacing = 8;         // Spacing for airflow straightening grid
grid_thickness = 1.5;     // Thickness of grid elements

module fan_diffuser() {
    difference() {
        union() {
            // Main diffuser body - expands from fan size to larger opening
            hull() {
                // Fan mounting end - rounded square
                translate([0, 0, 0])
                    linear_extrude(height=1, center=true)
                    offset(r=corner_radius)
                    square([fan_size-2*corner_radius, fan_size-2*corner_radius], center=true);
                
                // Large diffuser opening
                translate([0, 0, diffuser_length])
                    cylinder(d=diffuser_outer_diameter, h=1, center=true);
            }
            
            // Fan mounting flange
            translate([0, 0, -wall_thickness/2])
                linear_extrude(height=wall_thickness)
                offset(r=corner_radius)
                square([fan_size-2*corner_radius, fan_size-2*corner_radius], center=true);
            
            // Airflow straightening grid at output
            translate([0, 0, diffuser_length-grid_thickness/2]) {
                // Concentric circles for flow straightening
                for(r = [20:15:diffuser_outer_diameter/2-10]) {
                    difference() {
                        cylinder(d=r*2+grid_thickness, h=grid_thickness, center=true);
                        cylinder(d=r*2-grid_thickness, h=grid_thickness+1, center=true);
                    }
                }
                
                // Radial spokes
                for(angle = [0:45:315]) {
                    rotate([0, 0, angle])
                        translate([0, 0, 0])
                        cube([diffuser_outer_diameter-20, grid_thickness, grid_thickness], center=true);
                }
            }
        }
        
        // Hollow interior - airflow channel
        hull() {
            // Interior opening at fan end
            translate([0, 0, wall_thickness])
                linear_extrude(height=1, center=true)
                offset(r=corner_radius-wall_thickness)
                square([fan_size-2*corner_radius-2*wall_thickness, 
                       fan_size-2*corner_radius-2*wall_thickness], center=true);
            
            // Interior opening at diffuser end
            translate([0, 0, diffuser_length-grid_thickness])
                cylinder(d=diffuser_outer_diameter-2*wall_thickness, h=1, center=true);
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

// Generate the diffuser
fan_diffuser();