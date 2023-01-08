use <../../util.scad>

PI_PICO_LENGTH = 51;
PI_PICO_WIDTH = 21;
PI_PICO_BOARD_THICKNESS = 1.1;
PI_PICO_SIDE_PIN_COUNT = 20;
PI_PICO_PIN_SPACING = 2.54;
PI_PICO_PIN_INSET = 1.61;
PI_PICO_SIDE_PIN_SPAN = PI_PICO_PIN_SPACING * (PI_PICO_SIDE_PIN_COUNT - 1);
PI_PICO_SIDE_PIN_XOFFSET = (PI_PICO_LENGTH - PI_PICO_SIDE_PIN_SPAN) / 2;

module pi_pico() {
  overlap = 0.01;
  end_pin_count = 3;
  end_pin_span = PI_PICO_PIN_SPACING * (end_pin_count - 1);
  end_pin_ypad = (PI_PICO_WIDTH - end_pin_span) / 2;

  module main_board() {
    color("#080") cube([
        PI_PICO_LENGTH,
        PI_PICO_WIDTH,
        PI_PICO_BOARD_THICKNESS]);
  }

  module interface_pins() {
    module pico_pin(make_square) {
      gold_pad_width = 1.6;
      gold_pad_thickness = 0.1;
      pin_hole_size = 1;
      module gold_pad() {
        union() {
          if (make_square) {
            txy(-gold_pad_width/2, -overlap) cube([gold_pad_width, gold_pad_width / 2 + overlap, gold_pad_thickness]);  
          } else {
            cylinder(d=gold_pad_width, h=gold_pad_thickness);  
          }
          txy(-gold_pad_width/2, -PI_PICO_PIN_INSET)
            cube([gold_pad_width, PI_PICO_PIN_INSET, gold_pad_thickness]);
        }
      }

      color("gold") union() {
        tz(-overlap) gold_pad();
        tz(PI_PICO_BOARD_THICKNESS - gold_pad_thickness + overlap) gold_pad();
        cylinder(d=pin_hole_size, h=PI_PICO_BOARD_THICKNESS);
        txy(-gold_pad_width/2, -PI_PICO_PIN_INSET - overlap)
          cube([gold_pad_width, gold_pad_thickness, PI_PICO_BOARD_THICKNESS]);
        ty(-PI_PICO_PIN_INSET) cylinder(d=pin_hole_size, h=PI_PICO_BOARD_THICKNESS);
      }
    }

    for (i = [0:PI_PICO_SIDE_PIN_COUNT-1]) {
      tx(PI_PICO_SIDE_PIN_XOFFSET + PI_PICO_PIN_SPACING * i) {
        ty(PI_PICO_PIN_INSET - overlap) pico_pin(((i - 2) % 5) == 0);
        ty(PI_PICO_WIDTH - PI_PICO_PIN_INSET + overlap)
          rz(180) pico_pin(((i - 2) % 5) == 0);
      }
    }
    for (i = [0:end_pin_count-1]) {
      txy(PI_PICO_LENGTH - PI_PICO_PIN_INSET,
          end_pin_ypad + PI_PICO_PIN_SPACING * i) rz(90) pico_pin(i == 1);
    }
  }

  module mounting_holes() {
    mounting_hole_xpad = 2;
    mounting_hole_xspan = PI_PICO_LENGTH - (mounting_hole_xpad * 2);
    mounting_hole_yspan = 11.4;
    mounting_hole_ypad = (PI_PICO_WIDTH - mounting_hole_yspan) / 2;
    module mounting_hole() {
      mounting_hole_diameter = 2.1;
      cylinder(
          d = mounting_hole_diameter,
          h=PI_PICO_BOARD_THICKNESS + overlap * 2);
    }
    tz(-overlap) {
      txy(mounting_hole_xpad, mounting_hole_ypad) mounting_hole();
      txy(mounting_hole_xpad + mounting_hole_xspan, mounting_hole_ypad)
        mounting_hole();
      txy(mounting_hole_xpad, mounting_hole_ypad + mounting_hole_yspan)
        mounting_hole();
      txy(
          mounting_hole_xpad + mounting_hole_xspan,
          mounting_hole_ypad + mounting_hole_yspan) mounting_hole();
    }
  }

  module RP2040() {
    rp2040_width = 7;
    rp2040_thickness = 2 - PI_PICO_BOARD_THICKNESS;
    translate([
        22.9,
        (PI_PICO_WIDTH - rp2040_width) / 2,
        PI_PICO_BOARD_THICKNESS]) color("#222") cube([
          rp2040_width,
          rp2040_width,
          rp2040_thickness]);
  }

  module flash() {
    flash_length = 3;
    flash_width = 2;
    flash_thickness = 1.65 - PI_PICO_BOARD_THICKNESS;
    flash_xoffset = 18.2;
    flash_yoffset = 5.8;
    translate([
        flash_xoffset,
        flash_yoffset,
        PI_PICO_BOARD_THICKNESS]) color("#222") cube([
          flash_length,
          flash_width,
          flash_thickness]);
  }

  module bootsel_button() {
    bootsel_base_thickness = 2.9 - PI_PICO_BOARD_THICKNESS;
    bootsel_xoffset = 10.5;
    bootsel_yoffset = 5.5;
    bootsel_length = 4.5;
    bootsel_width = 3.4;
    bs_button_width = 2.2;
    bs_button_length = 3;
    bs_button_span = bs_button_length - bs_button_width;
    bs_button_thickness = 3.6 - bootsel_base_thickness - PI_PICO_BOARD_THICKNESS;
    module bootsel_base() {
      color("#aaa") cube([
          bootsel_length,
          bootsel_width,
          bootsel_base_thickness]);
    }
    module bootsel_button() {
      module endcap() {
        cylinder(d=bs_button_width, h=bs_button_thickness);
      }
      color("white") hull() {
        endcap();
        tx(bs_button_span) endcap();
      }
    }
    translate([
        bootsel_xoffset,
        bootsel_yoffset,
        PI_PICO_BOARD_THICKNESS]) bootsel_base();
    translate([
        bootsel_xoffset + ((bootsel_length - bs_button_span) / 2),
        bootsel_yoffset + (bootsel_width / 2),
        PI_PICO_BOARD_THICKNESS + bootsel_base_thickness]) bootsel_button();
  }

  module usb_micro() {
    usb_micro_length = 5.7;
    usb_micro_width = 7.3;
    usb_micro_thickness = 2.7;
    usb_micro_xoffset = 1.3;

    module connector() {
      wall_thickness = 0.3;
      module outer() {
        color("#aaa") cube([
            usb_micro_length,
            usb_micro_width,
            usb_micro_thickness]);
      }
      module inner() {
        translate([
            -overlap,
            wall_thickness,
            wall_thickness
        ]) color("#aaa") cube([
            usb_micro_length - wall_thickness,
            usb_micro_width - wall_thickness * 2,
            usb_micro_thickness - wall_thickness * 2]);
      }
      difference() {
        outer();
        inner();
      }
    }

    translate([
        -usb_micro_xoffset,
        (PI_PICO_WIDTH - usb_micro_width) / 2,
        PI_PICO_BOARD_THICKNESS]) connector();
  }

  module oscillator() {
    oscillator_length = 2.5;
    oscillator_width = 3;
    oscillator_thickness = 1.8 - PI_PICO_BOARD_THICKNESS;
    oscillator_xoffset = 34.3;
    oscillator_yoffset = 7;
    translate([
        oscillator_xoffset,
        oscillator_yoffset,
        PI_PICO_BOARD_THICKNESS]) color("#aaa") cube([
          oscillator_length,
          oscillator_width,
          oscillator_thickness]);
  }

  module diode() {
    diode_length = 2.6; 
    diode_width = 1.6;
    diode_thickness = 2.2 - PI_PICO_BOARD_THICKNESS;
    diode_xoffset = 5.7;
    diode_yoffset = PI_PICO_WIDTH - diode_width - 4;
    translate([
        diode_xoffset,
        diode_yoffset,
        PI_PICO_BOARD_THICKNESS]) color("#222") cube([
          diode_length,
          diode_width,
          diode_thickness]);
  }

  module unknown_chip() {
    unknown_length = 2.7; 
    unknown_width = 2.1;
    unknown_thickness = 2.3 - PI_PICO_BOARD_THICKNESS;
    unknown_xoffset = 10.4;
    unknown_yoffset = PI_PICO_WIDTH - unknown_width - 3.8;
    translate([
        unknown_xoffset,
        unknown_yoffset,
        PI_PICO_BOARD_THICKNESS]) color("#222") cube([
          unknown_length,
          unknown_width,
          unknown_thickness]);
  }

  module vreg() {
    vreg_length = 2.4; 
    vreg_width = 2.4;
    vreg_thickness = 2.1 - PI_PICO_BOARD_THICKNESS;
    vreg_xoffset = 10.4;
    vreg_yoffset = PI_PICO_WIDTH - vreg_width - 6.5;
    translate([
        vreg_xoffset,
        vreg_yoffset,
        PI_PICO_BOARD_THICKNESS]) color("#222") cube([
          vreg_length,
          vreg_width,
          vreg_thickness]);
  }

  module vreg_caps() {
    cap_xspan = 5.1;
    module cap() {
      cap_length = 1.4; 
      cap_width = 2.1;
      cap_thickness = 2.3 - PI_PICO_BOARD_THICKNESS;
      cap_xoffset = 8.3;
      cap_yoffset = PI_PICO_WIDTH - cap_width - 6.6;
      translate([
          cap_xoffset,
          cap_yoffset,
          PI_PICO_BOARD_THICKNESS]) color("#522") cube([
            cap_length,
            cap_width,
            cap_thickness]);
    }
    cap();
    tx(cap_xspan) cap();
  }

  module led() {
    led_length = 0.9; 
    led_width = 1.7;
    led_thickness = 1.8 - PI_PICO_BOARD_THICKNESS;
    led_xoffset = 4.5;
    led_yoffset = 3.9;
    translate([
        led_xoffset,
        led_yoffset,
        PI_PICO_BOARD_THICKNESS]) color("#5f5", 0.5) cube([
          led_length,
          led_width,
          led_thickness]);
  }

  difference() {
    main_board();
    interface_pins();
    mounting_holes();
  }

  RP2040();
  flash();
  bootsel_button();
  usb_micro();
  oscillator();
  diode();
  unknown_chip();
  vreg();
  vreg_caps();
  led();
}

/*
$fa=2;
$fs=0.2;
pi_pico();
*/

