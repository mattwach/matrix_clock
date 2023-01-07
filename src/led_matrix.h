#ifndef LED_MATRIX_H
#define LED_MATRIX_H
// This file is the front-end abstraction for any number of
// "LED HARDWARE" modules.  The idea is that you change the .c
// file in CMakeLists.txt to correspond to the hardware you
// actually have and the link process will then bundle the
// appropriate code into the driver.

#include <inttypes.h>

// Matrix dimensions.  You may need to change these if your's
// is different as well as attend to code that might not
// like your chosen numbers.
#define LED_MATRIX_COUNT (LED_MATRIX_WIDTH * LED_MATRIX_HEIGHT)

// Initialize the matrix
void led_matrix_init();

// render the matrix.
// data is of the form 0xIIRRGGBB where
// II -> Intensity (brightness)
// RR -> Red
// GG -> Green
// BB -> Blue
//
// Values are 0-255
// format is horizontal rows.  The first pixel
// sent is the southwest corner of the matrix.
void led_matrix_render(uint32_t* data);

// converts a x,y coordinate to a pixel index
static inline uint8_t get_pixel_idx(uint8_t x, uint8_t y) {
  return (y * LED_MATRIX_WIDTH) + x;
}

// sets a pixel
static inline void set_pixel(
  uint32_t* led,  // matrix
  uint8_t x,
  int8_t y,
  uint8_t br,  // brightness
  uint8_t r,   // red
  uint8_t g,   // green
  uint8_t b    // blue
  ) {
  const uint32_t pixel = (br << 24) | (r << 16) | (g << 8) | b;
  const uint8_t idx = get_pixel_idx(x, y);
  led[idx] = pixel;
}
#endif
