#ifndef NUMBER_DRAW_H
#define NUMBER_DRAW_H
// Draws a number to the LED matrix

#include <inttypes.h>

#define DRAW_MODE_OVERLAY 0
#define DRAW_MODE_WHITE 1

struct NumberFont {
  int16_t x;
  int16_t y;
  uint8_t brightness;
  uint8_t char_width;
  uint8_t char_height;
  uint8_t char_spacing;
};

// initialize a font
// char spacing does not include char_width
// char width and height will be made odd for better rendering
void number_font_init(
    struct NumberFont* font,
    uint8_t brightness,
    uint8_t char_width,
    uint8_t char_height,
    uint8_t char_spacing);

// draws dashes between numbers
void number_draw_dash(struct NumberFont* font, uint32_t* led);

void number_draw_mode(
    struct NumberFont* font,
    uint32_t* led,
    uint8_t digit,
    uint8_t mode);

// draws a digit with an indexed color.  Off screen for x,y  is OK
static inline void number_draw(
    struct NumberFont* font,
    uint32_t* led,
    uint8_t digit) {
  number_draw_mode(font, led, digit, DRAW_MODE_OVERLAY);
}

// draws two digits with an indexed color
void draw_numbers(
    struct NumberFont* font,
    uint32_t* led,
    uint8_t val);

#endif
