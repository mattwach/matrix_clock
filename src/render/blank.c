#include "blank.h"

#include "../led_matrix.h"
#include <string.h>

void blank_render(
    uint32_t* led,
    uint32_t unused_frame_index,
    uint16_t unused_time_hhmm,
    const struct ClockSettings* unused_settings) {
  memset(led, 0, LED_MATRIX_WIDTH * LED_MATRIX_HEIGHT * sizeof(uint32_t));
}
