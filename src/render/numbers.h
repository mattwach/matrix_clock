#ifndef RENDER_NUMBERS_H
#define RENDER_NUMBERS_H

#include <inttypes.h>
#include "../clock_settings.h"

// Shows the "normal" led matrix but occasionally overlay's
// number representations for hours and minutes
void numbers_render(
    uint32_t* led,
    uint32_t frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings);
#endif

