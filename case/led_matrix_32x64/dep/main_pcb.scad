use <mattwach/util.scad>
use <mattwach/vitamins/electronics/pi_pico.scad>
include <mattwach/vitamins/electronics/ds3231_rtc.scad>
include <NopSCADlib/core.scad>
include <NopSCADlib/vitamins/pin_headers.scad>

module main_pcb() {
  pcb_zsize = 1.6;

  module pcb() {
    rz(90) color("#262") import("main_pcb.stl");
  }

  module pico() {
    translate([
        58.65,
        54.2,
        pcb_zsize]) rz(180) pi_pico();
  }

  module rtc() {
    pin_x_offset = 41.05;
    pin_y_offset = 4.05;
    pin_z_offset = -2.54;
    module clock_pins() {
      translate([
          pin_x_offset,
          pin_y_offset,
          pin_z_offset]) pin_header(2p54header, 4, 1);
    }

    module clock_module() {
      translate([
          29.9,
          -31.7,
          -2.54]) ry(180) rz(90) ds3231_rtc();
    }

    clock_pins();
    clock_module();
  }

  module led_interface_pins() {
    translate([
        23.25,
        13.35,
        pcb_zsize]) rz(-90) pin_header(2p54header, 8, 2, right_angle=true);
  }

  module button_pins() {
    translate([
        55,
        21.6,
        pcb_zsize]) pin_header(2p54header, 3, 1, right_angle=true);
  }

  pcb();
  pico();
  rtc();
  led_interface_pins();
  button_pins();
}

$fa=2;
$fs=0.2;
main_pcb();
