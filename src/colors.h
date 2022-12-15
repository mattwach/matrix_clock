#ifndef COLORS_H
#define COLORS_H

#include <inttypes.h>

// pass a number 0-9 to get a color in 0xVVRRGGBB format where
// VV is birghtness
// RR is red
// GG is green
// BB is blue
// All range 0-255
uint32_t get_color(uint8_t number);
// pass a number to gt a color name
const char* get_color_name(uint8_t number);

#endif
