// Matrix clock
//
// By: Matt Wachowski

#include "pico/stdlib.h"
#include <string.h>
#include "clock.h"
#include "clock_render.h"
#include "clock_settings.h"
#include "led_matrix.h"
#include "buttons.h"
#include "set_time.h"
#include <stdio.h>

#define FRAME_DELAY_MS 25
#define TIME_UPDATE_FRAMES (1000/FRAME_DELAY_MS)

// set to 1 if the user is setting the time
uint8_t setting_time;

// format is 0xIIRRGGBB  See led_matrix.h for mor details.
uint32_t led[LED_MATRIX_COUNT];
uint16_t time_hhmm;  // a cache of the time to avoid calling for it as much

static inline uint32_t uptime_ms() {
  return to_ms_since_boot(get_absolute_time());
}

// Initialization function
static void init(void) {
  time_hhmm = 0;
  setting_time = 0;
  clock_settings_init();
  led_matrix_init();
  clock_init();
  buttons_init();
  sleep_ms(50);
}

// Updates time_hhmm periocially (based on TIME_UPDATE_FRAMES)
static void maybe_update_time(uint32_t frame_idx) {
  if ((frame_idx % TIME_UPDATE_FRAMES) == 0) {
    time_hhmm = clock_get_time();
  }
}

// called repeatedly by main to render a frame
static uint32_t render(uint32_t frame_idx) {
  uint32_t t1 = uptime_ms();
  maybe_update_time(frame_idx);
  memset(led, 0, sizeof(led));
  uint8_t toggle_setting_time = 0;
  if (setting_time) {
    toggle_setting_time = set_time_render(
          led, buttons_get(), frame_idx, time_hhmm, clock_settings());
  } else {
    toggle_setting_time = clock_render(
          led, buttons_get(), frame_idx, time_hhmm, clock_settings());
  }
  if (toggle_setting_time) {
    setting_time = !setting_time;
    frame_idx = 0;
  } else {
    ++frame_idx;
  }
  led_matrix_render(led);
  clock_settings_poll(time_hhmm);
  // calculate tdelta to get a smooth frame rate, even if the loop time
  // varies.
  uint32_t tdelta = uptime_ms() - t1;
  if (tdelta < FRAME_DELAY_MS) {
    sleep_ms(FRAME_DELAY_MS - tdelta);
  }
  return frame_idx;
}

// Program starting point
int main() {
  init();
  uint32_t frame_idx = 0;
  while (1) {
    frame_idx = render(frame_idx);
  }
}
