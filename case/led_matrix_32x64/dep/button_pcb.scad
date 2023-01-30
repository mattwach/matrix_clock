use <mattwach/util.scad>
include <NopSCADlib/core.scad>
include <NopSCADlib/vitamins/buttons.scad>
include <NopSCADlib/vitamins/pin_headers.scad>

module button_pcb() {
  pcb_zsize = 1.6;

  module pcb() {
    rz(90) color("#262") import("dep/button_pcb_kicad.stl");
  }

  module buttons() {
    module button() {
      translate([
          4.65,
          18.6,
          pcb_zsize
      ]) square_button(button_6mm);
    }

    button();
    ty(10.7) button();
  }

  module pins() {
    translate([
        4.5,
        9,
        0]) rz(180) ry(180) pin_header(2p54header, 3, 1, right_angle=true);
  }

  pcb();
  buttons();
  pins();
}

/*
$fa=2;
$fs=0.2;
button_pcb();
*/

