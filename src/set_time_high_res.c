#include "set_time_high_res.h"

#include "buttons.h"
#include "clock.h"
#include "led_matrix.h"
#include "number_draw.h"
#include <stdlib.h>

#define DIGIT_CURRENT_BRIGHTNESS 0xFF
#define DIGIT_SELECTED_BRIGHTNESS 0x40
#define DIGIT_OTHER_BRIGHTNESS 0x10

// hold the time when the state was first entered.  This
// is used to avoid setting the clock (which changes seconds
// to zero) if the user just scrolls through the time without
// changing anything.
static uint16_t orig_time;

#define PLACE_HOUR_TENS   0
#define PLACE_HOUR_ONES   1
#define PLACE_MINUTE_TENS 2
#define PLACE_MINUTE_ONES 3
static uint8_t place;
static uint8_t digits[4];

static struct NumberFont font;

static void init(uint16_t time_hhmm) {
  orig_time = time_hhmm;
  digits[0] = time_hhmm / 1000;
  digits[1] = (time_hhmm / 100) % 10;
  digits[2] = (time_hhmm / 10) % 10;
  digits[3] = time_hhmm % 10;
  place = PLACE_HOUR_TENS;
  // assuming a 32x64 display for the font sizes
  number_font_init(
    &font,
    0xFF,
    5,
    5,
    4
  );
}

static void render_digits(
  uint32_t* led,
  uint8_t val,
  uint8_t max_val,
  int16_t x,
  uint8_t is_current) {
  font.y = LED_MATRIX_HEIGHT - font.char_height - 1;
  for (uint8_t v=0; v <= max_val; ++v, font.y -= (font.char_height + 1)) {
    font.x = x;
    if (v != val) {
      font.brightness = DIGIT_OTHER_BRIGHTNESS;
    } else if (is_current) {
      font.brightness = DIGIT_CURRENT_BRIGHTNESS;
    } else {
      font.brightness = DIGIT_SELECTED_BRIGHTNESS;
    }
    number_draw(&font, led, v);
  }
}  

static void maybe_set_time(void) {
  uint16_t new_time =
    digits[0] * 1000 +
    digits[1] * 100 +
    digits[2] * 10 +
    digits[3];
  if (new_time != orig_time) {
      clock_set_time(new_time);
  }
}

static void check_for_digit_wrap() {
  for (uint8_t p=0; p<4; ++p) {
    uint8_t max_val = 9;
    switch (p) {
      case 0:
        max_val = 2;
        break;
      case 1:
        if (digits[0] == 2) {
          max_val = 3;
        }
        break;
      case 2:
        max_val = 5;
        break;
      default:
        break;
    }
    if (digits[p] > max_val) {
      digits[p] = 0;
    }
  }
}

// public interface.  Called to show and allow changing of the time
uint8_t set_time_highres_render(
    uint32_t* led,
    uint8_t button_pressed,
    uint32_t frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings) {
  if (frame_index == 0) {
    init(time_hhmm);
  }

  if (button_pressed & SELECT_BUTTON) {
    ++place;
    if (place == 4) {
      maybe_set_time();
      return 1;
    }
  }

  if (button_pressed & INCREMENT_BUTTON) {
    ++digits[place];
    check_for_digit_wrap();
  }

  render_digits(
    led,
    digits[0],
    2,
    0,
    place == 0);
  render_digits(
    led,
    digits[1],
    digits[0] == 2 ? 3 : 9,
    font.char_width,
    place == 1);
  render_digits(
    led,
    digits[2],
    5,
    LED_MATRIX_WIDTH - font.char_width * 2,
    place == 2);
  render_digits(
    led,
    digits[3],
    9,
    LED_MATRIX_WIDTH - font.char_width,
    place == 3);
  return 0;
}
