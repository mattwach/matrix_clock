#ifndef RENDER_GUIDE_H
#define RENDER_GUIDE_H

#include <inttypes.h>
#include "../clock_settings.h"

// Shows the "normal" led matrix but uses the leftmost
// two columns to show a color key.  This may be
// useful for learning to read the clock.
void guide_render(
    uint32_t* led,
    uint32_t frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings);
#endif

