#include "clock_render.h"
#include "buttons.h"
#include "colors.h"
#include "led_matrix.h"
#include "number_draw.h"
#include <stdlib.h>
#include <string.h>

#define PARTICLE_COUNT 15
// Color decay is 0-255 how much to decay color intensity per frame
#define COLOR_DECAY_PERCENT 80
#define BRIGHTNESS_DECAY_PERCENT 80

// brightness is 0-255 and changed to 0-31 in led_matrix_dotstar.c
#define MIN_BRIGHTNESS 40   // 0-255
#define BRIGHTNESS_STEP_SIZE 20   // 0-255
#define BRIGHTNESS_STEPS 10   // 0-255

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

// Display modes
#define DISPLAY_NORMAL 0
#define DISPLAY_GUIDE 1
#define DISPLAY_NUMBERS 2
#define DISPLAY_OFF 3
#define NUM_DISPLAY_MODES 4
uint8_t display_mode = 0;
// this is so the numbes always show right away in DISPLAY_NUMBERS
uint32_t frame_index_delta = 0;

// Holds state of all particles
struct Particle particle[PARTICLE_COUNT];

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

static inline uint8_t get_pixel_idx(uint8_t x, uint8_t y) {
  return (y * LED_MATRIX_WIDTH) + x;
}

static inline void set_pixel(
  uint32_t* led,
  uint8_t x,
  int8_t y,
  uint8_t br,
  uint8_t r,
  uint8_t g,
  uint8_t b) {
  const uint32_t pixel = (br << 24) | (r << 16) | (g << 8) | b;
  const uint8_t idx = get_pixel_idx(x, y);
  led[idx] = pixel;
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
  const uint8_t idx = get_pixel_idx(x, y);
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
inline static uint8_t color_brightness(uint8_t br, uint8_t color) {
  return (br < color) ? br : color;
}


// calculates a brightness value (8-bit, intended for merge_pixel(), or set_pixel())
static uint8_t calc_brightness(uint16_t brightness_step) {
  return MIN_BRIGHTNESS + (BRIGHTNESS_STEP_SIZE * brightness_step);
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

static uint8_t check_buttons(uint8_t button_pressed, uint32_t frame_index) {
  if (button_pressed & INCREMENT_BUTTON) {
    ++display_mode;
    if (display_mode >= NUM_DISPLAY_MODES) {
      display_mode = 0;
    }
    // This is done to show numbers right away in DISPLAY_NUMBERS mode
    frame_index_delta = frame_index;
  }
  if (button_pressed & SELECT_BUTTON) {
    return 1;
  }
  return 0;
}

void all_pixels_off(uint32_t* led) {
  memset(led, 0, LED_MATRIX_WIDTH * LED_MATRIX_HEIGHT * sizeof(uint32_t));
}

static void overlay_guide(uint32_t *led, uint8_t br) {
  // init two black strips
  for (uint8_t x = 0; x < 2; ++x) {
    for (uint8_t y = 0; y < LED_MATRIX_HEIGHT; ++y) {
      led[get_pixel_idx(x, y)] = 0;
    }
  }

  for (uint8_t i=0; i<10; ++i) {
    const uint32_t color = get_color(i);
    const uint8_t r = (color >> 16) & 0xFF;
    const uint8_t g = (color >> 8) & 0xFF; 
    const uint8_t b  = color & 0xFF; 
    const uint8_t x = i % 2;
    const uint8_t y = LED_MATRIX_HEIGHT - 3 - i / 2;
    set_pixel(led, x, y, br, r, g, b);
  }
}

// assume 30 FPS
// show hour for 1 second
#define OVERLAY_HOUR_FRAME 30
// show minute for 1 second
#define OVERLAY_MINUTE_FRAME (OVERLAY_HOUR_FRAME + 30)
// nothing for 6 seconds
#define OVERLAP_NOTHING_FRAME (OVERLAY_MINUTE_FRAME + 300)
static void overlay_numbers(
    uint32_t* led,
    uint16_t time_hhmm,
    uint32_t frame_index,
    uint8_t br) {
  frame_index = (frame_index - frame_index_delta) % OVERLAP_NOTHING_FRAME;
  if (frame_index < OVERLAY_HOUR_FRAME) {
    draw_numbers(led, time_hhmm / 100, br);
  } else if (frame_index < OVERLAY_MINUTE_FRAME) {
    draw_numbers(led, time_hhmm % 100, br);
  }
}

// public interface.  Called to render all particles and update their state.
uint8_t clock_render(
    uint32_t* led,
    uint8_t button_pressed,
    uint32_t frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings) {
  if (frame_index == 0) {
    frame_index_delta = 0;
    for (uint8_t i = 0; i < PARTICLE_COUNT; ++i) {
      init_particle(particle + i, time_hhmm);
    }
  }

  uint8_t brightness_step = settings->brightness;
  if (brightness_step > BRIGHTNESS_STEPS) {
    brightness_step = BRIGHTNESS_STEPS;
  } 
  uint8_t br = calc_brightness(brightness_step);

  if (display_mode == DISPLAY_OFF) {
    all_pixels_off(led);
  } else {
    for (uint8_t i=0; i < PARTICLE_COUNT; ++i) {
      render_particle(led, particle + i, time_hhmm, br);
      update_particle(particle + i);
    }
  }

  if (display_mode == DISPLAY_GUIDE) {
    overlay_guide(led, br); 
  } else if (display_mode == DISPLAY_NUMBERS) {
    overlay_numbers(led, time_hhmm, frame_index, br);
  }

  return check_buttons(button_pressed, frame_index);
}
