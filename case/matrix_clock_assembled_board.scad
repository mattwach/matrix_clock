use <mattwach/util.scad>
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
        0]) rz(-90) import("matrix_clock_kicad_pcb.stl");
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
    module board_pin_header() {
      translate([
          pin_x_offset,
          pin_y_offset,
          MATRIX_CLOCK_PCB_THICKNESS]) pin_socket(2p54header, 4, 1);
    }

    module clock_pins() {
      translate([
          pin_x_offset,
          pin_y_offset,
          12.6]) ry(180) pin_header(2p54header, 4, 1);
    }

    module clock_module() {
      translate([
          58.8,
          40.3,
          14.2]) rz(-90) rx(180) ds3231_rtc();
    }

    board_pin_header();
    clock_pins();
    clock_module();
  }

  pcb();
  pico();
  rtc();
}

$fa=2;
$fs=0.2;
matrix_clock_assembled_board();
