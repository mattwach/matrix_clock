#include "waveform.h"

#include "../led_matrix.h"
#include "../number_draw.h"
#include <string.h>
#include <stdlib.h>

#define MIN_DIRECTION_WEIGHT 0xA0
#define MAX_DIRECTION_WEIGHT 0xFF
#define MAX_DELTA 4
#define WAVEFORM_COUNT 1

#define FONT_WIDTH 15
#define FONT_HEIGHT 15
#define FONT_XOFFSET ((LED_MATRIX_WIDTH - FONT_WIDTH) >> 1)
#define FONT_YSPACING 16

struct Waveform {
  // 0xIIRRGGBB
  uint32_t color;
  int8_t dir;
  uint8_t dir_weight;
  // an xpos for each given ypos.  -1 means "nothing"
  // ypos is the (index + frame_index) % LED_MATRIX_HEIGHT
  int8_t xpos[LED_MATRIX_HEIGHT];
};

static struct Waveform waves[WAVEFORM_COUNT];
static struct NumberFont font;

static inline int16_t rand_range(int16_t min, int16_t max) {
  return min + (random() % (max - min));
}

static inline void reverse_wave(struct Waveform* w) {
  w->dir = 1 - w->dir;
  w->dir_weight = rand_range(MIN_DIRECTION_WEIGHT, MAX_DIRECTION_WEIGHT);
}

static void add_wave_point(struct Waveform* w, int8_t head_idx) {
  const int8_t prev_head_idx = (head_idx - 1) % LED_MATRIX_HEIGHT;
  const int8_t prev_x = w->xpos[prev_head_idx];
  if (prev_x < 0) {
    // just initialized (or bug)
    w->xpos[head_idx] = rand_range(0, LED_MATRIX_WIDTH);
    return;
  }
  // The logic below chooses an xdelta and direction from the
  // previous x position.  It leans toward using w->dir (-1 or 1)
  // as a direction, weighted by 0-MAX_DELTA.  The chances
  // of this happening are w->dir_weight / 255.  Otherwise
  // the wave will proceed in the oppossite direction to
  // create a chaotic feel to the rendering.
  uint16_t r = random();
  // -1 means reverse
  const int8_t reverse = (r & 0xFF) < w->dir_weight ? 1 : -1; 
  r >>= 8;
  const int8_t delta = reverse * w->dir * (int8_t)(r % MAX_DELTA);
  int8_t new_x = prev_x + delta;
  if ((new_x < 0) || (new_x >= LED_MATRIX_WIDTH)) {
    new_x = prev_x - delta;  // go the other way instead
    reverse_wave(w);
  }
  w->xpos[head_idx] = new_x;
}

static void add_wave_points(int8_t head_idx) {
  for (uint8_t i=0; i<WAVEFORM_COUNT; ++i) {
    add_wave_point(waves + i, head_idx);
  }
}

static void render_wave(uint32_t* led, struct Waveform* w, uint8_t head_idx) {
  for (uint16_t y=0; y<LED_MATRIX_HEIGHT; ++y) {
    const int8_t x = w->xpos[head_idx];
    if (x >= 0) {
      set_pixel2(led, (uint16_t)x, y, w->color); 
    }
    ++head_idx;
    if (head_idx >= LED_MATRIX_HEIGHT) {
      head_idx = 0;
    }
  }
}

static void render_waves(uint32_t* led, uint8_t head_idx) {
  for (uint8_t i=0; i<WAVEFORM_COUNT; ++i) {
    render_wave(led, waves + i, head_idx);
  }
}

static void overlay_time(uint32_t* led, uint16_t time_hhmm) {
  for (uint8_t i=0; i<4; ++i) {
    font.x = FONT_XOFFSET;
    font.y = i * FONT_YSPACING;
    number_draw_mode(&font, led, time_hhmm % 10, DRAW_MODE_INTERSECT);
    time_hhmm = time_hhmm / 10;
  }
}

static void init_wave(struct Waveform* w, uint8_t brightness) {
  w->color = ((uint32_t)brightness << 24) | 0x00402080;
  w->dir_weight = rand_range(MIN_DIRECTION_WEIGHT, MAX_DIRECTION_WEIGHT);
  w->dir = random() & 1 ? -1 : 1;
  for (uint8_t i = 0; i < LED_MATRIX_HEIGHT; ++i) {
    w->xpos[i] = -1;
  }
}

static void init(const uint8_t brightness) {
  number_font_init(&font, brightness, FONT_WIDTH, FONT_HEIGHT, FONT_WIDTH + 1);
  for (uint8_t i=0; i<WAVEFORM_COUNT; ++i) {
    init_wave(waves + i, brightness);
  }
}

void waveform_render(
    uint32_t* led,
    uint32_t frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings) {
  if (frame_index == 0) {
    const uint8_t br = brightness_step_to_brightness(settings);
    init(br);
  }
  const int8_t head_idx = (0 - (int32_t)frame_index) % LED_MATRIX_HEIGHT;
  add_wave_points(head_idx);
  render_waves(led, head_idx);
  overlay_time(led, time_hhmm);
}