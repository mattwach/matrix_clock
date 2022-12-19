#include "number_cascade.h"

#include "../led_matrix.h"
#include "../number_draw.h"

#define FRAMES_PER_SCROLL 10
#define FONT FONT3X5

static int8_t hours_ypos;  // upper right of hour text

void number_cascade_render(
    uint32_t* led,
    uint32_t frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings) {
  if (frame_index == 0) {
    hours_ypos = 0;
    frame_index = 1; // don't scroll right away
  }
  const uint8_t br = brightness_step_to_brightness(settings);
  int8_t y = hours_ypos;
  uint8_t show_hours = 1;
  const uint8_t width = font_width(FONT);
  const uint8_t height = font_height(FONT);
  for (; y < LED_MATRIX_HEIGHT; y += height, show_hours = !show_hours) {
    const int8_t x = show_hours ? 0 : LED_MATRIX_WIDTH - (width * 2);
    const int8_t val = show_hours ? time_hhmm / 100 : time_hhmm % 100;
    draw_numbers(led, val, x, y, br, FONT3X5);
  }
  if ((frame_index % FRAMES_PER_SCROLL) == 0) {
    ++hours_ypos;
    if (hours_ypos > 0) {
      // need to set the ypos so that bottom
      // row of the minutes is showing
      hours_ypos = -(height * 2) + 1;
    }
  }
}
