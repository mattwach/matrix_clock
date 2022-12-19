#ifndef NUMBER_DRAW_H
#define NUMBER_DRAW_H
// Draws a number to the LED matrix

#include <inttypes.h>

// Font definitions which must align with the font[] structure defined in number_draw.c
#define FONT3x7         0
#define FONT3x5         1
#define FONT4x4         2
#define BINARY_FONT_4x4 3

// draws a digit.  Off screen for x,y  is OK
void number_draw(
    uint32_t* led,
    uint8_t digit,
    int8_t x,
    int8_t y,
    uint32_t color,
    uint8_t fnot);

// draws two digits in predetermined locations
void draw_numbers(uint32_t* led, uint8_t val, int8_t brightness);

#endif
