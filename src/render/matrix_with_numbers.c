#include "matrix_with_numbers.h"
#include "matrix.h"
#include "../led_matrix.h"
#include "../number_draw.h"

#define FONT_XPAD 1
#define FONT_YPAD 1

static struct NumberFont font;

static void overlay_numbers(uint32_t* led, uint16_t time_hhmm) {
  font.x = LED_MATRIX_WIDTH - font.char_spacing * 4 - 3 - FONT_XPAD;
  font.y = font.char_height + FONT_YPAD;

  draw_numbers(&font, led, time_hhmm / 100);
  number_draw_dash(&font, led);
  draw_numbers(&font, led, time_hhmm % 100);
}

void matrix_with_numbers_render(
    uint32_t* led,
    uint32_t frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings) {
  if (frame_index == 0) {
    number_font_init(&font, brightness_step_to_brightness(settings), 5, 9, 6);
  }
  matrix_render(led, frame_index, time_hhmm, settings);
  overlay_numbers(led, time_hhmm);
}

