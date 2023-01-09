#include "fade.h"

#include "../clock_settings.h"
#include "../led_matrix.h"
#include "../number_draw.h"
#include <string.h>
#include <stdlib.h>

// Note, this code assumes 256 LED max.  If oyu have more,
// you'll need to change data types or simply disable this module.

// Contains the order in which the fade should happen
// each value is an offset into the matrix.  The array order determines
// the order.
static uint16_t fade_order[LED_MATRIX_COUNT];

// we need to hold our data becuase it is added to each frame
static uint32_t data[LED_MATRIX_COUNT];
static uint32_t target[LED_MATRIX_COUNT];

// update these at the start to avoid odd changing mid animation
static uint8_t minutes;

static struct NumberFont font;

// how fast the fade occurs
#define FRAMES_PER_FADE_PIXEL 1

// we take a frame_index modulus and compare to the following
// thresholds to determine what the render should be doing
#define BUILD_HOUR_DIGITS (FRAMES_PER_FADE_PIXEL * LED_MATRIX_COUNT)
#define SHOW_HOUR (BUILD_HOUR_DIGITS + 50)
#define BUILD_MINUTE_DIGITS (SHOW_HOUR + FRAMES_PER_FADE_PIXEL * LED_MATRIX_COUNT)
#define SHOW_MINUTE (BUILD_MINUTE_DIGITS + 50)
#define BLANK_OUT (SHOW_MINUTE + FRAMES_PER_FADE_PIXEL * LED_MATRIX_COUNT)
#define SHOW_BLANK (BLANK_OUT + 50)
#define STATE_MODULUS SHOW_BLANK

static void swap_with_random_fade_slot(uint16_t i) {
  const uint16_t j = (uint8_t)(random() % LED_MATRIX_COUNT);
  const uint16_t tmp = fade_order[i];
  fade_order[i] = fade_order[j];
  fade_order[j] = tmp;
}

static void shuffle_fade_order(void) {
  for (uint16_t i=0; i < LED_MATRIX_COUNT; ++i) {
    swap_with_random_fade_slot(i);
  }
}

static void init(uint8_t brightness) {
  memset(data, 0, sizeof(data));
  for (uint16_t i=0; i<LED_MATRIX_COUNT; ++i) {
    fade_order[i] = i;
  }
  number_font_init(
      &font,
      brightness,
      (LED_MATRIX_WIDTH / 2) - 1,
      LED_MATRIX_HEIGHT - 1,
      LED_MATRIX_WIDTH / 2);
}

static void hour_digits(uint32_t* led, uint32_t fade_index, uint16_t time_hhmm) {
  if (fade_index == 0) {
    shuffle_fade_order();
    memset(target, 0, sizeof(target));
    minutes = time_hhmm % 100;
    font.x = 0;
    font.y = 0;
    draw_numbers(&font, target, time_hhmm / 100);
  }
  const uint16_t led_index = fade_order[fade_index / FRAMES_PER_FADE_PIXEL];
  data[led_index] = target[led_index];
}

static void minute_digits(uint32_t* led, uint32_t fade_index) {
  if (fade_index == 0) {
    shuffle_fade_order();
    memset(target, 0, sizeof(target));
    font.x = 0;
    font.y = 0;
    draw_numbers(&font, target, minutes);
  }
  const uint16_t led_index = fade_order[fade_index / FRAMES_PER_FADE_PIXEL];
  data[led_index] = target[led_index];
}

static void blank_out(uint32_t* led, uint32_t fade_index) {
  if (fade_index == 0) {
    shuffle_fade_order();
    memset(target, 0, sizeof(target));
  }
  const uint16_t led_index = fade_order[fade_index / FRAMES_PER_FADE_PIXEL];
  data[led_index] = target[led_index];
}

void fade_render(
    uint32_t* led,
    uint32_t frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings) {
  if (frame_index == 0) {
    init(brightness_step_to_brightness(settings));
  }
  const uint32_t state = frame_index % STATE_MODULUS;
  if (state < BUILD_HOUR_DIGITS) {
    hour_digits(led, state, time_hhmm);
  } else if (state < SHOW_HOUR) {
    // do nothing
  } else if (state < BUILD_MINUTE_DIGITS) {
    minute_digits(led, state - SHOW_HOUR);
  } else if (state < SHOW_MINUTE) {
    // do nothing
  } else if (state < BLANK_OUT) {
    blank_out(led, state - SHOW_MINUTE);
  }
  // else (SHOW_BLANK) do nothing

  memcpy(led, data, sizeof(data));
}

