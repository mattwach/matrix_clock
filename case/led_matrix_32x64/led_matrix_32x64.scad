use <mattwach/util.scad>
include <mattwach/vitamins/electronics/led_panel_64x32.scad>
include <mattwach/honeycomb.scad>
include <dep/button_pcb.scad>
include <dep/main_pcb.scad>

overlap = 0.01;

pcb_thickness = 1.6;
pcb_clearance = pcb_thickness + 0.2;
placed_button_pcb_xoffset = 13;
placed_button_pcb_yoffset = 3.5;
placed_button_pcb_zoffset = 3;

back_grid_zsize = 2;
hex_grid_inset = 5;
hex_grid_size = 8;
hex_grid_thickness = 1.5;
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
      9 + placed_button_pcb_zoffset]) rz(-90) ry(90) button_pcb();
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

module back_grid() {
  grid_ysize = support_length - 66.7;
  mesh_gap = 0.5;
  module hex_grid() {
    tz(support_zsize - back_grid_zsize) intersection() {
      translate([
          LED_PANEL_64_32_BACK_WIDTH / 2,
          grid_ysize / 2,
          -overlap * 2]) honeycomb([
          LED_PANEL_64_32_BACK_WIDTH,
          LED_PANEL_64_32_BACK_WIDTH,
          back_grid_zsize + overlap * 4], hex_grid_size, hex_grid_thickness);
      translate([
          support_side_thickness + hex_grid_inset,
          hex_grid_inset,
          -overlap]) cube([
            LED_PANEL_64_32_BACK_WIDTH - support_side_thickness * 2 - hex_grid_inset * 2,
            grid_ysize - mesh_gap - hex_grid_inset * 2,
            back_grid_zsize + overlap * 2]);
    }
  }
  module block() {
    translate([
        support_side_thickness - overlap,
        0,
        support_zsize - back_grid_zsize]) cube([
          LED_PANEL_64_32_BACK_WIDTH - support_side_thickness * 2 + overlap * 2,
          grid_ysize - mesh_gap + overlap,
          back_grid_zsize]);
  }
  difference() {
    block();
    hex_grid();
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

  module bottom_side() {
    bottom_side_thickness = 7;
    module main() {
      tx(support_side_thickness - overlap) cube([
            LED_PANEL_64_32_BACK_WIDTH - support_side_thickness * 2 + overlap * 2,
            bottom_side_thickness,
            support_zsize]);
    }

    module main_pcb_cutout() {
      pcb_width = 69.2;
      pcb_inset = 1.2;
      translate([
          LED_PANEL_64_32_BACK_WIDTH - pcb_width - support_side_thickness,
          bottom_side_thickness - pcb_inset,
          -overlap]) cube([
            pcb_width + overlap * 2,
            pcb_inset + overlap,
            pcb_clearance + overlap]);
    }

    module usb_cutout() {
      usb_cutout_width = 17;
      usb_cutout_height = support_zsize - 2;
      usb_cutout_xoffset = 80;
      usb_cutout_fillet = 5;

      module top_corner() {
        translate([
            usb_cutout_fillet,
            0,
            usb_cutout_height - usb_cutout_fillet]) rx(-90) cylinder(r=usb_cutout_fillet, h=bottom_side_thickness + overlap * 2);
      }

      translate([
          usb_cutout_xoffset,
          -overlap,
          -overlap]) hull() {
        cube([
            usb_cutout_width,
            bottom_side_thickness + overlap * 2,
            1]);
        top_corner();
        tx(usb_cutout_width - usb_cutout_fillet * 2) top_corner();
      }
    }

    module button_front_cutout() {
      button_cutout_width = 20.5;
      button_cutout_height = support_zsize - 2;
      button_cutout_xoffset = placed_button_pcb_xoffset + 14;
      button_cutout_fillet = 3;

      module top_corner() {
        translate([
            button_cutout_fillet,
            0,
            button_cutout_height - button_cutout_fillet]) rx(-90) cylinder(r=button_cutout_fillet, h=bottom_side_thickness + overlap * 2);
      }

      translate([
          button_cutout_xoffset,
          -overlap,
          -overlap]) hull() {
        cube([
            button_cutout_width,
            bottom_side_thickness + overlap * 2,
            1]);
        top_corner();
        tx(button_cutout_width - button_cutout_fillet * 2) top_corner();
      }
    }

    module button_back_cutout() {
      pcb_padding = 1;
      pcb_width = 42;
      pcb_height = 9;
      pcb_yoffset = placed_button_pcb_yoffset;
      translate([
          placed_button_pcb_xoffset - pcb_padding,
          pcb_yoffset,
          -overlap]) cube([
            pcb_width + pcb_padding * 2,
            bottom_side_thickness - pcb_yoffset + overlap,
            placed_button_pcb_zoffset + pcb_height + pcb_padding + overlap]);
    }

    module button_bolt_holes() {
      bolt_hole_span = 36;
      module bolt_hole() {
        bolt_hole_xoffset = 3;
        bolt_hole_zoffset = 4;
        translate([
            placed_button_pcb_xoffset + bolt_hole_xoffset,
            -overlap,
            placed_button_pcb_zoffset + bolt_hole_zoffset]) rx(-90) cylinder(d=1.85, h=bottom_side_thickness + overlap * 2);
      }

      bolt_hole();
      tx(bolt_hole_span) bolt_hole();
    }

    difference() {
      main();
      main_pcb_cutout();
      usb_cutout();
      button_front_cutout();
      button_back_cutout();
      button_bolt_holes();
    }
  }

  union() {
    left_side();
    right_side();
    bottom_side();
    ty(support_arm_yoffset) back_grid();
  }
}

module top_support() {
  top_ysize = 6;
  module left_side() {
    module left_side_main() {
      cube([
          support_side_thickness,
          support_length,
          support_zsize]);
    }

    module mesh_cutout() {
      translate([
          -overlap,
          -overlap,
          mesh_zsize]) cube([
            support_side_thickness + overlap * 2,
            support_side_thickness + overlap,
            support_zsize - mesh_zsize + overlap]);
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

  module top_side() {
    translate([
        support_side_thickness,
        support_length - top_ysize,
        0]) cube([
          LED_PANEL_64_32_BACK_WIDTH - support_side_thickness * 2 + overlap * 2,
          top_ysize,
          support_zsize]);
  }

  module hanging_mount() {
    notch_offset = top_ysize + 2;
    notch_width = 3;
    notch_depth = 5;
    mount_zsize = 2;

    module support() {
      lead_in_ysize = 25;
      xwest = support_side_thickness - overlap;
      xeast = LED_PANEL_64_32_BACK_WIDTH - support_side_thickness + overlap;
      ynorth = support_length - top_ysize + overlap;
      ysouth = ynorth - lead_in_ysize;
      xcenter_west = LED_PANEL_64_32_BACK_WIDTH / 2 - notch_width / 2;
      xcenter_east = xcenter_west + notch_width;
      ycenter = support_length - notch_offset + overlap - notch_depth;

      tz(support_zsize - mount_zsize) linear_extrude(mount_zsize) polygon([
          [xwest, ynorth],
          [xwest, ysouth],
          [xcenter_west, ycenter],
          [xcenter_east, ycenter],
          [xeast, ysouth],
          [xeast, ynorth],
          [xwest, ynorth],
      ]);
    }

    module hanging_hole() {
      translate([
          LED_PANEL_64_32_BACK_WIDTH / 2,
          support_length - notch_offset - notch_width / 2,
          support_zsize - mount_zsize - overlap]) union() {
        cylinder(d=notch_width, h=mount_zsize + overlap * 2);
        txy(-notch_width / 2, -notch_depth + notch_width / 2 - overlap)
          cube([
              notch_width,
              notch_depth - notch_width / 2 + overlap,
              mount_zsize + overlap * 2]);
      }
    }

    difference() {
      support();
      hanging_hole();
    }
  }

  top_support_yoffset =
    LED_PANEL_62_32_BOLT_HOLE_LOFFSET +
    LED_PANEL_62_32_BOLT_HOLE_LSPAN - 
    support_side_thickness / 2;
  ty(top_support_yoffset) union() {
    left_side();
    tx(LED_PANEL_64_32_BACK_WIDTH - support_side_thickness) left_side();
    top_side();
    ty(support_side_thickness) back_grid();
    hanging_mount();
  }
}

$fa=2;
$fs=0.2;
led_panel();
placed_main_pcb();
placed_button_pcb();
color("#855") bottom_support();
color("#558") top_support();

