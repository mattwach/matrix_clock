#include "waveform.h"

#include "../led_matrix.h"
#include "../number_draw.h"
#include "../colors.h"
#include <string.h>
#include <stdlib.h>
#include "pico/platform.h"

#include "distances.inc"

#define FONT_WIDTH 15
#define FONT_HEIGHT 13
#define FONT_XOFFSET ((LED_MATRIX_WIDTH - FONT_WIDTH) >> 1)
#define FONT_YSPACING 16

#define NUM_PARTICLES 16
#define MAX_WAIT_FRAMES 200

struct Particle {
  uint8_t x;
  uint8_t y;
  // negative frames are waiting to become active.  Positive
  // frames represent a distance, possibly scaled (to slow down
  // the animation).
  int16_t frame; 
  uint32_t color;
};

struct NumberFont font;
struct Particle particles[NUM_PARTICLES];

static void overlay_time(uint32_t* led, uint16_t time_hhmm) {
  for (uint8_t i=0; i<4; ++i) {
    font.x = FONT_XOFFSET;
    font.y = 1 + i * FONT_YSPACING;
    number_draw_mode(&font, led, time_hhmm % 10, DRAW_MODE_NUMBER);
    time_hhmm = time_hhmm / 10;
  }
}

static void reset_particle(struct Particle* p) {
  p->frame = -1 - (int16_t)(random() % MAX_WAIT_FRAMES);
}

static uint8_t position_already_taken(struct Particle* p) {
  for (uint16_t i=0; i<NUM_PARTICLES; ++i) {
    const struct Particle* cp = particles + i;
    if ((cp->frame > 0) && (cp->x == p->x) && (cp->y == p->y)) {
      return 1;
    }
  }

  return 0;
}

static void activate_particle(const uint32_t* led, struct Particle* p) {
  // Star by setting p to some random location
  const uint16_t r = random();
  p->x = FONT_XOFFSET + (r & 0xFF) % FONT_WIDTH;
  p->y = (r >> 8) % LED_MATRIX_HEIGHT;

  // set a scanning direction
  const int8_t dir = p->x > (LED_MATRIX_WIDTH / 2) ? -1 : 1;
  while (1) {
    uint32_t color = led[get_pixel_idx(p->x, p->y)] & 0x00FFFFFF;
    if (color) {
      if (position_already_taken(p)) {
        reset_particle(p);
      } else {
        p->color = color;
      }
      return;
    }
    // bounds checks
    p->x += dir;
    if (p->x < FONT_XOFFSET) {
      return;
    }
    if (p->x > (FONT_XOFFSET + FONT_WIDTH)) {
      return;
    }
  }
}

static void advance_particle(const uint32_t* led, struct Particle* p) {
  ++p->frame;
  if (p->frame < 0) {
  } else if (p->frame == 0) {
    activate_particle(led, p);
  } else if (p->frame >= NUM_DISTANCES) {
    reset_particle(p);
  }
  // else do nothing
}

static void advance_particles(const uint32_t* led) {
  for (uint16_t i=0; i<NUM_PARTICLES; ++i) {
    advance_particle(led, particles + i);
  }
}

static uint32_t calc_particle_color(const struct Particle* p) {
  const uint32_t color = p->color;
  const uint32_t weight = distance_weights[p->frame];
  uint32_t r = (color >> 16) * weight / 0xFF;
  uint32_t g = ((color >> 8) & 0xFF) * weight / 0xFF; 
  uint32_t b = (color & 0xFF) * weight / 0xFF; 
  return (r << 16) | (g << 8) | b;
}

static void try_merge_point(uint32_t* led, int8_t x, int8_t y, uint32_t color) {
  if (x < 0) {
    return;
  }
  if (y < 0) {
    return;
  }
  if (x >= LED_MATRIX_WIDTH) {
    return;
  }
  if (y >= LED_MATRIX_HEIGHT) {
    return;
  }

  const uint16_t idx = get_pixel_idx(x, y);
  const uint32_t existing_color = led[idx];
  if (existing_color == 0) {
    led[idx] = color;
    return;
  }

  // need to add channels to the found channels
  const uint32_t br_mask = color & 0x00FFFFFF;
  uint32_t r = ((existing_color >> 16) & 0xFF) + ((color >> 16) & 0xFF);
  uint32_t g = ((existing_color >> 8) & 0xFF) + ((color >> 8) & 0xFF);
  uint32_t b = (existing_color & 0xFF) + (color & 0xFF);
  if (r >= 0x100) {
    r = 0xFF;
  }
  if (g >= 0x100) {
    g = 0xFF;
  }
  if (b >= 0x100) {
    b = 0xFF;
  }

  led[idx] = br_mask | (r << 16) | (g << 8) | b;
}

static void overlay_active_particle(uint32_t* led, const struct Particle* p, uint32_t br_mask) {
  const uint32_t color = br_mask | calc_particle_color(p);
  const uint16_t distance_offset_end = distance_offsets[p->frame + 1];
  for (uint16_t i = distance_offsets[p->frame]; i < distance_offset_end; ++i) {
    const uint16_t xyd = distance_points[i];
    const int8_t xd = xyd >> 8;
    const int8_t yd = xyd & 0xFF;
    const int8_t xc = p->x;
    const int8_t yc = p->y;
    // distance_point is only a 90 degree arc, thus we need to mirror it on
    // both x and y
    try_merge_point(led, xc + xd, yc + yd, color);
    try_merge_point(led, xc - xd, yc + yd, color);
    try_merge_point(led, xc - xd, yc - yd, color);
    try_merge_point(led, xc + xd, yc - yd, color);
  } 
}

static void overlay_active_particles(uint32_t* led, uint32_t br_mask) {
  for (uint16_t i=0; i<NUM_PARTICLES; ++i) {
    const struct Particle* p = particles + i;
    if (p->frame > 0) {
      overlay_active_particle(led, p, br_mask);
    }
  }
}

static void init_particles(void) {
  for (uint16_t i=0; i<NUM_PARTICLES; ++i) {
    reset_particle(particles + i);
  }
}

static void init(const uint8_t brightness) {
  number_font_init(&font, brightness, FONT_WIDTH, FONT_HEIGHT, FONT_WIDTH + 1);
  init_particles();
}

void glow_render(
    uint32_t* led,
    uint32_t frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings) {
  const uint8_t br = brightness_step_to_brightness(settings);
  if (frame_index == 0) {
    init(br);
  }
  overlay_time(led, time_hhmm);
  advance_particles(led);
  overlay_active_particles(led, br << 24);
}

