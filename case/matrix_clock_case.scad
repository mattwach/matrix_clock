use <mattwach/util.scad>
include <lib/matrix_clock_assembled_board.scad>

module matrix_clock_case() {

  module clock_base() {
    case_bottom_thickness = 2;
    board_zpad = case_bottom_thickness + 4;  // clearance for bolts and buttons
    case_shell_thickness = 1.5;
    board_xypad = 1;
    case_fillet_radius = 5;
    case_ysize = MATRIX_CLOCK_PCB_WIDTH + case_shell_thickness * 2 + board_xypad * 2;
    case_xsize = MATRIX_CLOCK_PCB_LENGTH + board_xypad * 2 + case_fillet_radius * 2;
    base_zsize = board_zpad + 20;

    module base(height, inset) {
      x1 = board_xypad + inset;
      x2 = MATRIX_CLOCK_PCB_LENGTH + board_xypad - inset;
      y1 = case_fillet_radius - case_shell_thickness - board_xypad + inset;
      y2 = MATRIX_CLOCK_PCB_WIDTH - case_fillet_radius + case_shell_thickness + board_xypad - inset;
      //txy(-case_fillet_radius - board_xypad,
      //    -case_shell_thickness - board_xypad)
      //  cube([case_xsize, case_ysize, height/2]);
      hull() {
        txy(x1, y1) cylinder(r=case_fillet_radius, h=height);
        txy(x1, y2) cylinder(r=case_fillet_radius, h=height);
        txy(x2, y1) cylinder(r=case_fillet_radius, h=height);
        txy(x2, y2) cylinder(r=case_fillet_radius, h=height);
      }
    }

    module base_shell() {
      difference() {
        base(base_zsize, 0);
        tz(case_bottom_thickness) base(base_zsize, case_shell_thickness);
      }
    }

    tz(board_zpad) matrix_clock_assembled_board();
    color("white", 0.5) base_shell();
  }
  
  clock_base();
}

$fa=2;
$fs=0.2;
matrix_clock_case();
