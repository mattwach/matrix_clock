#ifndef RENDER_WAVEFORM_H
#define RENDER_WAVEFORM_H

#include <inttypes.h>
#include "../clock_settings.h"

// Turns off all LEDs
void waveform_render(
    uint32_t* led,
    uint32_t frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings);
#endif

