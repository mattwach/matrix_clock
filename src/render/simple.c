#include "simple.h"

#include "../led_matrix.h"
#include "../number_draw.h"
#include <string.h>
#include <stdlib.h>

#define FONT_WIDTH 15
#define FONT_HEIGHT 13
#define FONT_XOFFSET ((LED_MATRIX_WIDTH - FONT_WIDTH) >> 1)
#define FONT_YSPACING 16

static struct NumberFont font;

void simple_render(
    uint32_t* led,
    uint32_t frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings) {
  const uint8_t br = brightness_step_to_brightness(settings);

  if (frame_index == 0) {
    number_font_init(&font, br, FONT_WIDTH, FONT_HEIGHT, FONT_WIDTH + 1);
  }

  const uint32_t br_mask = (uint32_t)br << 24;
  for (uint8_t i=0; i<4; ++i) {
    font.x = FONT_XOFFSET;
    font.y = 1 + i * FONT_YSPACING;
    font.color = br_mask | 0x00FFFFFF; 
    number_draw_mode(&font, led, time_hhmm % 10, DRAW_MODE_COLOR);
    time_hhmm = time_hhmm / 10;
  }
}
