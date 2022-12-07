#include "clock_render.h"
#include "buttons.h"
#include "colors.h"
#include "led_matrix.h"
#include <stdlib.h>

#define PARTICLE_COUNT 15
// Color decay is 0-255 how much to decay color intensity per frame
#define COLOR_DECAY_PERCENT 80
#define BRIGHTNESS_DECAY_PERCENT 80

#define MIN_BRIGHTNESS 40   // 0-255
#define BRIGHTNESS_STEP_SIZE 30   // 0-255
#define BRIGHTNESS_STEPS 5   // 0-255

// maximum frames to wait before recycling a particle
#define START_DELAY_MAX 60
// How fast to move particles, based on their represented clock position
#define HOUR_DELAY 7
#define MINUTE_DELAY 3
#define SECOND_DELAY 1

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
// holds the current brightness step
uint8_t brightness_step = 2;

// inisializes a particle
static void init_particle(struct Particle* p, uint16_t time_hhmm) {
  p->x = random() % LED_MATRIX_WIDTH;
  // top of array
  p->y = LED_MATRIX_HEIGHT - 1;
  p->start_delay = random() % START_DELAY_MAX;
  p->delay_counter = 0;

  uint8_t color_idx = 0;
  switch (random() & 0x03) {
    case 0:  // hour (24h, 1s place)
      p->delay = HOUR_DELAY;
      color_idx = (time_hhmm / 100) % 10;
      break;
    case 1:  // minute (10s place)
      p->delay = MINUTE_DELAY;
      color_idx = (time_hhmm / 10) % 10;
      break;
    default: // second (1s place) 
      p->delay = SECOND_DELAY;
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
  const uint32_t pixel = (br << 24) | (r << 16) | (g << 8) | b;
  const uint8_t idx = (y * LED_MATRIX_WIDTH) + x;
  const uint32_t old_pixel = led[idx];
  uint32_t new_pixel = 0x00000000;
  // Finds the maximum for each independent channel
  for (uint32_t mask = 0xFF000000; mask; mask >>= 8) {
    if ((old_pixel & mask) >= (pixel & mask)) {
      new_pixel |= old_pixel & mask;
    } else {
      new_pixel |= pixel & mask;
    }
  }
  if (new_pixel != old_pixel) {
    led[idx] = new_pixel;
  }
}

// decays a cell according to it's current brightness
inline static uint8_t calc_brightness(uint8_t br, uint8_t color) {
  return (br < color) ? br : color;
}

// renders a particle to the led matrix (in memory only at this point).
static void render_particle(
  uint32_t* led,
  struct Particle* p,
  uint16_t time_hhmm) {
  if (p->start_delay > 0) {
    return;
  }
  const uint8_t x = p->x;
  uint16_t cbr = 255;
  uint16_t ubr = MIN_BRIGHTNESS + (BRIGHTNESS_STEP_SIZE * brightness_step);
  uint8_t something_rendered = 0;
  for (int8_t y = p->y; y < LED_MATRIX_HEIGHT; ++y) {
    if ((cbr >= 0) && (ubr > 0)) {
      const uint8_t r = calc_brightness(cbr, p->red); 
      const uint8_t g = calc_brightness(cbr, p->green); 
      const uint8_t b = calc_brightness(cbr, p->blue); 
      if (y >= 0) {
        merge_pixel(led, x, y, (uint8_t)ubr, r, g, b);
        something_rendered = 1;
      }
      // Apply the degregations more times depending on delay
      for (uint8_t j=0; j < p->delay; ++j) {
        cbr = cbr * COLOR_DECAY_PERCENT / 100;
        ubr = ubr * BRIGHTNESS_DECAY_PERCENT / 100;
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

static uint8_t check_buttons(uint8_t button_pressed) {
  if (button_pressed & INCREMENT_BUTTON) {
    ++brightness_step;
    if (brightness_step >= BRIGHTNESS_STEPS) {
      brightness_step = 0;
    }
  }
  if (button_pressed & SELECT_BUTTON) {
    return 1;
  }
  return 0;
}

// public interface.  Called to render all particles and update their state.
uint8_t clock_render(
    uint32_t* led,
    uint8_t button_pressed,
    uint32_t frame_index,
    uint16_t time_hhmm) {
  if (frame_index == 0) {
    for (uint8_t i = 0; i < PARTICLE_COUNT; ++i) {
      init_particle(particle + i, time_hhmm);
    }
  }

  for (uint8_t i=0; i < PARTICLE_COUNT; ++i) {
    render_particle(led, particle + i, time_hhmm);
    update_particle(particle + i);
  }

  return check_buttons(button_pressed);
}
