#ifndef NUMBER_DRAW_H
#define NUMBER_DRAW_H
// Draws a number to the LED matrix

#include <inttypes.h>

#define NUMBER_WIDTH 3
#define NUMBER_HEIGHT 7

// draws a digit
void number_draw(uint32_t* led, uint8_t digit, uint8_t x, uint8_t y, uint32_t color);

// draws two digits in predetermined locations
void draw_numbers(uint32_t* led, uint8_t val, int8_t brightness);

#endif
