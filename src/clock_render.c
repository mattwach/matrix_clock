#include "clock_render.h"
#include "buttons.h"
#include "render/blank.h"
#include "render/bounce.h"
#include "render/fade.h"
#include "render/glow.h"
#include "render/matrix.h"
#include "render/matrix_with_numbers.h"
#include "render/number_cascade.h"
#include "render/number_cascade_hires.h"
#include "render/waveform.h"

// This is the "render multiplexer" file.  It registers a table
// of rendering options (called DisplayMode) and provides logic to
// call the "current" one and select between them with the
// INCREMENT_BUTTON.
//
// You can easily customize display_modes[], removing, rearranging
// or adding your own new entries.  Anything you add should be put
// in the render/ directory for consistency.  Using the existing
// modes as a starting point (such as render/blank.c) should make
// the process relatively straight-forwrd for those who are
// already familiar with C.
//
// This file also supports the concept of sleep_time and wake_time
// which is used to automatically change the display mode to "off"
// when sleep_time is encoundered and change it back when 
// wake_time is encountered..  This only happens if sleep_time != wake_time

struct DisplayMode {
  const char* name;
  void (*render)(
      uint32_t* led,
      uint32_t frame_index,
      uint16_t time_hhmm,
      const struct ClockSettings* settings);
};

#if defined(led_matrix_dotstar)
  struct DisplayMode display_modes[] = {
    {"matrix", matrix_render},  // This entry will be the default power-on mode
    {"number_cascade", number_cascade_render},
    {"number_fade", fade_render},
    {"off", blank_render},  // always put this entry at the end of the list
  };
#elif defined(led_matrix_64x32)
  struct DisplayMode display_modes[] = {
    {"matrix_with_numbers", matrix_with_numbers_render},  // This entry will be the default power-on mode
    {"matrix", matrix_render},
    {"bounce", bounce_render},
    {"number_cascade", number_cascade_hires_render},
    {"waveform", waveform_render},
    {"glow", glow_render},
    {"off", blank_render},  // always put this entry at the end of the list
  };
#else
  #error Unknown LED_MATRIX_SOURCE
#endif

#define NUM_DISPLAY_MODES (sizeof(display_modes) / sizeof(display_modes[0]))
#define OFF_DISPLAY_MODE_INDEX (NUM_DISPLAY_MODES - 1)

static uint16_t last_sleep_or_wake_hhmm;  // used to create an edge trigger
static uint16_t next_mode_change_hhmm;
// this is so initilization can happen when switching modes
static uint32_t frame_index_delta = 0;
static uint8_t display_mode; // currently active index
static uint8_t wake_display_mode; // what mode to wake up to
static uint8_t display_mode_intitialized = 0;

// interface for display modes
uint8_t clock_render_num_display_modes(void) {
  return NUM_DISPLAY_MODES;
}

void clock_render_set_display_mode(uint8_t mode) {
  if (mode >= NUM_DISPLAY_MODES) {
    return;
  }
  display_mode = mode;
}

uint8_t clock_render_get_display_mode(void) {
  return display_mode;
}

// interface for getting names.  Used by clock_settings.c
const char* clock_render_display_mode_name(uint8_t mode) {
  if (mode < NUM_DISPLAY_MODES) {
    return display_modes[mode].name;
  }
  return "unknown";
}

static void increment_display_mode(uint32_t frame_index) {
  ++display_mode;
  if (display_mode >= NUM_DISPLAY_MODES) {
    display_mode = 0;
  }
  // the mode to return to when waking up
  wake_display_mode = display_mode;
  // This is done to show numbers right away in DISPLAY_NUMBERS mode
  // +1 because check_buttons is called at the end of the loop
  frame_index_delta = frame_index + 1;
  clock_render_reset_mode_change();
}

// logic that checks button state and changed the display mode if needed.
static uint8_t check_buttons(uint8_t button_pressed, uint32_t frame_index) {
  if (button_pressed & INCREMENT_BUTTON) {
    increment_display_mode(frame_index);
  }
  if (button_pressed & SELECT_BUTTON) {
    // The user asked to go into set time mode (whether they realize it or not).
    return 1;
  }
  return 0;
}

void clock_render_reset_mode_change(void) {
  next_mode_change_hhmm = 0xFFFF;
}

// Initializes data the first time it is called.
static void maybe_clock_init(
  uint32_t frame_index,
  uint8_t time_hhmm,
  const struct ClockSettings* settings) {
  if (!display_mode_intitialized) {
    last_sleep_or_wake_hhmm = 0xFFFF;
    clock_render_reset_mode_change();
    display_mode = settings->startup_display_mode;
    wake_display_mode = display_mode;
    display_mode_intitialized = 1;
  }
  if (frame_index == 0) {
    // reset the delta so that display_modes that need to
    // do something when they are first selected can
    // get an effective frame_index = 0
    frame_index_delta = 0;
  }
}

// This logic implements sleep_time and wake_time
static void check_for_sleep_and_wake(
  uint16_t time_hhmm,
  const struct ClockSettings* settings) {
  if (settings->sleep_time == settings->wake_time) {
    return;
  }
  // last_sleep_or_wake_hhmm could be removed.  The penalty for
  // doing so is that if the user wants to change modes and
  // the time happens to be sleep_time or wake_time, then
  // the clock will continuously override the user setting until
  // the minute has passed.
  if (last_sleep_or_wake_hhmm == time_hhmm) {
    return;
  }
  if (time_hhmm == settings->sleep_time) {
    display_mode = OFF_DISPLAY_MODE_INDEX;
    last_sleep_or_wake_hhmm = time_hhmm;
  } else if (time_hhmm == settings->wake_time) {
    display_mode = wake_display_mode;
    last_sleep_or_wake_hhmm = time_hhmm;
  }
}

static uint16_t add_minutes(uint16_t time_hhmm, uint16_t delta) {
  uint16_t minutes = (time_hhmm % 100) + delta;
  uint16_t hours = time_hhmm / 100;
  while (minutes >= 60) {
    ++hours;
    minutes -= 60;
  }
  return (hours % 24) * 100 + minutes;
}

static void check_for_auto_mode_change(
  uint16_t time_hhmm,
  const struct ClockSettings* settings,
  uint32_t frame_index) {
  if (settings->mode_change_minutes == 0) {
    // disabled
    return;
  }
  if (display_mode == OFF_DISPLAY_MODE_INDEX) {
    return;
  }
  if (time_hhmm == next_mode_change_hhmm) {
    do {
      increment_display_mode(frame_index);
    } while ((settings->enabled_modes & (1 << display_mode)) == 0);
  }
  if (next_mode_change_hhmm == 0xFFFF) {
    next_mode_change_hhmm = add_minutes(time_hhmm, settings->mode_change_minutes);
    // Odd observation. With this previous code:
    // framerates took a huge hit on the matrix mode.  This
    // happened even when this function was not called at all.
    // It seems like having random() here is changing something
    // fundamental with the build
    /*
    next_mode_change_hhmm =
      time_hhmm +
      settings->mode_change_min_minutes +
      (random() % (settings->mode_change_max_minutes -
                   settings->mode_change_min_minutes));
    */
  }
}
  
// public interface.  Called to render all particles and update their state.
uint8_t clock_render(
    uint32_t* led,
    uint8_t button_pressed,
    uint32_t frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings) {
  maybe_clock_init(frame_index, time_hhmm, settings);
  check_for_sleep_and_wake(time_hhmm, settings);
  check_for_auto_mode_change(time_hhmm, settings, frame_index);
  display_modes[display_mode].render(
      led,
      frame_index - frame_index_delta,
      time_hhmm,
      settings);
  return check_buttons(button_pressed, frame_index);
}
