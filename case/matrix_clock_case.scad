use <dep/mattwach/util.scad>
include <lib/matrix_clock_assembled_board.scad>
include <dep/mattwach/vitamins/electronics/adafruit_dotstar_8x8_matrix.scad>

module matrix_clock_case() {
  overlap = 0.01;

  button_slot_width = 3;
  button_slot_xoffset = 24.3;
  button_slot_zoffset = 14;
  button_slot_span = 14.5;
  button_peg_height = 3;
  button_peg_width = button_slot_width - 0.25;
  button_peg_length = 19;

  case_bottom_thickness = 2;
  board_zpad = case_bottom_thickness + 4;  // clearance for bolts and buttons
  board_xypad = 1;
  case_fillet_radius = 12;
  case_shell_thickness = 1.5;
  base_zsize = board_zpad + 19.5 + 5 * 2 - case_shell_thickness;
  case_ysize = MATRIX_CLOCK_PCB_WIDTH + case_shell_thickness * 2 + board_xypad * 2;
  case_xsize = MATRIX_CLOCK_PCB_LENGTH + board_xypad * 2 + case_fillet_radius * 2;
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

  module clock_base() {
    led_matrix_z = 5;
    led_matrix_ypad = 2;

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
        usb_access_hole_depth = 17;
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

      module led_matrix_mount() {
        mounting_post_height = led_matrix_ypad + DOTSTAR_8X8_THICKNESS - DOTSTAR_8X8_PCB_THICKNESS;
        mounting_post_length = DOTSTAR_8X8_BASE_WIDTH;
        mounting_post_width = DOTSTAR_8X8_TAB_WIDTH - 1;
        module matrix_mounting_post() {
          module matrix_mounting_hole() {
            matrix_hole_diameter = 2.35;
            tz(-overlap) cylinder(
                d=matrix_hole_diameter, h=mounting_post_height + overlap * 2);
          }
          module post() {
            cylinder(d=mounting_post_width, h=mounting_post_height);
          }

          translate([
              case_xbase + mounting_post_height + case_shell_thickness,
              MATRIX_CLOCK_PCB_WIDTH / 2 + DOTSTAR_8X8_WIDTH_WITH_TABS / 2 - DOTSTAR_8X8_TAB_WIDTH / 2,
              led_matrix_z + DOTSTAR_8X8_TAB_WIDTH / 2]) ry(-90) difference() {
            post();
            matrix_mounting_hole();
            tx(DOTSTAR_8X8_BOLT_SPAN) matrix_mounting_hole();
          }
        }

        matrix_mounting_post();
        ty(-DOTSTAR_8X8_WIDTH_WITH_TABS + DOTSTAR_8X8_TAB_WIDTH) matrix_mounting_post();
        tz(DOTSTAR_8X8_BOLT_SPAN) matrix_mounting_post();
        ty(-DOTSTAR_8X8_WIDTH_WITH_TABS + DOTSTAR_8X8_TAB_WIDTH) tz(DOTSTAR_8X8_BOLT_SPAN) matrix_mounting_post();
      }

      module button_slots() {
        module button_slot() {
          button_slot_height = 4 + button_peg_height;

          translate([
              case_xbase + button_slot_xoffset,
              case_ybase - overlap,
              button_slot_zoffset]) cube([
                button_slot_width,
                case_shell_thickness + overlap * 2,
                button_slot_height]);
        }

        button_slot();
        tx(button_slot_span) button_slot();
      }

      difference() {
        union() {
          difference() {
            base(base_zsize, 0);
            tz(case_bottom_thickness) base(base_zsize, case_shell_thickness);
          }
          pcb_mounting_posts();
          led_matrix_mount();
        }
        mounting_holes();
        button_access_holes();
        usb_access_hole();
        button_slots();
      }
    }

    module button_peg() {
      button_peg_zoffset = button_slot_zoffset + 3;
      button_peg_yoffset = 2;
      button_peg_backing_thickness = 2;
      button_peg_backing_xpad = 1.5;
      button_peg_backing_zpad = 5;
      module button_peg_shaft() {
        button_peg_front_xpad = 1;
        module main_peg() {
          cube([
              button_peg_width,
              button_peg_length,
              button_peg_height]);
        }
        module front_retainer() {
          translate([
              -button_peg_front_xpad,
              0,
              0]) cube([
                button_peg_width + button_peg_front_xpad * 2,
                button_peg_yoffset,
                button_peg_height]);
        }
        color("green") union() {
          main_peg();
          front_retainer();
        }
      }

      module button_peg_backing() {
        main_backing_xsize = button_peg_backing_xpad * 2 + button_peg_width;
        main_backing_zsize = button_peg_backing_zpad * 2 + button_peg_height;
        module main_backing() {
          cube([
              main_backing_xsize,
              button_peg_backing_thickness,
              main_backing_zsize]);
        }
        module peg_hole() {
          translate([
              (main_backing_xsize - button_peg_width) / 2,
              -overlap,
              (main_backing_zsize - button_peg_height) / 2]) cube([
                button_peg_width,
                button_peg_backing_thickness + overlap * 2,
                button_peg_height]);
        }
        color("blue") translate([
            -button_peg_backing_xpad,
            button_peg_yoffset + case_shell_thickness,
            -button_peg_backing_zpad]) difference() {
          main_backing();
          peg_hole();
        }
      }

      translate([
          case_xbase + button_slot_xoffset,
          case_ybase - button_peg_yoffset,
          button_peg_zoffset]) {
        button_peg_shaft();
        button_peg_backing();
      }
    }

    module led_matrix() {
      translate([
          case_xbase + DOTSTAR_8X8_THICKNESS + case_shell_thickness + led_matrix_ypad,
          case_ybase + (DOTSTAR_8X8_BASE_WIDTH + case_ysize) / 2, 
          led_matrix_z]) ry(-90) rz(-90) adafruit_dotstar_8x8_matrix();
    }

    tz(board_zpad) matrix_clock_assembled_board();
    led_matrix();    
    color("white", 0.5) base_shell();
    button_peg();
  }

  module top_cover() {
    top_cover_thickness = case_shell_thickness;
    inset_thickness = 1.5;
    insert_zsize = 10;

    module cover() {
      base(top_cover_thickness, 0);
    }

    module side_inserts() {
      side_insert_length = case_xsize - case_fillet_radius * 2;
      module side_insert() {
        translate([
            case_xbase + (case_xsize - side_insert_length) / 2,
            case_ybase,
            -insert_zsize + overlap]) cube([side_insert_length, inset_thickness, insert_zsize]);
      }
      ty(case_shell_thickness) side_insert();
      ty(case_ysize - inset_thickness - case_shell_thickness) side_insert();
    }

    module front_back_inserts() {
      fb_insert_length = case_ysize - case_fillet_radius * 2;
      module fb_insert(zsize) {
        translate([
            case_xbase,
            case_ybase + (case_ysize - fb_insert_length) / 2,
            -zsize + overlap]) cube([inset_thickness, fb_insert_length, zsize]);
      }
      tx(case_shell_thickness) fb_insert(3);
      tx(case_xsize - inset_thickness - case_shell_thickness) fb_insert(insert_zsize);
    }

    tz(base_zsize) union() {
      cover();
      side_inserts();
      front_back_inserts();
    }
  }
  
  *top_cover();
  clock_base();
}

$fa=2;
$fs=0.2;
matrix_clock_case();
