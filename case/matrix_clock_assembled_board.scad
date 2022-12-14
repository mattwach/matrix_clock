use <mattwach/util.scad>
include <mattwach/vitamins/electronics/pi_pico.scad>

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

  pcb();
  pico();
}

$fa=2;
$fs=0.2;
matrix_clock_assembled_board();
