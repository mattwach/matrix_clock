use <../../util.scad>

DOTSTAR_8X8_BASE_WIDTH = 25.4;
DOTSTAR_8X8_WIDTH_WITH_TABS = 35.8;
DOTSTAR_8X8_TAB_WIDTH = (DOTSTAR_8X8_WIDTH_WITH_TABS - DOTSTAR_8X8_BASE_WIDTH) / 2;
DOTSTAR_8X8_TAB_HOLE_DIAMETER = 3.3;
DOTSTAR_8X8_PCB_THICKNESS = 1.5;
DOTSTAR_8X8_THICKNESS = 2.4;
DOTSTAR_8X8_BOLT_SPAN = DOTSTAR_8X8_BASE_WIDTH - DOTSTAR_8X8_TAB_WIDTH;

module adafruit_dotstar_8x8_matrix(include_tabs=true) {
  overlap = 0.01;

  module base_pcb() {
    color("#222") cube([
        DOTSTAR_8X8_BASE_WIDTH,
        DOTSTAR_8X8_BASE_WIDTH,
        DOTSTAR_8X8_PCB_THICKNESS]);
  }

  module tab() {
    module main() {
      color("#222") cube([
          DOTSTAR_8X8_TAB_WIDTH,
          DOTSTAR_8X8_TAB_WIDTH,
          DOTSTAR_8X8_PCB_THICKNESS]);
    }
    module hole() {
      translate([
          DOTSTAR_8X8_TAB_WIDTH / 2,
          DOTSTAR_8X8_TAB_WIDTH / 2,
          -overlap]) cylinder(
          d=DOTSTAR_8X8_TAB_HOLE_DIAMETER,
          DOTSTAR_8X8_PCB_THICKNESS + overlap * 2);
    }
    difference() {
      main();
      hole();
    }
  }

  module pads() {
    pad_side_offset = 1.27;
    pad_diameter = 2;
    pad_spacing = 2.54;
    pad_depth = 0.1;
    pad_text_size = 1.2;
    module pad() {
      tz(-overlap) color("gold") cylinder(d=pad_diameter, h=pad_depth);
    }

    module bottom_pads() {
      module pad_with_label(label) {
        txy(pad_side_offset, pad_side_offset) pad();
        translate([
            pad_side_offset - (pad_text_size / 2),
            pad_spacing,
            pad_depth - overlap])
          color("white")
          ry(180) rz(90)
          linear_extrude(pad_depth) text(label, size=pad_text_size);
      }

      pad_with_label("COUT");
      tx(pad_spacing) pad_with_label("DOUT");
      tx(pad_spacing * 2) pad_with_label("GND");
      tx(pad_spacing * 3) pad_with_label("+5V");
    }

    module top_pads() {
      module pad_with_label(label) {
        txy(DOTSTAR_8X8_BASE_WIDTH - pad_side_offset,
            DOTSTAR_8X8_BASE_WIDTH - pad_side_offset) pad();
        translate([
            DOTSTAR_8X8_BASE_WIDTH - pad_side_offset - (pad_text_size / 2),
            DOTSTAR_8X8_BASE_WIDTH - pad_spacing,
            pad_depth - overlap])
          color("white")
          ry(180) rz(90)
          linear_extrude(pad_depth) text(label, size=pad_text_size, halign="right");
      }

      pad_with_label("CIN");
      tx(-pad_spacing) pad_with_label("DIN");
      tx(-pad_spacing * 2) pad_with_label("GND");
      tx(-pad_spacing * 3) pad_with_label("+5V");
    }

    module center_pads() {
      center_pad_yoffset = 14.5;
      center_pad_span = 6.2;
      center_pad_xoffset = (DOTSTAR_8X8_BASE_WIDTH - center_pad_span) / 2;
      module pad_with_label(label) {
        txy(center_pad_xoffset, center_pad_yoffset) {
          pad();
          translate([
              0,
              pad_diameter / 2,
              pad_depth - overlap]) ry(180) color("white")
            linear_extrude(pad_depth)
            text(label, size=pad_text_size, halign="center");
        }
      }
      pad_with_label("+5V");
      tx(center_pad_span) pad_with_label("GND");
    }

    bottom_pads();
    top_pads();
    center_pads();
  }

  module led_matrix() {
    led_matrix_span = 23.7;
    led_side_count = 8;
    led_offset = (DOTSTAR_8X8_BASE_WIDTH - led_matrix_span) / 2;
    led_width = 2;
    led_spacing = (led_matrix_span - led_width) / (led_side_count - 1);
    module led() {
      led_thickness = DOTSTAR_8X8_THICKNESS - DOTSTAR_8X8_PCB_THICKNESS;
      translate([
          led_offset,
          led_offset,
          DOTSTAR_8X8_PCB_THICKNESS]) color("#ddd", 0.5) cube([
          led_width,
          led_width,
          led_thickness]);
    }
    for (y = [0:led_side_count-1]) {
      for (x = [0:led_side_count-1]) {
        txy(x * led_spacing, y * led_spacing) led();
      }
    }
  }

  base_pcb();

  if (include_tabs) {
    tx(-DOTSTAR_8X8_TAB_WIDTH) tab();
    tx(DOTSTAR_8X8_BASE_WIDTH) tab();
    txy(-DOTSTAR_8X8_TAB_WIDTH,
        DOTSTAR_8X8_BASE_WIDTH - DOTSTAR_8X8_TAB_WIDTH) tab();
    txy(DOTSTAR_8X8_BASE_WIDTH,
        DOTSTAR_8X8_BASE_WIDTH - DOTSTAR_8X8_TAB_WIDTH) tab();
  }

  pads();
  led_matrix();
}

/*
$fa=2;
$fs=0.2;
adafruit_dotstar_8x8_matrix();
*/

