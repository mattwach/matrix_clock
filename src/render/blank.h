#ifndef RENDER_BLANK_H
#define RENDER_BLANK_H

#include <inttypes.h>
#include "../clock_settings.h"

// Turns off all LEDs
void blank_render(
    uint32_t* led,
    uint32_t frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings);
#endif

