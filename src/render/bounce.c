#include "bounce.h"

#include "../led_matrix.h"
#include "../number_draw.h"
#include "../colors.h"
#include <string.h>
#include <stdlib.h>

#define FONT_WIDTH 15
#define FONT_HEIGHT 13
#define FONT_XOFFSET ((LED_MATRIX_WIDTH - FONT_WIDTH) >> 1)
#define FONT_YSPACING 16

static struct NumberFont font;

static inline int16_t rand_range(int16_t min, int16_t max) {
  return min + (random() % (max - min));
}

static void overlay_time(uint32_t* led, uint16_t time_hhmm) {
  for (uint8_t i=0; i<4; ++i) {
    font.x = FONT_XOFFSET;
    font.y = 1 + i * FONT_YSPACING;
    number_draw_mode(&font, led, time_hhmm % 10, DRAW_MODE_WHITE);
    time_hhmm = time_hhmm / 10;
  }
}

static void init(const uint8_t brightness) {
  number_font_init(&font, brightness, FONT_WIDTH, FONT_HEIGHT, FONT_WIDTH + 1);
}

void bounce_render(
    uint32_t* led,
    uint32_t frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings) {
  const uint8_t br = brightness_step_to_brightness(settings);
  if (frame_index == 0) {
    init(br);
  }
  overlay_time(led, time_hhmm);
}
