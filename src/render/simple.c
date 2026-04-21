#include "simple.h"

#include "../led_matrix.h"
#include "../number_draw.h"
#include <string.h>
#include <stdlib.h>

#define FONT_WIDTH 15
#define FONT_HEIGHT 13
#define FONT_XOFFSET ((LED_MATRIX_WIDTH - FONT_WIDTH) >> 1)
#define FONT_YSPACING 16

static struct NumberFont font;

struct Temperature {
    uint16_t timehhmm;
    uint32_t rgb;
};

// color to use when outside of defined table
static const uint32_t evening_color = 0x0000FF;

// a sample of times of day and color temperature
static struct Temperature time_colors[] = {
  {600, evening_color},
  {700, 0xFF0D0D},
  {710, 0xFF2C2C},
  {720, 0xFF4646},
  {730, 0xFF5B5B},
  {740, 0xFF6D6D},
  {750, 0xFF7E7E},
  {800, 0xFF8C8C},
  {830, 0xFFAFAF},
  {900, 0xFFC6C6},
  {1000, 0xFFDBDB},
  {1100, 0xFFE1E1},
  {1200, 0xFFE7E7},
  {1330, 0xFFE7E7},
  {1500, 0xFFDBDB},
  {1630, 0xFFC6C6},
  {1800, 0xFFA6A6},
  {1900, 0xFF7B7B},
  {1910, 0xFF6D6D},
  {1920, 0xFF5F5F},
  {1930, 0xFF4F4F},
  {1940, 0xFF3C3C},
  {1950, 0xFF2727},
  {2000, 0xFF0D0D},
  {2100, evening_color},
};

#define NUM_TIME_COLORS (sizeof(time_colors) / sizeof(time_colors[0]))

// shifts a color channel between c1 and c2 based on the delta (time - c1_time)
// and span (c2_time - c1_time).  Times close to c1 time will be similar to
// c1 and visa versa.
static inline uint32_t interpolate_color_channnel(
    uint32_t c1, uint32_t c2, int shift, int16_t delta, int16_t span) {
    const int16_t c1_base = (int16_t)((c1 >> shift) & 0xFF);
    const int16_t c1_new =
      c1_base + (((int16_t)((c2 >> shift) & 0xFF) - c1_base) * delta / span);
    return (uint32_t)c1_new << shift;
}

// calculates a difference between two hhmm times in minutes.
static int16_t minute_delta(uint16_t t2, uint16_t t1) {
    int16_t m2 = (int16_t)((t2 / 100) * 60 + (t2 % 100));
    int16_t m1 = (int16_t)((t1 / 100) * 60 + (t1 % 100));
    return m2-m1;
}

// Determines an interpolated color between two color entries in time_colors
static uint32_t interpolate_color(uint16_t timehhmm, int i) {
    const uint32_t c1 = time_colors[i].rgb;
    const uint32_t c2 = time_colors[i+1].rgb;
    const int16_t delta = minute_delta(timehhmm, time_colors[i].timehhmm);
    const int16_t span = minute_delta(time_colors[i+1].timehhmm, time_colors[i].timehhmm);
    const uint32_t rdelta = interpolate_color_channnel(c1, c2, 16, delta, span);
    const uint32_t gdelta = interpolate_color_channnel(c1, c2, 8, delta, span);
    const uint32_t bdelta = interpolate_color_channnel(c1, c2, 0, delta, span);
    return rdelta | gdelta | bdelta;
}

// searches time_colors for the two table rows that the current time
// falls between.  If the time is < time_colors[0] or off the
// end of the array, evening_color is used.
static uint32_t get_color(uint16_t timehhmm) {
    for (int i=0; i<(NUM_TIME_COLORS-1); ++i) {
        if ((timehhmm >= time_colors[i].timehhmm) &&
            (timehhmm < time_colors[i+1].timehhmm)) {
            return interpolate_color(timehhmm, i);
        }
    }

    return evening_color;
}

// Entry point for the module.
void simple_render(
    uint32_t* led,
    uint32_t frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings) {
  const uint8_t br = brightness_step_to_brightness(settings);

  if (frame_index == 0) {
    number_font_init(&font, br, FONT_WIDTH, FONT_HEIGHT, FONT_WIDTH + 1);
  }

  const uint32_t br_mask = (uint32_t)br << 24;
  font.color = br_mask | get_color(time_hhmm); 
  for (uint8_t i=0; i<4; ++i) {
    font.x = FONT_XOFFSET;
    font.y = 1 + i * FONT_YSPACING;
    number_draw_mode(&font, led, time_hhmm % 10, DRAW_MODE_COLOR);
    time_hhmm = time_hhmm / 10;
  }
}
