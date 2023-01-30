use <mattwach/util.scad>
include <mattwach/vitamins/electronics/led_panel_64x32.scad>

module led_panel() {
      rz(90) rx(180) led_panel_64x32();
}

$fa=2;
$fs=0.2;
led_panel();

