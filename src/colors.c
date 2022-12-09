#include "colors.h"

struct Color {
  uint32_t value;
  const char* name;
};

// defines colors
static struct Color colors[10] = {
  { 0x000101, "black" },     // 0
  { 0xF00000, "red" },       // 1
  { 0xD05000, "orange" },    // 2
  { 0xFFFF10, "yellow" },    // 3
  { 0x00FF00, "green" },     // 4
  { 0x0000FF, "blue" },      // 5
  { 0x3B0062, "indigo" },    // 6
  { 0x8F20FF, "violet" },    // 7
  { 0x080860, "dark blue" }, // 8
  { 0xFFFFFF, "white" },     // 9
};

uint32_t get_color(uint8_t number) {
  return colors[number].value;
}

const char* get_color_name(uint8_t number) {
  return colors[number].name;
}