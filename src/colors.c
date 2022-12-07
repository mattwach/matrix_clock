#include "colors.h"

// defines colors
static uint32_t colors[10] = {
  0x000101, // 0 - black
  0xF00000, // 1 - red
  0xD05000, // 2 - orange
  0xFFFF10, // 3 - yellow
  0x00FF00, // 4 - green
  0x0000FF, // 5 - blue
  0x3B0062, // 6 - indigo
  0x8F20FF, // 7 - violet
  0x080810, // 8 - dark blue
  0xFFFFFF, // 9 - white
};

uint32_t get_color(uint8_t number) {
  return colors[number];
}
