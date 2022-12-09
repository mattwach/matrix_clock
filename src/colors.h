#ifndef COLORS_H
#define COLORS_H

#include <inttypes.h>

// pass a number 0-9
uint32_t get_color(uint8_t number);
const char* get_color_name(uint8_t number);

#endif