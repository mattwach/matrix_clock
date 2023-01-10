#ifndef SET_TIME_LOWRES_H
#define SET_TIME_LOWRES_H

#include <inttypes.h>
#include "clock_settings.h"

// provides a common interface for rendering a clock

uint8_t set_time_low_res_render(
  uint32_t* led,
  uint8_t button_pressed,
  uint32_t frame_index,
  uint16_t time_hhmm,
  const struct ClockSettings* settings);

#endif
