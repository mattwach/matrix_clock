#include "clock_render.h"
#include "buttons.h"
#include "render/blank.h"
#include "render/guide.h"
#include "render/matrix.h"
#include "render/numbers.h"

// Display modes
#define DISPLAY_NORMAL 0
#define DISPLAY_GUIDE 1
#define DISPLAY_NUMBERS 2
#define DISPLAY_OFF 3
#define NUM_DISPLAY_MODES 4
uint16_t last_sleep_or_wake_hhmm;  // used to create an edge trigger
uint8_t display_mode;
uint8_t wake_display_mode; // what mode to wake up to
uint8_t display_mode_intitialized = 0;
// this is so initilization can happen when switching modes
uint32_t frame_index_delta = 0;

struct RenderModes {
  const char* name;
  void (*render)(
      uint32_t* led,
      uint32_t frame_index,
      uint16_t time_hhmm,
      const struct ClockSettings* settings);
};

// interface for display modes
uint8_t clock_render_num_display_modes(void) {
  return NUM_DISPLAY_MODES;
}

const char* clock_render_display_mode_name(uint8_t mode) {
  switch (mode) {
    case DISPLAY_NORMAL:
      return "normal";
    case DISPLAY_GUIDE:
      return "guide";
    case DISPLAY_NUMBERS:
      return "numbers";
    case DISPLAY_OFF:
      return "off";
  }

  return "unknown";
}

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
    return 1;
  }
  return 0;
}

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
    frame_index_delta = 0;
  }
}

static void check_for_sleep_and_wake(
  uint8_t time_hhmm,
  const struct ClockSettings* settings) {
  if (settings->sleep_time == settings->wake_time) {
    return;
  }
  if (last_sleep_or_wake_hhmm == time_hhmm) {
    return;
  }
  if (time_hhmm == settings->sleep_time) {
    display_mode = DISPLAY_OFF;
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
  frame_index -= frame_index_delta;

  switch (display_mode) {
    case DISPLAY_OFF:
      blank_render(led, frame_index, time_hhmm, settings);
      break;
    case DISPLAY_NORMAL:
      matrix_render(led, frame_index, time_hhmm, settings);
      break;
    case DISPLAY_GUIDE:
      guide_render(led, frame_index, time_hhmm, settings);
      break;
    case DISPLAY_NUMBERS:
      numbers_render(led, frame_index, time_hhmm, settings);
      break;
  }

  return check_buttons(button_pressed, frame_index);
}
