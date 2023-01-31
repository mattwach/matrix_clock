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
#include "set_time_low_res.h"
#include "set_time_high_res.h"
#include <stdio.h>

#if defined(led_matrix_dotstar)
  #define SET_TIME_RENDER set_time_lowres_render
#elif defined(led_matrix_64x32)
  #define SET_TIME_RENDER set_time_highres_render
#else
  #error Unknown LED_MATRIX_SOURCE
#endif

#define TIME_UPDATE_FRAMES (1000/FRAME_DELAY_MS)
#define LED_PIN PICO_DEFAULT_LED_PIN

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
  setting_time = 0;
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  clock_settings_init();
  led_matrix_init();
  clock_init();
  buttons_init();
  sleep_ms(50);
  time_hhmm = clock_get_time();
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
  uint8_t buttons = buttons_get();
  buttons |= clock_settings_poll(time_hhmm);
  if (setting_time) {
    toggle_setting_time = SET_TIME_RENDER(
          led, buttons, frame_idx, time_hhmm, clock_settings());
  } else {
    toggle_setting_time = clock_render(
          led, buttons, frame_idx, time_hhmm, clock_settings());
  }
  ++frame_idx;
  if (toggle_setting_time) {
    setting_time = !setting_time;
    frame_idx = 0;
  }
  led_matrix_render(led);
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
    gpio_put(LED_PIN, is_current_over());
  }
}
