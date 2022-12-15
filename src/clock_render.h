#ifndef CLOCK_RENDER_H
#define CLOCK_RENDER_H

#include <inttypes.h>
#include "clock_settings.h"

// provides a common interface for rendering a clock
uint8_t clock_render(
  uint32_t* led,
  uint8_t button_pressed,
  uint32_t frame_index,
  uint16_t time_hhmm,
  const struct ClockSettings* settings);

// returns the number of available display modes
uint8_t clock_render_num_display_modes(void);
// maps a display mode index to a name.
const char* clock_render_display_mode_name(uint8_t mode);

#endif
