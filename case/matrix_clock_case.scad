use <mattwach/util.scad>
include <lib/matrix_clock_assembled_board.scad>
include <mattwach/vitamins/electronics/adafruit_dotstar_8x8_matrix.scad>

module matrix_clock_case() {
  overlap = 0.01;

  module clock_base() {
    case_bottom_thickness = 2;
    board_zpad = case_bottom_thickness + 4;  // clearance for bolts and buttons
    case_shell_thickness = 1.5;
    board_xypad = 1;
    case_fillet_radius = 10;
    case_ysize = MATRIX_CLOCK_PCB_WIDTH + case_shell_thickness * 2 + board_xypad * 2;
    case_xsize = MATRIX_CLOCK_PCB_LENGTH + board_xypad * 2 + case_fillet_radius * 2;
    base_zsize = board_zpad + 40;
    case_xbase = -board_xypad - case_fillet_radius;
    case_ybase = -case_shell_thickness - board_xypad;

    module base(height, inset) {
      x1 = -board_xypad + inset;
      x2 = MATRIX_CLOCK_PCB_LENGTH + board_xypad - inset;
      y1 = case_fillet_radius - case_shell_thickness - board_xypad + inset;
      y2 = MATRIX_CLOCK_PCB_WIDTH - case_fillet_radius + case_shell_thickness + board_xypad - inset;
      //txy(-case_fillet_radius - board_xypad,
      //  -case_shell_thickness - board_xypad)
      //  cube([case_xsize, case_ysize, height/2]);
      hull() {
        txy(x1, y1) cylinder(r=case_fillet_radius, h=height);
        txy(x1, y2) cylinder(r=case_fillet_radius, h=height);
        txy(x2, y1) cylinder(r=case_fillet_radius, h=height);
        txy(x2, y2) cylinder(r=case_fillet_radius, h=height);
      }
    }

    module base_shell() {
      mounting_post_hole_x1 = 59.9;
      mounting_post_hole_y1 = 3.7;
      mounting_post_hole_x2 = 6.9;
      mounting_post_hole_y2 = 50.7;
      module mounting_post(is_hole) {
        mounting_post_hole_size = 1.85;
        mounting_post_diameter = mounting_post_hole_size + 4;
        if (is_hole) {
          tz(-overlap) cylinder(
              d=mounting_post_hole_size,
              h=board_zpad + overlap * 2);
        } else {
          tz(case_bottom_thickness - overlap) cylinder(
              d=mounting_post_diameter, h=board_zpad - case_bottom_thickness);
        }
      }

      module pcb_mounting_posts() {
        txy(mounting_post_hole_x1, mounting_post_hole_y1) mounting_post(false);
        txy(mounting_post_hole_x2, mounting_post_hole_y2) mounting_post(false);
        txy(3,3) mounting_post(false);
        txy(60.5,51.5) mounting_post(false);
      }

      module mounting_holes() {
        txy(mounting_post_hole_x1, mounting_post_hole_y1) mounting_post(true);
        txy(mounting_post_hole_x2, mounting_post_hole_y2) mounting_post(true);
      }

      module button_access_holes() {
        access_hole_diameter = 15;
        module access_hole() {
          tz(-overlap) cylinder(
              d=access_hole_diameter,
              h=case_bottom_thickness + overlap * 2);
        }
        module bootsel_hole() {
          txy(47, 33) access_hole();
        }
        module reset_hole() {
          txy(29, 48) access_hole();
        }
        bootsel_hole();
        reset_hole();
      }

      module usb_access_hole() {
        usb_access_hole_width = 14;
        usb_access_hole_depth = 15;
        usb_access_hole_height = 8;
        usb_access_hole_yoffest = 36.5;
        union() {
          translate([
              case_xbase + case_xsize - usb_access_hole_depth + usb_access_hole_width / 2,
              usb_access_hole_yoffest,
              -overlap]) cylinder(
                d=usb_access_hole_width,
                h=case_bottom_thickness + overlap * 2);
          translate([
              case_xbase + case_xsize - usb_access_hole_depth + usb_access_hole_width / 2,
              usb_access_hole_yoffest - usb_access_hole_width / 2,
              -overlap]) cube([
                usb_access_hole_depth - usb_access_hole_width / 2 + overlap,
                usb_access_hole_width,
                usb_access_hole_height]);
        }
      }

      difference() {
        union() {
          difference() {
            base(base_zsize, 0);
            tz(case_bottom_thickness) base(base_zsize, case_shell_thickness);
          }
          pcb_mounting_posts();
        }
        mounting_holes();
        button_access_holes();
        usb_access_hole();
      }
    }

    module led_matrix() {
      led_matrix_z = 10;
      led_matrix_ypad = 1;
      translate([
          case_xbase + DOTSTAR_8X8_THICKNESS + case_shell_thickness + led_matrix_ypad,
          case_ybase + (DOTSTAR_8X8_BASE_WIDTH + case_ysize) / 2, 
          led_matrix_z]) ry(-90) rz(-90) adafruit_dotstar_8x8_matrix();
    }

    tz(board_zpad) matrix_clock_assembled_board();
    led_matrix();    
    color("white", 0.5) base_shell();
  }
  
  clock_base();
}

$fa=2;
$fs=0.2;
matrix_clock_case();
