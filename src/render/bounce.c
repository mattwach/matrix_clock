#include "bounce.h"

#include "../led_matrix.h"
#include "../number_draw.h"
#include "../colors.h"
#include <string.h>
#include <stdlib.h>

#define FONT_WIDTH 15
#define FONT_HEIGHT 13
#define FONT_XOFFSET ((LED_MATRIX_WIDTH - FONT_WIDTH) >> 1)
#define FONT_YSPACING 16

#define NUM_PARTICLES 64

// Note: >= 0x100 could end up going through a digit
#define MAX_MINUTE_ONE_VELOCITY 0x40
#define MAX_MINUTE_TEN_VELOCITY 0x20
#define MAX_HOUR_ONE_VELOCITY 0x10

// used to range the max velocity
#define MAX_VEL_MIN_PERCENT 75

struct Particle {
  // all integers below are in (points * 256)
  uint16_t x;
  uint16_t y;
  int16_t xvel;
  int16_t yvel;
};

static struct NumberFont font;
static struct Particle minute_ones[NUM_PARTICLES];
static struct Particle minute_tens[NUM_PARTICLES];
static struct Particle hour_ones[NUM_PARTICLES];

static void overlay_time(
    uint32_t* led, uint16_t time_hhmm, uint32_t br_mask) {
  for (uint8_t i=0; i<4; ++i) {
    font.x = FONT_XOFFSET;
    font.y = 1 + i * FONT_YSPACING;
    font.color = br_mask | 0x00FFFFFF; 
    number_draw_mode(&font, led, time_hhmm % 10, DRAW_MODE_COLOR);
    time_hhmm = time_hhmm / 10;
  }
}

static void init_particle(struct Particle* p, int16_t max_vel) {
  // choose some random x/y
  p->x = random() % (LED_MATRIX_WIDTH << 8);
  p->y = random() % (LED_MATRIX_HEIGHT << 8);

  // One dimension will be related to the max velocity
  int16_t axis1 = (max_vel * MAX_VEL_MIN_PERCENT) / 100 +
    (random() % (max_vel * (100 - MAX_VEL_MIN_PERCENT) / 100));
  // grab a number to split up
  const uint16_t r = random();
  // The other dimension will range to the maximum velocity
  const uint16_t r2 = r & 0xFF;
  int16_t axis2 = (max_vel / 2) + (r2 % (max_vel / 2));
  // randomly invert each axis
  if (r & (1 << 9)) {
    axis1 = -axis1;
  }
  if (r & (1 << 10)) {
    axis2 = -axis2;
  }

  // randomly assign the axes
  if (r & (1 << 11)) {
    p->xvel = axis1;
    p->yvel = axis2;
  } else {
    p->xvel = axis2;
    p->yvel = axis1;
  }
}

static uint8_t particle_is_in_open_location(
    uint32_t* led, const struct Particle* p) {
  const uint16_t x = p->x >> 8;
  if (x >= LED_MATRIX_WIDTH) {
    return 0;
  }
  const uint16_t y = p->y >> 8;
  if (y >= LED_MATRIX_HEIGHT) {
    return 0;
  }
  return led[get_pixel_idx(x, y)] == 0;
}

static void move_particle_to_open_location(
    uint32_t* led, struct Particle* p, int16_t max_vel) {
  if (particle_is_in_open_location(led, p)) {
    // the common case
    return;
  }
  uint32_t direction = random() % 8;  // starting direction
  const uint16_t orig_x = p->x;
  const uint16_t orig_y = p->y;
  while (1) {
    for (uint8_t i=0; i<8; ++i) {
      switch (direction) {
        case 0:
          p->x += 0x100;  // E
          break; 
        case 1:
          p->x += 0x100;  // SE
          p->y -= 0x100;
          break; 
        case 2:
          p->y -= 0x100;  // S
          break; 
        case 3:
          p->x -= 0x100;  // SW
          p->y -= 0x100;
          break; 
        case 4:
          p->x -= 0x100;  // W
          break; 
        case 5:
          p->x -= 0x100;  // NW
          p->y += 0x100;
          break; 
        case 6:
          p->y += 0x100;  // N
          break; 
        default:
          p->x += 0x100;  // NE
          p->y += 0x100;
          break; 
      }
      if (particle_is_in_open_location(led, p)) {
        return;
      }
      p->x = orig_x;
      p->y = orig_y;
      ++direction;
      if (direction >= 8) {
        direction = 0;
      }
    }

    // there is no direction that works
    init_particle(p, max_vel);
  }
}

static void advance_particle(
    uint32_t* led, struct Particle* p, int16_t max_vel) {
  move_particle_to_open_location(led, p, max_vel);
  p->x += p->xvel;
  p->y += p->yvel;

  if (particle_is_in_open_location(led, p)) {
    return;
  }

  // Need to bounce off something

  // try inverting x
  p->x -= p->xvel;  // undo the x change
  if (particle_is_in_open_location(led, p)) {
    p->xvel = -p->xvel;
    return;
  }

  // that didn't work, try y instead 
  p->x += p->xvel;  // redo the x change
  p->y -= p->yvel;  // undo the y change
  if (particle_is_in_open_location(led, p)) {
    p->yvel = -p->yvel;
    return;
  }

  // still didn't, inverse both  
  p->y += p->yvel;  // redo the y change
  // invert both axes
  p->xvel = -p->xvel;
  p->yvel = -p->yvel;
}

static void advance_particles(
    uint32_t* led, struct Particle* particles, int16_t max_vel) {
  for (uint16_t i=0; i<NUM_PARTICLES; ++i) {
    advance_particle(led, particles + i, max_vel);
  }
}

static void init_particles(struct Particle* particles, int16_t max_vel) {
  for (uint16_t i=0; i<NUM_PARTICLES; ++i) {
    init_particle(particles + i, max_vel);
  }
}

static void render_particles(
    uint32_t* led, struct Particle* particles, uint32_t color) {
  for (uint16_t i=0; i<NUM_PARTICLES; ++i) {
    set_pixel2(led, particles[i].x >> 8, particles[i].y >> 8, color);
  }
}

static void init(const uint8_t brightness) {
  number_font_init(
      &font, brightness, FONT_WIDTH, FONT_HEIGHT, FONT_WIDTH + 1);
  init_particles(minute_ones, MAX_MINUTE_ONE_VELOCITY);
  init_particles(minute_tens, MAX_MINUTE_TEN_VELOCITY);
  init_particles(hour_ones, MAX_HOUR_ONE_VELOCITY);
}

void bounce_render(
    uint32_t* led,
    uint32_t frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings) {
  const uint8_t br = brightness_step_to_brightness(settings);
  if (frame_index == 0) {
    init(br);
  }
  const uint32_t br_mask = (uint32_t)br << 24;
  overlay_time(led, time_hhmm, br_mask);
  advance_particles(led, minute_ones, MAX_MINUTE_ONE_VELOCITY);
  advance_particles(led, minute_tens, MAX_MINUTE_TEN_VELOCITY);
  advance_particles(led, hour_ones, MAX_HOUR_ONE_VELOCITY);

  render_particles(
      led, minute_ones, br_mask | get_color(time_hhmm % 10));
  render_particles(
      led, minute_tens, br_mask |get_color((time_hhmm / 10) % 10));
  render_particles(
      led, hour_ones, br_mask | get_color((time_hhmm / 100) % 10));
}
