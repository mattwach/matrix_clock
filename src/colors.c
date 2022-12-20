#include "colors.h"

struct Color {
  uint32_t value;
  const char* name;
};

// defines colors
static struct Color colors[10] = {
  { 0x654321, "brown" },      // 0
  { 0xFF0000, "red" },        // 1
  { 0xD05000, "orange" },     // 2
  { 0xFFFF10, "yellow" },     // 3
  { 0x00FF00, "green" },      // 4
  { 0x0000FF, "blue" },       // 5
  { 0x00FFFF, "cyan" },       // 6
  { 0xFF00FF, "magenta" },    // 7
  { 0xFF9090, "pink" },       // 8
  { 0xFFFFFF, "white" },      // 9
};

uint32_t get_color(uint8_t number) {
  return colors[number].value;
}

const char* get_color_name(uint8_t number) {
  return colors[number].name;
}
