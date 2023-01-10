#include "set_time_low_res.h"

#include "buttons.h"
#include "clock.h"
#include "led_matrix.h"
#include "number_draw.h"
#include <stdlib.h>

// hold the time when the state was first entered.  This
// is used to avoid setting the clock (which changes seconds
// to zero) if the user just scrolls through the time without
// changing anything.
static uint16_t orig_time;

// UI state.
static uint8_t hours;
static uint8_t minutes;
static uint8_t showing_minutes;

static struct NumberFont font;

// Increments minutes
static void increment_minutes(void) {
  ++minutes;
  if (minutes >= 60) {
    minutes = 0;
  }
}

// Increments hours
static void increment_hours(void) {
  ++hours;
  if (hours >= 24) {
    hours = 0;
  }
}

// Increments either the hours or minutes
static void increment_current(void) {
  if (showing_minutes) {
    increment_minutes();
  } else {
    increment_hours();
  }
}

// public interface.  Called to show and allow changing of the time
uint8_t set_time_lowres_render(
    uint32_t* led,
    uint8_t button_pressed,
    uint32_t frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings) {
  if (frame_index == 0) {
    orig_time = time_hhmm;
    hours = time_hhmm / 100;
    minutes = time_hhmm % 100;
    showing_minutes = 0;
    number_font_init(
        &font,
        brightness_step_to_brightness(settings),
        LED_MATRIX_WIDTH / 2 - 1,
        LED_MATRIX_HEIGHT,
        LED_MATRIX_WIDTH / 4);
    }
  font.x = 0;
  font.y = 0;
  draw_numbers(
      &font,
      led,
      showing_minutes ? minutes : hours);
  if (button_pressed & INCREMENT_BUTTON) {
    increment_current();
  }
  if (button_pressed & SELECT_BUTTON) {
    if (showing_minutes) {
      const uint16_t new_time = hours * 100 + minutes;
      if (new_time != orig_time) {
        clock_set_time(new_time);
      }
      return 1;
    }
    showing_minutes = 1;
  }
  return 0;
}
