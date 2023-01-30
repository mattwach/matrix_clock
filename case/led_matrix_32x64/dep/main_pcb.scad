use <mattwach/util.scad>
use <mattwach/vitamins/electronics/pi_pico.scad>

module main_pcb() {
  pcb_zsize = 1.6;

  rz(90) color("#262") import("main_pcb.stl");
  translate([
      58.65,
      54.2,
      pcb_zsize]) rz(180) pi_pico();
}

$fa=2;
$fs=0.2;
main_pcb();
