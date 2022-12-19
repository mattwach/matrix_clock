#ifndef RENDER_NUMBER_CASCADE_H
#define RENDER_NUMBER_CASCADE_H

#include <inttypes.h>
#include "../clock_settings.h"

// Cascades time numbers
void number_cascade_render(
    uint32_t* led,
    uint32_t frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings);
#endif

