#include "number_grid.h"

#include "../led_matrix.h"
#include "../number_draw.h"

void binary_grid_render(
    uint32_t* led,
    uint32_t unused_frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings) {
  const uint8_t font = BINARY_FONT4X4;
  const uint8_t br = brightness_step_to_brightness(settings);
  draw_numbers(led, time_hhmm / 100, 0, font_height(font), br, font);
  draw_numbers(led, time_hhmm % 100, 0, 0, br, font);
}
