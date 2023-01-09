#include "set_time.h"

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

#define BLINK_FRAMES (FRAME_DELAY_MS / 2)

// The display is different between the 8x8 and 32x64 matrix
// 8x8 -> HH or MM, whatever the current choice happens to be.
// 32x64 -> HH:MM with the current choice blinking at BLINK_FRAMES
#if LED_MATRIX_SOURCE == led_matrix_dotstar
  static void init_number_font(uint8_t brightness) {
      number_font_init(
          &font,
          brightness,
          LED_MATRIX_WIDTH / 2 - 1,
          LED_MATRIX_HEIGHT,
          LED_MATRIX_WIDTH / 4);
  }

  static void draw_time(uint32_t* led, uint32_t unused_frame_idx) {
    draw_numbers(
      &font,
      led,
      showing_minutes ? minutes : hours);
  }
#elif LED_MATRIX_SOURCE == led_matrix_32x64
  static void init_number_font(uint8_t brightness) {
      number_font_init(
          &font,
          brightness,
          (LED_MATRIX_WIDTH - 3) / 4 - 1,
          LED_MATRIX_HEIGHT,
          (LED_MATRIX_WIDTH - 3) / 4);
  }

  static void draw_time(uint32_t* led, uint32_t frame_idx) {
    if (showing_minutes || ((frame_idx % BLINK_FRAMES) > (BLINK_FRAMES / 2))) {
      draw_numbers(
        &font,
        led,
        hours);
    } else {
      font.x += font.char_spacing * 2;
    }

    number_draw_dash(&font, led);

    if (!showing_minutes || ((frame_idx % BLINK_FRAMES) > (BLINK_FRAMES / 2))) {
      draw_numbers(
        &font,
        led,
        minutes);
    }
  }
#else
  #error Unknown LED_MATRIX_SOURCE
#endif


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
    uint16_t time_hhmm,
    const struct ClockSettings* settings) {
  if (frame_index == 0) {
    orig_time = time_hhmm;
    hours = time_hhmm / 100;
    minutes = time_hhmm % 100;
    showing_minutes = 0;
    init_number_font(brightness_step_to_brightness(settings));
  }
  font.x = 0;
  font.y = 0;
  draw_time(led, frame_index);
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
