#ifndef RENDER_FADE_H
#define RENDER_FADE_H

#include <inttypes.h>
#include "../clock_settings.h"

// Turns off all LEDs
void fade_render(
    uint32_t* led,
    uint32_t frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings);
#endif

