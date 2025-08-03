#!/usr/bin/env python3
"""
Generate STL files for 120mm PC fan intake and diffuser
Alternative to OpenSCAD using Python and cadquery
"""

try:
    import cadquery as cq
    print("Using cadquery for STL generation")
    CADQUERY_AVAILABLE = True
except ImportError:
    print("cadquery not available, will create OpenSCAD files only")
    CADQUERY_AVAILABLE = False

import math

def create_fan_intake():
    """Create 120mm PC fan intake piece"""
    if not CADQUERY_AVAILABLE:
        return None
    
    # Parameters
    fan_size = 120
    fan_thickness = 25
    mounting_hole_spacing = 105
    mounting_hole_diameter = 4.5
    intake_length = 60
    intake_outer_diameter = 140
    wall_thickness = 2.5
    corner_radius = 5
    
    # Create the main funnel shape
    # Start with fan mounting square
    fan_profile = cq.Workplane("XY").rect(fan_size, fan_size).extrude(1)
    
    # Create intake opening
    intake_profile = (cq.Workplane("XY")
                     .transformed(offset=(0, 0, intake_length))
                     .circle(intake_outer_diameter/2)
                     .extrude(1))
    
    # Create the hull between them
    # This is a simplified approach - in practice you'd use loft
    result = (cq.Workplane("XY")
              .rect(fan_size - 2*corner_radius, fan_size - 2*corner_radius)
              .workplane(offset=intake_length)
              .circle(intake_outer_diameter/2)
              .loft(combine=True)
              .faces("<Z")
              .workplane()
              .rect(fan_size, fan_size)
              .extrude(-wall_thickness))
    
    # Add mounting holes
    for x in [-1, 1]:
        for y in [-1, 1]:
            result = (result
                     .faces("<Z")
                     .workplane()
                     .pushPoints([(x * mounting_hole_spacing/2, y * mounting_hole_spacing/2)])
                     .hole(mounting_hole_diameter))
    
    # Hollow out the interior
    inner_result = (cq.Workplane("XY")
                   .rect(fan_size - 2*corner_radius - 2*wall_thickness, 
                         fan_size - 2*corner_radius - 2*wall_thickness)
                   .workplane(offset=intake_length)
                   .circle(intake_outer_diameter/2 - wall_thickness)
                   .loft(combine=True))
    
    result = result.cut(inner_result)
    
    return result

def create_fan_diffuser():
    """Create 120mm PC fan diffuser"""
    if not CADQUERY_AVAILABLE:
        return None
        
    # Parameters  
    fan_size = 120
    fan_thickness = 25
    mounting_hole_spacing = 105
    mounting_hole_diameter = 4.5
    diffuser_length = 50
    diffuser_outer_diameter = 160
    wall_thickness = 2.0
    corner_radius = 5
    
    # Create the main diffuser shape
    result = (cq.Workplane("XY")
              .rect(fan_size - 2*corner_radius, fan_size - 2*corner_radius)
              .workplane(offset=diffuser_length)
              .circle(diffuser_outer_diameter/2)
              .loft(combine=True)
              .faces("<Z")
              .workplane()
              .rect(fan_size, fan_size)
              .extrude(-wall_thickness))
    
    # Add mounting holes
    for x in [-1, 1]:
        for y in [-1, 1]:
            result = (result
                     .faces("<Z")
                     .workplane()
                     .pushPoints([(x * mounting_hole_spacing/2, y * mounting_hole_spacing/2)])
                     .hole(mounting_hole_diameter))
    
    # Hollow out the interior
    inner_result = (cq.Workplane("XY")
                   .rect(fan_size - 2*corner_radius - 2*wall_thickness, 
                         fan_size - 2*corner_radius - 2*wall_thickness)
                   .workplane(offset=diffuser_length)
                   .circle(diffuser_outer_diameter/2 - wall_thickness)
                   .loft(combine=True))
    
    result = result.cut(inner_result)
    
    return result

def main():
    if CADQUERY_AVAILABLE:
        print("Generating STL files with cadquery...")
        
        # Generate intake piece
        intake = create_fan_intake()
        if intake:
            cq.exporters.export(intake, "120mm_fan_intake.stl")
            print("Generated: 120mm_fan_intake.stl")
        
        # Generate diffuser
        diffuser = create_fan_diffuser()
        if diffuser:
            cq.exporters.export(diffuser, "120mm_fan_diffuser.stl")
            print("Generated: 120mm_fan_diffuser.stl")
    else:
        print("Please install cadquery: pip install cadquery")
        print("Or use OpenSCAD with the provided .scad files")

if __name__ == "__main__":
    main()