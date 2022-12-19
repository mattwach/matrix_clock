#ifndef RENDER_NUMBER_GRID_H
#define RENDER_NUMBER_GRID_H

#include <inttypes.h>
#include "../clock_settings.h"

// Shows a 4x4 grid of decimal or binary numbers
void number_grid_render(
    uint32_t* led,
    uint32_t frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings);

void binary_grid_render(
    uint32_t* led,
    uint32_t frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings);
#endif

