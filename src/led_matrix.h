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
#define LED_MATRIX_WIDTH 8
#define LED_MATRIX_HEIGHT 8
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

#endif
