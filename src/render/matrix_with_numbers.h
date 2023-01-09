#ifndef RENDER_MATRIX_WITH_NUMBERS_H
#define RENDER_MATRIX_WITH_NUMBERS_H

#include <inttypes.h>
#include "../clock_settings.h"

// calls matrix and overlays numbers
void matrix_with_numbers_render(
    uint32_t* led,
    uint32_t frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings);
#endif

