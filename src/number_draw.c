#include "number_draw.h"
#include "colors.h"
#include "led_matrix.h"
#include <pico/platform.h>

struct FontData {
  uint8_t width;
  uint8_t height;
  const char* data;
};

// Here I am using string to represent the number pixels.
// using a bit array is more space-efficient but harder to read
// and we are not anywhere close to using the Picos flash at this point
const char font3x7[] __in_flash() =
"###"
"# #"
"# #"
"# #"
"# #"
"# #"
"###"

" # "
" # "
" # "
" # "
" # "
" # "
" # "

"###"
"  #"
"  #"
"###"
"#  "
"#  "
"###"

"###"
"  #"
"  #"
"###"
"  #"
"  #"
"###"

"# #"
"# #"
"# #"
"###"
"  #"
"  #"
"  #"

"###"
"#  "
"#  "
"###"
"  #"
"  #"
"###"

"###"
"#  "
"#  "
"###"
"# #"
"# #"
"###"

"###"
"  #"
"  #"
"  #"
"  #"
"  #"
"  #"

"###"
"# #"
"# #"
"###"
"# #"
"# #"
"###"

"###"
"# #"
"# #"
"###"
"  #"
"  #"
"###";

const char font3x5[] __in_flash() =
"###"
"# #"
"# #"
"# #"
"###"

" # "
" # "
" # "
" # "
" # "

"###"
"  #"
"###"
"#  "
"###"

"###"
"  #"
"###"
"  #"
"###"

"# #"
"# #"
"###"
"  #"
"  #"

"###"
"#  "
"###"
"  #"
"###"

"###"
"#  "
"###"
"# #"
"###"

"###"
"  #"
"  #"
"  #"
"  #"

"###"
"# #"
"###"
"# #"
"###"

"###"
"# #"
"###"
"  #"
"###";

const char font4x4[] __in_flash() =
"####"
"#  #"
"#  #"
"####"

"  # "
"  # "
"  # "
"  # "

"####"
"  ##"
"##  "
"####"

"####"
" ###"
"   #"
"####"

"#  #"
"#  #"
"####"
"   #"

"####"
"##  "
"  ##"
"####"

"####"
"#   "
"####"
"####"

"####"
"   #"
"   #"
"   #"

" ## "
"####"
" ## "
"####"

"####"
"####"
"   #"
"####";

const char binary4x4[] __in_flash() =
"####"
"    "
"    "
"####"

"### "
"   #"
"   #"
"### "

"## #"
"  # "
"  # "
"## #"

"##  "
"  ##"
"  ##"
"##  "

"# ##"
" #  "
" #  "
"# ##"

"# # "
" # #"
" # #"
"# # "

"#  #"
" ## "
" ## "
"#  #"

"#   "
" ###"
" ###"
"#   "

" ###"
"#   "
"#   "
" ###"

" ## "
"#  #"
"#  #"
" ## ";


static struct FontData fonts[] = {
  {3, 7, font3x7},
  {3, 5, font3x5},
  {4, 4, font4x4},
  {4, 4, binary4x4},
};


// draws a single pixel to the set matrix, if 
static void pixel(
  uint32_t* led,
  int8_t x,
  int8_t y,
  uint32_t color_with_brightness
) {
  if ((x < 0) || (x >= LED_MATRIX_WIDTH)) {
    return;
  }
  if ((y < 0) || (y >= LED_MATRIX_HEIGHT)) {
    return;
  }
  led[get_pixel_idx(x, y)] = color_with_brightness;
}

// Draws a number 0-9 at the specified cordinates
void number_draw(
    uint32_t* led,
    uint8_t digit,
    int8_t x,
    int8_t y,
    uint8_t brightness,
    uint8_t font) {
  if (digit > 9) {
    return;
  }
  const struct FontData* font_data = fonts + font;
  const uint32_t color = ((uint32_t)brightness << 24) | get_color(digit);
  const char* data = font_data->data + (font_data->width * font_data->height * digit);
  for (int8_t yd = 0; yd < font_data->height; ++yd) {
    for (int8_t xd = 0; xd < font_data->width; ++xd) {
      // need to scan from the bottom of the font becuase the matrix
      // defines Y from bottom up.
      if (data[((font_data->height - 1) - yd) * font_data->width + xd] != ' ') {
        pixel(led, x + xd, y + yd, color);
      }
    }
  }
}

// Draws val to led memory.  val must be 0-99.
void draw_numbers(
    uint32_t* led,
    uint8_t val,
    int8_t x,
    int8_t y,
    int8_t brightness,
    uint8_t font) {
  const uint8_t tens = val / 10;
  number_draw(led, tens, x, y, brightness, font);
  const uint8_t ones = val % 10; 
  number_draw(led, ones, x + fonts[font].width, y, brightness, font);
}

uint8_t font_width(uint8_t font) {
  return fonts[font].width;
}

uint8_t font_height(uint8_t font) {
  return fonts[font].height;
}

