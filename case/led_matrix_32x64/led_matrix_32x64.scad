use <mattwach/util.scad>
include <mattwach/vitamins/electronics/led_panel_64x32.scad>
include <dep/button_pcb.scad>
include <dep/main_pcb.scad>

overlap = 0.01;
pcb_thickness = 1.6;
pcb_clearance = pcb_thickness + 0.2;
placed_button_pcb_inset = 3.4;


support_zsize = 10;
mesh_zsize = 4;
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
      10,
      placed_button_pcb_inset + pcb_thickness,
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

  right_side();
}

$fa=2;
$fs=0.2;
led_panel();
placed_main_pcb();
placed_button_pcb();
bottom_support();

