#ifndef RENDER_GLOW_H
#define RENDER_GLOW_H

#include <inttypes.h>
#include "../clock_settings.h"

void glow_render(
    uint32_t* led,
    uint32_t frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings);
#endif

