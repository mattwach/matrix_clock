#ifndef RENDER_MATRIX_H
#define RENDER_MATRIX_H

#include <inttypes.h>
#include "../clock_settings.h"

// Renders the "falling point" matrix-style effect
// where particle speed is the time digit e.g.
// 13:45 ->
//
// The 1 is dropped.
// The 3 is represented by slow points
// The 4 is rpresented by medium-speed points
// The 5 is represented by fast points
//
// The digits themselves (0-9) are represented
// by colors as-defined in colors.c.  Under current
// definitions this would represent 13:45 as
// 1 -> dropped
// 3 -> yellow (slow)
// 4 -> green (medium speed)
// 5 -> blue (fast)
void matrix_render(
    uint32_t* led,
    uint32_t frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings);
#endif

