use <../../util.scad>

LED_PANEL_64_32_BACK_LENGTH = 253;
LED_PANEL_64_32_BACK_WIDTH = 125.8;
LED_PANEL_64_32_FRONT_LENGTH = 256;
LED_PANEL_64_32_FRONT_WIDTH = 128;
LED_PANEL_64_32_THICKNESS = 14;

LED_PANEL_64_32_BOLT_HOLE_WSPAN = 112;
LED_PANEL_64_32_BOLT_HOLE_WOFFSET = (
    LED_PANEL_64_32_BACK_WIDTH - LED_PANEL_64_32_BOLT_HOLE_WSPAN) / 2;
LED_PANEL_64_32_BOLT_HOLE_LSPAN = 120;
LED_PANEL_64_32_BOLT_HOLE_LOFFSET =
    LED_PANEL_64_32_BACK_LENGTH / 2 - LED_PANEL_64_32_BOLT_HOLE_LSPAN;


module led_panel_64x32() {
  overlap = 0.01;
  pcb_thickness = 2;
  side_support_thickness = 10.9;
  end_support_thickness = 10.7;
  inner_support_thinkness = 12;
  center_support_thickness = 13.8;

  end_cutout_xsize = 45.9;
  inner_cutout_width = LED_PANEL_64_32_BACK_WIDTH - (side_support_thickness * 2);
  cutout_depth = LED_PANEL_64_32_THICKNESS - pcb_thickness + overlap;

  inner_cutout_xsize =
    (LED_PANEL_64_32_BACK_LENGTH -
    end_support_thickness * 2 -
    inner_support_thinkness * 2 -
    end_cutout_xsize * 2 - 
    center_support_thickness) / 2;

  module shell() {
    xoffset = (LED_PANEL_64_32_BACK_LENGTH - LED_PANEL_64_32_FRONT_LENGTH) / 2;
    yoffset = (LED_PANEL_64_32_BACK_WIDTH - LED_PANEL_64_32_FRONT_WIDTH) / 2;

    hull() {
      cube([
          LED_PANEL_64_32_BACK_LENGTH,
          LED_PANEL_64_32_BACK_WIDTH,
          overlap]);
      translate([
          xoffset,
          yoffset,
          LED_PANEL_64_32_THICKNESS - pcb_thickness]) cube([
            LED_PANEL_64_32_FRONT_LENGTH,
            LED_PANEL_64_32_FRONT_WIDTH,
            pcb_thickness]);
    }
  }

  module end_cutout() {
    cutout_bump_span = 90;

    module main_cutout() {
      translate([
          end_support_thickness,
          side_support_thickness,
          -overlap]) cube([
          end_cutout_xsize,
          inner_cutout_width,
          cutout_depth]);
    }

    module slanted_cutout() {
      slanted_cutout_width1 = 33.8;
      slanted_cutout_support_thickness = 6;
      slanted_cutout_width2 = slanted_cutout_width1 + (end_support_thickness - slanted_cutout_support_thickness) * 2;
      hull() {
        translate([
            slanted_cutout_support_thickness,
            (LED_PANEL_64_32_BACK_WIDTH - slanted_cutout_width1) / 2,
            -overlap]) cube([
              overlap,
              slanted_cutout_width1,
              cutout_depth]);
        translate([
            end_support_thickness - overlap,
            (LED_PANEL_64_32_BACK_WIDTH - slanted_cutout_width2) / 2,
            -overlap]) cube([
              overlap * 2,
              slanted_cutout_width2,
              cutout_depth]);
      }
    }

    module bump() {
      cutout_bump_diameter = 13.5;
      cutout_bump_offset = (LED_PANEL_64_32_BACK_WIDTH - cutout_bump_span) / 2;
      translate([
          cutout_bump_diameter / 2,
          cutout_bump_offset,
          -overlap]) cylinder(
        d = cutout_bump_diameter,
        h = cutout_depth);
    }

   difference() {
     union() {
       main_cutout();
       slanted_cutout();
     }
     bump();
     ty(cutout_bump_span) bump();
    }
  }

  module middle_cutout() {
    translate([
        end_support_thickness + end_cutout_xsize + inner_support_thinkness,
        side_support_thickness,
        -overlap]) cube([
          inner_cutout_xsize,
          inner_cutout_width,
          cutout_depth]);
  }

  module bolt_holes() {
    module hole() {
      hole_diameter = 3;
      hole_depth = cutout_depth;
      tz(-overlap) cylinder(d=hole_diameter, h=hole_depth);
    }

    module hole_row() {
      hole();
      ty(LED_PANEL_64_32_BOLT_HOLE_WSPAN) hole();
    }

    txy(LED_PANEL_64_32_BOLT_HOLE_LOFFSET, LED_PANEL_64_32_BOLT_HOLE_WOFFSET) {
      hole_row();
      tx(LED_PANEL_64_32_BOLT_HOLE_LSPAN) hole_row();
      tx(LED_PANEL_64_32_BOLT_HOLE_LSPAN * 2) hole_row();
    }
  }

  difference() {
    color("#444") shell();
    color("#222") end_cutout();
    color("#222") txy(
        LED_PANEL_64_32_BACK_LENGTH,
        LED_PANEL_64_32_BACK_WIDTH
    ) rz(180) end_cutout();
    color("#222") middle_cutout();
    color("#222") tx(inner_cutout_xsize + center_support_thickness) middle_cutout();
    color("gold") bolt_holes();
  }
}

/*
$fa=2;
$fs=0.5;
led_panel_64x32();
*/
