#include "clock_render.h"
#include "buttons.h"
#include "render/blank.h"
#include "render/guide.h"
#include "render/matrix.h"
#include "render/numbers.h"

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

struct DisplayMode display_modes[] = {
  {"normal", matrix_render},  // This entry will be the default power-on mode
  {"guide", guide_render},
  {"numbers", numbers_render},
  {"off", blank_render},  // always put this entry at the end of the list
};
#define NUM_DISPLAY_MODES (sizeof(display_modes) / sizeof(display_modes[0]))
#define OFF_DISPLAY_MODE_INDEX (NUM_DISPLAY_MODES - 1)

uint16_t last_sleep_or_wake_hhmm;  // used to create an edge trigger
// this is so initilization can happen when switching modes
uint32_t frame_index_delta = 0;
uint8_t display_mode; // currently active index
uint8_t wake_display_mode; // what mode to wake up to
uint8_t display_mode_intitialized = 0;

// interface for display modes
uint8_t clock_render_num_display_modes(void) {
  return NUM_DISPLAY_MODES;
}

// interface for getting names.  Used by clock_settings.c
const char* clock_render_display_mode_name(uint8_t mode) {
  if (mode < NUM_DISPLAY_MODES) {
    return display_modes[mode].name;
  }
  return "unknown";
}

// logic that checks button state and changed the display mode if needed.
static uint8_t check_buttons(uint8_t button_pressed, uint32_t frame_index) {
  if (button_pressed & INCREMENT_BUTTON) {
    ++display_mode;
    if (display_mode >= NUM_DISPLAY_MODES) {
      display_mode = 0;
    }
    // the mode to return to when waking up
    wake_display_mode = display_mode;
    // This is done to show numbers right away in DISPLAY_NUMBERS mode
    frame_index_delta = frame_index;
  }
  if (button_pressed & SELECT_BUTTON) {
    // The user asked to go into set time mode (whether they realize it or not).
    return 1;
  }
  return 0;
}

// Initializes data the first time it is called.
static void maybe_clock_init(
  uint32_t frame_index,
  uint8_t time_hhmm,
  const struct ClockSettings* settings) {
  if (!display_mode_intitialized) {
    last_sleep_or_wake_hhmm = 0xFFFF;
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
  uint8_t time_hhmm,
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
  
// public interface.  Called to render all particles and update their state.
uint8_t clock_render(
    uint32_t* led,
    uint8_t button_pressed,
    uint32_t frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings) {
  maybe_clock_init(frame_index, time_hhmm, settings);
  check_for_sleep_and_wake(time_hhmm, settings);
  display_modes[display_mode].render(
      led,
      frame_index - frame_index_delta,
      time_hhmm,
      settings);
  return check_buttons(button_pressed, frame_index);
}
