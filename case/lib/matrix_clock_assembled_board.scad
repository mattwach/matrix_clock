use <mattwach/util.scad>
use <mattwach/vitamins/electronics/buttons.scad>
include <mattwach/vitamins/electronics/pi_pico.scad>
include <mattwach/vitamins/electronics/ds3231_rtc.scad>
include <NopSCADlib/core.scad>
include <NopSCADlib/vitamins/pin_headers.scad>

MATRIX_CLOCK_PCB_LENGTH = 63.5;
MATRIX_CLOCK_PCB_WIDTH = 54.6;
MATRIX_CLOCK_PCB_THICKNESS = 1.6;

module matrix_clock_assembled_board() {
  overlap = 0.01;

  module pcb() {
    color("#760") translate([
        MATRIX_CLOCK_PCB_LENGTH,
        MATRIX_CLOCK_PCB_WIDTH,
        0]) rz(-90) import("lib/matrix_clock_kicad_pcb.stl");
  }

  module pico() {
    translate([
        59.8,
        25.7,
        -overlap * 2]) ry(180) pi_pico();
  }

  module rtc() {
    pin_x_offset = 47.6;
    pin_y_offset = 4.5;
    pin_z_offset = 2.4 + MATRIX_CLOCK_PCB_THICKNESS + 3;
    module clock_pins() {
      translate([
          pin_x_offset,
          pin_y_offset,
          pin_z_offset]) ry(180) pin_header(2p54header, 4, 1);
    }

    module clock_module() {
      translate([
          58.8,
          40.3,
          pin_z_offset + 1.6]) rz(-90) rx(180) ds3231_rtc();
    }

    clock_pins();
    clock_module();
  }

  module led_matrix_interface() {
    translate([
        3.8,
        41.9,
        MATRIX_CLOCK_PCB_THICKNESS]) rz(-90) jst_xh_header(jst_xh_header, 4);
  }

  module interface_buttons() {
      button_y_offset = 4.6;
      button_z_offset = 2;
      translate([
          10.2,
          button_y_offset,
          button_z_offset]) push_switch_12x12(include_button=false);
      translate([
          24.8,
          button_y_offset,
          button_z_offset]) push_switch_12x12(include_button=false);
  }

  module reset_button() {
    reset_button_length = 6.3;
    reset_button_width = 2.4;
    reset_button_height = 3;
    translate([
        26,
        50,
        -reset_button_height]) cube([
          reset_button_length,
          reset_button_width,
          reset_button_height]);
  }

  pcb();
  pico();
  rtc();
  led_matrix_interface();
  interface_buttons();
  reset_button();
}

/*
$fa=2;
$fs=0.2;
matrix_clock_assembled_board();
*/
