#include "numbers.h"

#include "matrix.h"
#include "../number_draw.h"
#include "../led_matrix.h"

// assume 30 FPS
// show hour for 1 second
#define OVERLAY_HOUR_FRAME 30
// show minute for 1 second
#define OVERLAY_MINUTE_FRAME (OVERLAY_HOUR_FRAME + 30)
// nothing for 6 seconds
#define OVERLAP_NOTHING_FRAME (OVERLAY_MINUTE_FRAME + 300)

void numbers_render(
    uint32_t* led,
    uint32_t frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings) {
  // start with the matrix as an underlay
  matrix_render(led, frame_index, time_hhmm, settings);

  const uint8_t br = brightness_step_to_brightness(settings);
  frame_index = frame_index % OVERLAP_NOTHING_FRAME;
  if (frame_index < OVERLAY_HOUR_FRAME) {
    draw_numbers(led, time_hhmm / 100, br);
  } else if (frame_index < OVERLAY_MINUTE_FRAME) {
    draw_numbers(led, time_hhmm % 100, br);
  }
}

