#include "number_cascade_hires.h"
#include "../led_matrix.h"
#include "../number_draw.h"
#include <stdlib.h>
#include <stdio.h>

#define MINUTES_ONES 0
#define MINUTES_TENS 1
#define HOURS_ONES 2
#define HOURS_TENS 3

#define NUM_DIGITS 50
#define START_DIGIT_COUNT 8
#define DIGIT_INIT_ATTEMPTS 16
#define WAIT_FRAMES_MIN (500 / FRAME_DELAY_MS)
#define WAIT_FRAMES_MAX (2000 / FRAME_DELAY_MS)

struct DigitProps {
  uint8_t width;
  uint8_t height;
  uint8_t speed;  // a higher value is slower
};

const static struct DigitProps digit_props[] = {
  {5, 5, 5},   // MINUTE_ONES
  {7, 7, 4},   // MINUTE_TENS
  {9, 9, 3},   // HOUR_ONES
  {11, 11, 2}, // HOUR_TENS
};

static inline int16_t rand_range(int16_t min, int16_t max) {
  return min + (random() % (max - min));
}

struct Digit {
  // if wait_frame > 0, then the digit is inactive for the
  // specified number of frames.
  int16_t wait_frames;
  int16_t x;
  int16_t y;
  uint8_t type;
  uint8_t value;
};

static struct Digit digits[NUM_DIGITS];
static struct NumberFont font;

// Puts a digit into the waiting state
static inline void reset_digit(struct Digit* d) {
  d->wait_frames = rand_range(WAIT_FRAMES_MIN, WAIT_FRAMES_MAX);
}

// Initialize the digit array
static void init(uint8_t brightness) {
  for (uint16_t i=0; i<NUM_DIGITS; ++i) {
    reset_digit(digits + i);
  }
  for (uint16_t j=0; j<START_DIGIT_COUNT; ++j) {
    digits[j].wait_frames = 1; // so that the next decrement activates them
  }
  font.brightness = brightness;
}

// checks for overlap of a specific digit
static inline uint8_t digit_overlaps_existing2(
    uint8_t type, int16_t x, const struct Digit* digit) {
  if (digit->wait_frames > 0) {
    // not active
    return 0;
  }
  if (digit->type != type) {
    // different plane
    return 0;
  }
  if (digit->y < LED_MATRIX_HEIGHT) {
    // already onscreen
    return 0;
  }
  const uint8_t digit_width = digit_props[type].width; 
  if ((x + digit_width) < digit->x) {
    // the new digit will be to the left of the existing one
    return 0;
  }
  if (x > (digit->x + digit_width)) {
    // the new digit will be to the right of the existing one
    return 0;
  }
  // overlapping
  return 1;
}

// returns 1 if any active digits of the same type overlap the
// proposed new digit.
static uint8_t digit_overlaps_existing(uint8_t type, int16_t x) {
  for (uint16_t i=0; i<NUM_DIGITS; ++i) {
    if (digit_overlaps_existing2(type, x, digits + i)) {
      return 1;
    }
  }
  return 0;
}

// Initializes a specific digit slot
static inline uint8_t try_init_digit2(struct Digit* d, uint16_t time_hhmm) {
  const uint8_t type = random() % (HOURS_TENS + 1);  // 0-3
  const uint8_t digit_width = digit_props[type].width;
  const int16_t x = rand_range(-digit_width + 1, LED_MATRIX_WIDTH - 1);
  if (digit_overlaps_existing(type, x)) {
    return 0;  // failed attempt
  }
  d->type = type; 
  d->x = x;
  d->y = LED_MATRIX_HEIGHT + digit_props[type].height + 1;
  switch (type) {
    case MINUTES_ONES:
      d->value = time_hhmm % 10;
      break;
    case MINUTES_TENS:
      d->value = (time_hhmm / 10) % 10;
      break;
    case HOURS_ONES:
      d->value = (time_hhmm / 100) % 10;
      break;
    case HOURS_TENS:
      d->value = (time_hhmm / 1000) % 10;
      break;
  }
  return 1; // success
}

// Attempt to initialze a digit.  This may fail due to overlaps.
static void try_init_digit(struct Digit* d, uint16_t time_hhmm) {
  for (uint16_t attempt = 0; attempt < DIGIT_INIT_ATTEMPTS; ++attempt) {
    if (try_init_digit2(d, time_hhmm)) {
      return;
    }
  }
  // failed to initialize.  Reset the wait
  reset_digit(d);
}

// Advances the wait frames for a digits if it's currently inactive.
static inline void advance_wait_frame(struct Digit* d, uint16_t time_hhmm) {
  if (d->wait_frames == 0) {
    return;
  }
  --d->wait_frames;
  if (d->wait_frames == 0) {
    try_init_digit(d, time_hhmm);
  }
}

// Advances the wait frames for digits that are current inactive.
static void advance_wait_frames(uint16_t time_hhmm) {
  for (uint16_t i=0; i<NUM_DIGITS; ++i) {
    advance_wait_frame(digits + i, time_hhmm);
  }
}

// Renders a given digit.
static inline void render_digit(uint32_t* led, const struct Digit* d) {
  font.x = d->x;
  font.y = d->y;
  const struct DigitProps* p = digit_props + d->type;
  font.char_width = p->width;
  font.char_height = p->height;
  font.char_spacing = p->width + 1;
  number_draw(&font, led, d->value);
}

// Renders all active digits.
static void render(uint32_t* led) {
  // need to draw types in layers
  for (uint8_t type = 0; type < 4; ++type) {
    for (uint16_t i=0; i<NUM_DIGITS; ++i) {
      const struct Digit* d = digits + i;
      if ((d->wait_frames == 0) && (d->type == type)) {
        render_digit(led, d);
      }
    }
  }
}

// Advances the state of a given digit.
static inline void advance_active_frames2(uint32_t frame_index, struct Digit* d) {
  if (d->wait_frames > 0) {
    return;
  }
  const uint8_t frame_mod = digit_props[d->type].speed;
  if ((frame_index % frame_mod) != 0) {
    return;
  }
  const int16_t min_y = 0 - (int16_t)(digit_props[d->type].height);
  --d->y;
  if (d->y < min_y) {
    reset_digit(d);
  }
}

// Advances the state of all active digits.
static void advance_active_frames(uint32_t frame_index) {
  for (uint16_t i=0; i<NUM_DIGITS; ++i) {
    advance_active_frames2(frame_index, digits + i);
  }
}

void number_cascade_hires_render(
    uint32_t* led,
    uint32_t frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings) {
  if (frame_index == 0) {
    init(brightness_step_to_brightness(settings));
  }
  advance_wait_frames(time_hhmm);
  render(led);
  advance_active_frames(frame_index);
}

