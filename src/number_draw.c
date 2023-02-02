#include "number_draw.h"
#include "colors.h"
#include "led_matrix.h"
#include <pico/platform.h>

// numbers are made of lines
// Each line is on a coordinate system from 0x00 to 0x10
// end with all -1
//
// Note: 0,0 is lower, left
struct Line {
  int8_t x0;
  int8_t y0;
  int8_t x1;
  int8_t y1;
};

struct Line zero[] = {
  {0, 0, 16, 0},
  {0, 0, 0, 16},
  {0, 16, 16, 16},
  {16, 0, 16, 16},
  {-1, -1, -1, -1},
};

struct Line one[] = {
  {8, 0, 8, 16},
  {0, 0, 16, 0},
  {0, 16, 8, 16},
  {-1, -1, -1, -1},
};

struct Line two[] = {
  {0, 0, 16, 0},
  {0, 0, 0, 8},
  {0, 8, 16, 8},
  {16, 8, 16, 16},
  {0, 16, 16, 16},
  {-1, -1, -1, -1},
};


struct Line three[] = {
  {0, 0, 16, 0},
  {0, 8, 16, 8},
  {0, 16, 16, 16},
  {16, 0, 16, 16},
  {-1, -1, -1, -1},
};

struct Line four[] = {
  {16, 0, 16, 16},
  {0, 8, 0, 16},
  {0, 8, 16, 8},
  {-1, -1, -1, -1},
};

struct Line five[] = {
  {0, 0, 16, 0},
  {16, 0, 16, 8},
  {0, 8, 16, 8},
  {0, 8, 0, 16},
  {0, 16, 16, 16},
  {-1, -1, -1, -1},
};

struct Line six[] = {
  {0, 0, 16, 0},
  {0, 0, 0, 16},
  {0, 8, 16, 8},
  {0, 16, 16, 16},
  {16, 0, 16, 8},
  {-1, -1, -1, -1},
};

struct Line seven[] = {
  {0, 16, 16, 16},
  {16, 0, 16, 16},
  {-1, -1, -1, -1},
};

struct Line eight[] = {
  {0, 0, 16, 0},
  {0, 8, 16, 8},
  {0, 16, 16, 16},
  {0, 0, 0, 16},
  {16, 0, 16, 16},
  {-1, -1, -1, -1},
};

struct Line nine[] = {
  {0, 16, 16, 16},
  {0, 8, 0, 16},
  {0, 8, 16, 8},
  {16, 0, 16, 16},
  {0, 0, 16, 0},
  {-1, -1, -1, -1},
};

struct Line* font_lines[] = {
  zero,
  one,
  two,
  three,
  four,
  five,
  six,
  seven,
  eight,
  nine,
};

// draws a single pixel to the set matrix
static void pixel(
  uint32_t* led,
  int16_t x,
  int16_t y,
  uint32_t color_with_brightness,
  uint8_t mode) {
  if ((x < 0) || (x >= LED_MATRIX_WIDTH)) {
    return;
  }
  if ((y < 0) || (y >= LED_MATRIX_HEIGHT)) {
    return;
  }
  uint16_t pixel_idx = get_pixel_idx(x, y);
  switch (mode) {
    case DRAW_MODE_OVERLAY:
      led[pixel_idx] = color_with_brightness;
      break;
    case DRAW_MODE_WHITE:
      led[pixel_idx] = color_with_brightness | 0xFFFFFF;
      break;
  }
}

void number_font_init(
    struct NumberFont* font,
    uint8_t brightness,
    uint8_t char_width,
    uint8_t char_height,
    uint8_t char_spacing) {
  if (char_width & 1) {
    --char_width;
  }
  if (char_height & 1) {
    --char_height;
  }
  font->x = 0;
  font->y = 0;
  font->brightness = brightness;
  font->char_width = char_width;
  font->char_height = char_height;
  font->char_spacing = char_spacing;
}

static inline void draw_line(
    struct NumberFont* font,
    uint32_t* led,
    const struct Line* line,
    uint32_t color,
    uint8_t mode) {
  // scale from the 0-16 scale to the provided width/height values
  const int16_t x0 = font->x + ((line->x0 * font->char_width) >> 4);
  const int16_t y0 = font->y + ((line->y0 * font->char_height) >> 4);
  const int16_t x1 = font->x + ((line->x1 * font->char_width) >> 4);
  const int16_t y1 = font->y + ((line->y1 * font->char_height) >> 4);

  const int8_t xdir = x1 > x0 ? 1 : -1;
  const int8_t ydir = y1 > y0 ? 1 : -1;

  const int16_t dx = (x1 - x0) * xdir;
  const int16_t dy = (y1 - y0) * ydir;

  int16_t D = 2 * dy - dx;
  int16_t x = x0;
  int16_t y = y0;

  pixel(led, x, y, color, mode);

  while (x != x1 || y != y1) {
    if (D > 0) {
      y += ydir;
      D -= 2 * dx;
    } 
    
    if (D <= 0) {
      x += xdir;
      D += 2 * dy;
    }

    pixel(led, x, y, color, mode);
  }
}

// Draws a number 0-9 at the specified cordinates
void number_draw_mode(
    struct NumberFont* font,
    uint32_t* led,
    uint8_t digit,
    uint8_t mode) {
  if (digit > 9) {
    return;
  }
  const struct Line* lines = font_lines[digit];
  const uint32_t color = ((uint32_t)font->brightness << 24) | get_color(digit);

  for (; lines[0].x0 >= 0; ++lines) {
    draw_line(font, led, lines, color, mode);
  }
  font->x += font->char_spacing;
}

void number_draw_dash(struct NumberFont* font, uint32_t* led) {
  const uint32_t color = ((uint32_t)font->brightness << 24) | 0xFFFFFF; // white
  const struct Line dash1 = {0, 4, 0, 5};
  draw_line(font, led, &dash1, color, DRAW_MODE_OVERLAY);
  const struct Line dash2 = {0, 12, 0, 13};
  draw_line(font, led, &dash2, color, DRAW_MODE_OVERLAY);
  font->x += 2;
}

// Draws val to led memory.  val must be 0-99.
void draw_numbers(
    struct NumberFont* font,
    uint32_t* led,
    uint8_t val) {
  const uint8_t tens = val / 10;
  number_draw(font, led, tens);
  const uint8_t ones = val % 10; 
  number_draw(font, led, ones);
}

