#ifndef LED_MATRIX_H
#define LED_MATRIX_H
// This file is the front-end abstraction for any number of
// "LED HARDWARE" modules.  The idea is that you change the .c
// file in CMakeLists.txt to correspond to the hardware you
// actually have and the link process will then bundle the
// appropriate code into the driver.

#include <inttypes.h>

#if defined(led_matrix_dotstar)
#define LED_MATRIX_WIDTH 8
#define LED_MATRIX_HEIGHT 8
#define FRAME_DELAY_MS 25  // 40FPS
#elif defined(led_matrix_32x64)
#define LED_MATRIX_WIDTH 32
#define LED_MATRIX_HEIGHT 64
#define FRAME_DELAY_MS 10  // 100FPS
#else
#error Unknown LED_MATRIX_SOURCE
#endif

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

// stop any background operations.  This is needed to
// safely write settings to flash.  Some drivers will
// not need to do anything here. Another call to
// led_matrix_render would be expected to start things
// back up.
void led_matrix_stop(void);

// converts a x,y coordinate to a pixel index
static inline uint32_t get_pixel_idx(uint8_t x, uint8_t y) {
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
