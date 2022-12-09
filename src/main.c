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

#define FRAME_DELAY_MS 32
#define TIME_UPDATE_FRAMES (1000/FRAME_DELAY_MS)

// Rendering modules. Called by the main loop depending on which
// state the clock is in.  Updates led[].
// Returns a 1 if the next renderer should be selected
//
// Parameters include:
//   led: array of leds, see led_matrix.h for more details
//   button_pressed: bit array of buttons that were pressed.  Edge-triggered
//   frame_index: The number of frame this renderer has been in use.  If a different
//     renderer is chosen, the frame_index is reset to zero.
uint8_t (*render_functions[])(
  uint32_t* led,
  uint8_t button_pressed,
  uint32_t frame_index,
  uint16_t time_hhmm,
  const struct ClockSettings* settings) = {
  clock_render,
  set_time_render,
};

#define RENDER_FN_COUNT (sizeof(render_functions) / sizeof(render_functions[0]))

// format is 0xIIRRGGBB  See led_matrix.h for mor details.
uint32_t led[LED_MATRIX_COUNT];
uint16_t time_hhmm;  // a cache of the time to avoid calling for it as much

static inline uint32_t uptime_ms() {
  return to_ms_since_boot(get_absolute_time());
}

// Initialization function
static void init(void) {
  time_hhmm = 0;
  clock_settings_init();
  led_matrix_init();
  clock_init();
  buttons_init();
  sleep_ms(50);
}

// A function that is used for debug and "learning" what the colors mean.
static void maybe_update_time(uint32_t frame_idx) {
  if ((frame_idx % TIME_UPDATE_FRAMES) == 0) {
    time_hhmm = clock_get_time();
  }
}

// called repeatedly by main to render a frame
static uint8_t render(uint32_t frame_idx, uint8_t render_fn_idx) {
  uint32_t t1 = uptime_ms();
  maybe_update_time(frame_idx);
  memset(led, 0, sizeof(led));
  const uint8_t next_render_fn = render_functions[render_fn_idx](
    led, buttons_get(), frame_idx, time_hhmm, clock_settings());
  led_matrix_render(led);
  clock_settings_poll(time_hhmm);
  uint32_t tdelta = uptime_ms() - t1;
  if (tdelta < FRAME_DELAY_MS) {
    sleep_ms(FRAME_DELAY_MS - tdelta);
  }
  return next_render_fn;
}

// Program starting point
int main() {
  init();
  uint8_t render_fn_idx = 0;
  uint32_t frame_idx = 0;
  while (1) {
    if (render(frame_idx, render_fn_idx)) {
      ++render_fn_idx;
      frame_idx = 0;
      if (render_fn_idx >= RENDER_FN_COUNT) {
        render_fn_idx = 0;
      }
    } else {
      ++frame_idx;
    }
  }
}
