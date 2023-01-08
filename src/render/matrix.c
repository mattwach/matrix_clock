#include "matrix.h"
#include <stdlib.h>
#include "../colors.h"
#include "../led_matrix.h"

#define PARTICLE_COUNT 128

// maximum frames to wait before recycling a particle
#define START_DELAY_MAX 60
// How fast to move particles, based on their represented clock position
#define HOUR_DELAY 5
#define MINUTE_TENS_DELAY 2
#define MINUTE_ONES_DELAY 1

// typs for init_particle
#define HOUR_ONES_TYPE 0
#define MINUTE_TENS_TYPE 1
#define MINUTE_ONES_TYPE 2
#define NUM_PARTICLE_TYPES (MINUTE_ONES_TYPE + 1)

// Color decay is 0-255 how much to decay color intensity per frame
#define COLOR_DECAY_PERCENT 80
#define BRIGHTNESS_DECAY_PERCENT 80

// Tracks current state of each active particle
struct Particle {
  uint8_t start_delay; // delay before starting
  uint8_t x;
  int8_t y;
  uint8_t delay; // frames per advancement
  uint8_t delay_counter;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

// Holds state of all particles
struct Particle particle[PARTICLE_COUNT];

// Looks at what particles are live and uses this infomation
// to randomly weight the likelyhood of a new one.
// basically trying to keep the particle count even while
// still being somewhat random
#define DISTRIBUTION_MAX_ROUNDS 32
static uint8_t even_distribution_type(void) {
  uint8_t type_mask = (1 << NUM_PARTICLE_TYPES) - 1;
  for (uint32_t i=0; i<DISTRIBUTION_MAX_ROUNDS; ++i) {
    switch (type_mask) {
      case (1 << HOUR_ONES_TYPE):
        return HOUR_ONES_TYPE;
      case (1 << MINUTE_TENS_TYPE):
        return MINUTE_TENS_TYPE;
      case (1 << MINUTE_ONES_TYPE):
        return MINUTE_ONES_TYPE;
    }

    // try and remove something from the mask
    const struct Particle* p = particle + (random() % PARTICLE_COUNT);
    switch (p->delay) {
      case HOUR_DELAY:
        type_mask &= ~(1 << HOUR_ONES_TYPE);
        break;
      case MINUTE_TENS_DELAY:
        type_mask &= ~(1 << MINUTE_TENS_TYPE);
        break;
      case MINUTE_ONES_DELAY:
        type_mask &= ~(1 << MINUTE_ONES_TYPE);
        break;
    }
  }

  // we didn't get there, fall back to pure random
  return random() % NUM_PARTICLE_TYPES;
}

// inisializes a particle
static void init_particle(struct Particle* p, uint16_t time_hhmm) {
  p->x = random() % LED_MATRIX_WIDTH;
  // top of array
  p->y = LED_MATRIX_HEIGHT - 1;
  p->start_delay = random() % START_DELAY_MAX;
  p->delay_counter = 0;

  uint8_t color_idx = 0;
  switch (even_distribution_type()) {
    case HOUR_ONES_TYPE:
      p->delay = HOUR_DELAY;
      color_idx = (time_hhmm / 100) % 10;
      break;
    case MINUTE_TENS_TYPE:  // minute (10s place)
      p->delay = MINUTE_TENS_DELAY;
      color_idx = (time_hhmm / 10) % 10;
      break;
    default: // second (1s place) 
      p->delay = MINUTE_ONES_DELAY;
      color_idx = time_hhmm % 10;
      break;
  }

  const uint32_t color = get_color(color_idx);
  p->red = (color >> 16) & 0xFF;
  p->green = (color >> 8) & 0xFF; 
  p->blue = color & 0xFF; 
}


// If to particles render to the same LED, the resolution is to take
// the maximum RGB and brightness, each maximum tracked independently. 
static void merge_pixel(
  uint32_t* led,
  uint8_t x,
  int8_t y,
  uint8_t br,
  uint8_t r,
  uint8_t g,
  uint8_t b) {
  const uint32_t new_pixel = (br << 24) | (r << 16) | (g << 8) | b;
  led[get_pixel_idx(x, y)] = new_pixel;
}


// decays a cell according to it's current brightness
inline static uint8_t color_brightness(uint8_t br, uint8_t color) {
  return (br < color) ? br : color;
}


// renders a particle to the led matrix (in memory only at this point).
static void render_particle(
  uint32_t* led,
  struct Particle* p,
  uint16_t time_hhmm,
  uint8_t ubr) {
  if (p->start_delay > 0) {
    return;
  }
  const uint8_t x = p->x;
  uint8_t cbr = 255;
  uint8_t something_rendered = 0;
  for (int8_t y = p->y; y < LED_MATRIX_HEIGHT; ++y) {
    if ((cbr >= 0) && (ubr > 0)) {
      const uint8_t r = color_brightness(cbr, p->red); 
      const uint8_t g = color_brightness(cbr, p->green); 
      const uint8_t b = color_brightness(cbr, p->blue); 
      if (y >= 0) {
        merge_pixel(led, x, y, ubr, r, g, b);
        something_rendered = 1;
      }
      // Apply the degregations more times depending on delay
      for (uint8_t j=0; j < p->delay; ++j) {
        cbr = (uint8_t)((uint16_t)cbr * COLOR_DECAY_PERCENT / 100);
        ubr = (uint8_t)((uint16_t)ubr * BRIGHTNESS_DECAY_PERCENT / 100);
      }
    }
  }

  if (!something_rendered) {
    init_particle(p, time_hhmm);
  }
}

// changes particle position and state
static void update_particle(struct Particle* p) {
  if (p->start_delay > 0) {
    p->start_delay -= 1;
    return;
  }
  p->delay_counter += 1;
  if (p->delay_counter >= p->delay) {
    p->delay_counter = 0;
    p->y -= 1;
  }
}


// Initializes particles, but only when frame_idx is zero.
static void maybe_clock_init(
  uint32_t frame_index,
  uint16_t time_hhmm,
  const struct ClockSettings* settings) {
  if (frame_index == 0) {
    for (uint8_t i = 0; i < PARTICLE_COUNT; ++i) {
      init_particle(particle + i, time_hhmm);
    }
  }
}

// public interface
void matrix_render(
    uint32_t* led,
    uint32_t frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings) {
  maybe_clock_init(frame_index, time_hhmm, settings);
  const uint8_t br = brightness_step_to_brightness(settings);

  for (uint8_t i=0; i < PARTICLE_COUNT; ++i) {
    render_particle(led, particle + i, time_hhmm, br);
    update_particle(particle + i);
  }
}
