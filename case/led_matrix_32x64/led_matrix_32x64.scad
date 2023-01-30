use <mattwach/util.scad>
include <mattwach/vitamins/electronics/led_panel_64x32.scad>
include <dep/main_pcb.scad>

module led_panel() {
  rz(90) rx(180) led_panel_64x32();
}

module placed_main_pcb() {
  txy(44.8, 68) rz(-90) main_pcb();
}

$fa=2;
$fs=0.2;
led_panel();
placed_main_pcb();

