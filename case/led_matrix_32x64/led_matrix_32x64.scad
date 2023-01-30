use <mattwach/util.scad>
include <mattwach/vitamins/electronics/led_panel_64x32.scad>
include <mattwach/honeycomb.scad>
include <dep/button_pcb.scad>
include <dep/main_pcb.scad>

overlap = 0.01;

pcb_thickness = 1.6;
pcb_clearance = pcb_thickness + 0.2;
placed_button_pcb_xoffset = 12.5;
placed_button_pcb_yoffset = 3.4;

back_grid_zsize = 2;
support_zsize = 14;
mesh_zsize = 6;
support_side_thickness = 12;
support_length =
  LED_PANEL_62_32_BOLT_HOLE_LOFFSET +
  LED_PANEL_62_32_BOLT_HOLE_LSPAN +
  support_side_thickness / 2; 

module led_panel() {
  rz(90) rx(180) led_panel_64x32();
}

module placed_main_pcb() {
  txy(44.8, 68) rz(-90) main_pcb();
}

module placed_button_pcb() {
  translate([
      placed_button_pcb_xoffset,
      placed_button_pcb_yoffset + pcb_thickness,
      9]) rz(-90) ry(90) button_pcb();
}

module bolt_hole() {
  bolt_hole_inset = 4;
  translate([
      LED_PANEL_62_32_BOLT_HOLE_WOFFSET,
      LED_PANEL_62_32_BOLT_HOLE_LOFFSET,
      -overlap]) union() {
    cylinder(d=3.2, h=support_zsize - bolt_hole_inset + overlap);
    tz(support_zsize - bolt_hole_inset)
      cylinder(d=5.9, h=bolt_hole_inset + overlap * 2);
  }
}

module bottom_support() {
  support_arm_yoffset = 66.7;
  support_arm_ysize = 4;
  module left_side() {
    module left_side_main() {
      module main() {
        cube([
            support_side_thickness,
            support_length,
            support_zsize]);
      }

      module pcb_anti_slide_arm() {
        arm_inset = 2;
        xsize = 15;
        xoffset = 44.6;
        translate([
            xoffset - xsize,
            support_arm_yoffset - arm_inset,
            0]) cube([
              xsize,
              support_arm_ysize + arm_inset,
              support_zsize]);
      }

      module pcb_support_arm() {
        support_arm_xoffset = 80;
        support_arm_xsize = 15;
        module pcb_cutout() {
          cutout_yinset = 1.5;
          translate([
              -overlap,
              -overlap,
              -overlap]) cube([
                support_arm_xsize + overlap * 2,
                cutout_yinset + overlap,
                pcb_clearance + overlap]);
        }
        txy(
            support_arm_xoffset,
            support_arm_yoffset) difference() {
          cube([
              support_arm_xsize,
              support_arm_ysize,
              support_zsize]);
          pcb_cutout();
        }
      }

      union() {
        main();
        pcb_anti_slide_arm();
        pcb_support_arm();
      }
    }

    module mesh_cutout() {
      translate([
          -overlap,
          support_length - support_side_thickness + overlap,
          -overlap]) cube([
            support_side_thickness + overlap * 2,
            support_side_thickness + overlap,
            mesh_zsize]);
    }

    module bolt_holes() {
      bolt_hole();
      ty(LED_PANEL_62_32_BOLT_HOLE_LSPAN) bolt_hole();
    }

    difference() {
      left_side_main();
      mesh_cutout();
      bolt_holes();
    }
  }

  module right_side() {
    right_size_xoffset = LED_PANEL_64_32_BACK_WIDTH - support_side_thickness;
    module right_side_main() {
      translate([
          right_size_xoffset,
          0,
          0]) cube([
            support_side_thickness,
            support_length,
            support_zsize]);
    }

    module main_pcb_side_cutout() {
      main_pcb_side_cutout_yoffset = 5.8;
      main_pcb_side_cutout_ysize = 62.5;
      main_pcb_side_cutout_xsize = 1.3;
      translate([
          right_size_xoffset - overlap,
          main_pcb_side_cutout_yoffset,
          -overlap]) cube([
            main_pcb_side_cutout_xsize + overlap,
            main_pcb_side_cutout_ysize,
            pcb_clearance + overlap]);
    }

    module mesh_cutout() {
      translate([
          right_size_xoffset - overlap,
          support_length - support_side_thickness + overlap,
          -overlap]) cube([
            support_side_thickness + overlap * 2,
            support_side_thickness + overlap,
            mesh_zsize]);
    }

    module bolt_holes() {
      tx(LED_PANEL_62_32_BOLT_HOLE_WSPAN) {
        bolt_hole();
        ty(LED_PANEL_62_32_BOLT_HOLE_LSPAN) bolt_hole();
      }
    }

    difference() {
      right_side_main();
      main_pcb_side_cutout();
      mesh_cutout();
      bolt_holes();
    }
  }

  module back_grid() {
    mesh_gap = 0.5;
    module hex_grid() {
      hex_grid_inset = 5;
      hex_grid_size = 8;
      hex_grid_thickness = 1.5;
      tz(support_zsize - back_grid_zsize) intersection() {
        translate([
            LED_PANEL_64_32_BACK_WIDTH / 2,
            LED_PANEL_64_32_BACK_WIDTH * 3 / 4,
            -overlap * 2]) honeycomb([
            LED_PANEL_64_32_BACK_WIDTH,
            LED_PANEL_64_32_BACK_WIDTH,
            back_grid_zsize + overlap * 4], hex_grid_size, hex_grid_thickness);
        translate([
            support_side_thickness + hex_grid_inset,
            support_arm_yoffset + hex_grid_inset,
            -overlap]) cube([
              LED_PANEL_64_32_BACK_WIDTH - support_side_thickness * 2 - hex_grid_inset * 2,
              support_length -support_arm_yoffset - mesh_gap - hex_grid_inset * 2,
              back_grid_zsize + overlap * 2]);
      }
    }
    module block() {
      translate([
          support_side_thickness - overlap,
          support_arm_yoffset,
          support_zsize - back_grid_zsize]) cube([
            LED_PANEL_64_32_BACK_WIDTH - support_side_thickness * 2 + overlap * 2,
            support_length -support_arm_yoffset - mesh_gap + overlap,
            back_grid_zsize]);
    }
    difference() {
      block();
      hex_grid();
    }
  }

  union() {
    left_side();
    right_side();
    back_grid();
  }
}

$fa=2;
$fs=0.2;
led_panel();
placed_main_pcb();
placed_button_pcb();
bottom_support();

