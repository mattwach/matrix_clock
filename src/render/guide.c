#include "guide.h"

#include "../colors.h"
#include "../led_matrix.h"
#include "matrix.h"

void guide_render(
    uint32_t* led,
    uint32_t frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings) {
  // start with the matrix as an underlay
  matrix_render(led, frame_index, time_hhmm, settings);

  // init two black strips
  for (uint8_t x = 0; x < 2; ++x) {
    for (uint8_t y = 0; y < LED_MATRIX_HEIGHT; ++y) {
      led[get_pixel_idx(x, y)] = 0;
    }
  }

  // draw the color key table
  const uint8_t br = brightness_step_to_brightness(settings);
  for (uint8_t i=0; i<10; ++i) {
    const uint32_t color = get_color(i);
    const uint8_t r = (color >> 16) & 0xFF;
    const uint8_t g = (color >> 8) & 0xFF; 
    const uint8_t b  = color & 0xFF; 
    const uint8_t x = i % 2;
    const uint8_t y = LED_MATRIX_HEIGHT - 3 - i / 2;
    set_pixel(led, x, y, br, r, g, b);
  }
}

