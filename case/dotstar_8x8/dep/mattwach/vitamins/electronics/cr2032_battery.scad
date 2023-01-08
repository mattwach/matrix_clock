use <../../util.scad>

CR2032_DIAMETER = 20;
CR2032_THICKNESS = 3.2;

module cr2032() {
  negative_diameter = 17.7;
  negative_height = 0.3;
  overlap = 0.01;
  marker_thickness = 0.5;
  marker_length = 5;
  marker_depth = 0.1;
  module cell() {
    color("silver") union() {
      cylinder(d=negative_diameter, h=negative_height + overlap);
      tz(negative_height) cylinder(d=CR2032_DIAMETER, h=CR2032_THICKNESS-negative_height);
    }
  }

  module positive_marker() {
    color("#aaa") union() {
      translate([
          -marker_length / 2,
          -marker_thickness / 2,
          CR2032_THICKNESS - marker_depth + overlap
      ]) cube([marker_length, marker_thickness, marker_depth]);
      translate([
          -marker_thickness / 2,
          -marker_length / 2,
          CR2032_THICKNESS - marker_depth + overlap
      ]) cube([marker_thickness, marker_length, marker_depth]);
    }
  }

  module negative_marker() {
    color("#aaa") translate([
        -marker_length / 2,
        -marker_thickness / 2,
        -overlap
    ]) cube([marker_length, marker_thickness, marker_depth]);
  }

  difference() {
    cell();
    negative_marker();
    positive_marker();
  }
}

/*
$fa=2;
$fs=0.5;
cr2032();
*/
