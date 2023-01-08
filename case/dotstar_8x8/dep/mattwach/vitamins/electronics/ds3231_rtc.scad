use <../../util.scad>
include <cr2032_battery.scad>

DS3231_RTC_LENGTH = 37;
DS3231_RTC_WIDTH = 22.3;
DS3231_RTC_BOARD_THICKNESS = 1.6;

module ds3231_rtc() {
  overlap = 0.01;
  port_xoffset = 1.27;
  port_spacing = 2.54;
  port_text_size = 1.4;
  port_pad_thickness = 0.1;
  port_pad_size = 2;

  module board() {
    module pcb() {
      color("#008") cube([
          DS3231_RTC_LENGTH,
          DS3231_RTC_WIDTH,
          DS3231_RTC_BOARD_THICKNESS]);
    }

    module port() {
      port_hole_size = 0.8;

      module hole() {
        tz(-overlap) cylinder(
            d=port_hole_size,
            h=DS3231_RTC_BOARD_THICKNESS + overlap * 2);
      }

      module pad() {
        cylinder(d=port_pad_size, h=port_pad_thickness); 
      }

      color("#aaa") union() {
        hole();
        tz(-overlap) pad();
        tz(DS3231_RTC_BOARD_THICKNESS - port_pad_thickness + overlap) pad();
      }
    }

    module front_ports() {
      front_port_count = 6;
      front_port_span = (port_spacing * (front_port_count - 1));
      front_port_yoffset = (DS3231_RTC_WIDTH - front_port_span) / 2;

      module port_with_label(label) {
        port();
        translate([
            port_pad_size / 2,
            -port_text_size / 2,
            DS3231_RTC_BOARD_THICKNESS - port_pad_thickness + overlap])
          color("white")
          linear_extrude(port_pad_thickness)
          text(label, size=port_text_size);
      }

      front_port_labels = ["GND", "VCC", "SDA", "SCL", "SQW", "32K"];

      for (i = [0:front_port_count-1]) {
        txy(port_xoffset, front_port_yoffset + i * port_spacing)
          port_with_label(front_port_labels[i]);
      }
    }

    module back_ports() {
      back_port_count = 4;
      back_port_span = (port_spacing * (back_port_count - 1));
      back_port_yoffset = (DS3231_RTC_WIDTH - back_port_span) / 2;

      module port_with_label(label) {
        port();
        translate([
            -port_pad_size / 2,
            -port_text_size / 2,
            DS3231_RTC_BOARD_THICKNESS - port_pad_thickness + overlap])
          color("white")
          linear_extrude(port_pad_thickness)
          text(label, size=port_text_size, halign="right");
      }

      back_port_labels = ["GND", "VCC", "SDA", "SCL"];

      for (i = [0:back_port_count-1]) {
        txy(DS3231_RTC_LENGTH - port_xoffset,
            back_port_yoffset + i * port_spacing)
          port_with_label(back_port_labels[i]);
      }
    }

    module mounting_holes() {
      mounting_hole_xspan = 25.5;
      mounting_hole_yspan = 18;
      mounting_hole_xoffset = 7.6;
      mounting_hole_yoffset = (DS3231_RTC_WIDTH - mounting_hole_yspan) / 2;
      module mounting_hole() {
        mounting_hole_diameter = 2.8;
        tz(-overlap) cylinder(
            d=mounting_hole_diameter,
            h=DS3231_RTC_BOARD_THICKNESS + overlap * 2);
      }

      txy(mounting_hole_xoffset, mounting_hole_yoffset) mounting_hole();
      txy(mounting_hole_xoffset,
          DS3231_RTC_WIDTH - mounting_hole_yoffset) mounting_hole();
      txy(mounting_hole_xoffset + mounting_hole_xspan,
          DS3231_RTC_WIDTH - mounting_hole_yoffset) mounting_hole();
    }

    difference() {
      pcb();
      front_ports();
      back_ports();
      mounting_holes();
    }
  }

  module ds3231() {
    ds3231_length = 10.3;
    ds3231_width = 7.6;
    ds3231_thickness = 2.2;
    ds3231_xoffset = 11;
    ds3231_yoffset = 3.3;

    translate([
        DS3231_RTC_LENGTH - ds3231_length - ds3231_xoffset,
        DS3231_RTC_WIDTH - ds3231_width - ds3231_yoffset,
        DS3231_RTC_BOARD_THICKNESS]) color("#222") cube([
        ds3231_length,
        ds3231_width,
        ds3231_thickness]);
  }

  module eeprom() {
    eeprom_length = 5;
    eeprom_width = 3.8;
    eeprom_thickness = 1.2;
    eeprom_xoffset = 15.9;
    eeprom_yoffset = 2.4;

    translate([
        eeprom_xoffset,
        eeprom_yoffset,
        DS3231_RTC_BOARD_THICKNESS]) color("#222") cube([
        eeprom_length,
        eeprom_width,
        eeprom_thickness]);
  }

  module battery_holder() {
    shell_thickness = 0.4;
    outer_shell_max_height = 8.6;
    outer_shell_diameter = 22.3;
    outer_shell_xoffset = DS3231_RTC_LENGTH - outer_shell_diameter / 2 - 6.3;
    outer_shell_yoffset = DS3231_RTC_WIDTH / 2;
    module outer_shell() {
      module main() {
        cylinder(d=outer_shell_diameter, h=outer_shell_max_height);
      }

      module spring_holder() {
        spring_holder_width = 7.4;
        spring_holder_thickness = 2.4;
        txy(-outer_shell_diameter / 2 - spring_holder_thickness,
            -spring_holder_width / 2) cube([
              outer_shell_diameter / 2,  // just trying to fill the cylinder rect gap
              spring_holder_width,
              outer_shell_max_height]);
      }

      union() {
        main();
        spring_holder();
      }
    }

    module battery_opening() {
      tz(-overlap) cylinder(d=CR2032_DIAMETER, h=outer_shell_max_height);
    }

    module cutout_profile() {
      profile_depth = 4.5;
      descend_width = 6;
      bottom_width = 11.8;
      rise_width = 3.3;
      translate([
          -outer_shell_diameter / 2,
          -outer_shell_diameter / 2 - overlap,
          -overlap]) rx(-90) linear_extrude(outer_shell_diameter + overlap * 2) polygon([
          [0, 0],
          [descend_width, -profile_depth],
          [descend_width + bottom_width, -profile_depth],
          [descend_width + bottom_width + rise_width, -profile_depth + rise_width],
          [outer_shell_diameter, -profile_depth + rise_width],
          [outer_shell_diameter, 0]
      ]);
    }

    translate([
        outer_shell_xoffset,
        outer_shell_yoffset,
        -outer_shell_max_height
    ]) color("#222") difference() {
      outer_shell();
      battery_opening();
      cutout_profile();
    }

    translate([
        outer_shell_xoffset,
        outer_shell_yoffset,
        -shell_thickness - 1.5]) rx(180) cr2032();
  }

  board();
  ds3231();
  eeprom();
  battery_holder();
}

/*
$fa=2;
$fs=0.2;
ds3231_rtc();
*/
