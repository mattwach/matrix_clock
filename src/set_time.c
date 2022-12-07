#include "set_time.h"

#include "buttons.h"
#include "clock.h"
#include "led_matrix.h"
#include "number_draw.h"
#include "colors.h"
#include <stdlib.h>

// hold the time when the state was first entered.  This
// is used to avoid setting the clock (which changes seconds
// to zero) if the user just scrolls through the time without
// changing anything.
uint16_t orig_time;

// UI state.
uint8_t hours;
uint8_t minutes;
uint8_t showing_minutes;

// Draws val to led memory.  val must be 0-99.
static void draw_numbers(uint32_t* led, uint8_t val) {
  const uint32_t brightness = 0x10000000;
  const uint8_t tens = val / 10;
  number_draw(led, tens, 0, 0, brightness | get_color(tens));
  const uint8_t ones = val % 10; 
  number_draw(led, ones, 4, 0, brightness | get_color(ones));
}

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
uint8_t set_time_render(
    uint32_t* led,
    uint8_t button_pressed,
    uint32_t frame_index,
    uint16_t time_hhmm) {
  if (frame_index == 0) {
    orig_time = time_hhmm;
    hours = time_hhmm / 100;
    minutes = time_hhmm % 100;
    showing_minutes = 0;
  }
  draw_numbers(
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
