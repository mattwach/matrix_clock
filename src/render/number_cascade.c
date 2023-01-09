#include "number_cascade.h"

#include "../led_matrix.h"
#include "../number_draw.h"

#define FRAMES_PER_SCROLL 7
#define FONT FONT3X5

static int8_t hours_ypos;  // upper right of hour text
static struct NumberFont font; 

void number_cascade_render(
    uint32_t* led,
    uint32_t frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings) {
  if (frame_index == 0) {
    hours_ypos = 0;
    frame_index = 1; // don't scroll right away
    number_font_init(
        &font,
        brightness_step_to_brightness(settings),
        (LED_MATRIX_WIDTH / 2) - 1,
        LED_MATRIX_HEIGHT / 2 + 2,
        LED_MATRIX_WIDTH / 2);
  }
  font.y = hours_ypos;
  uint8_t show_hours = 1;
  for (;
       font.y < LED_MATRIX_HEIGHT;
       font.y += font.char_height, show_hours = !show_hours) {
    font.x = show_hours ? 0 : LED_MATRIX_WIDTH - (font.char_spacing * 2);
    const int8_t val = show_hours ? time_hhmm / 100 : time_hhmm % 100;
    draw_numbers(&font, led, val);
  }
  if ((frame_index % FRAMES_PER_SCROLL) == 0) {
    ++hours_ypos;
    if (hours_ypos > 0) {
      // need to set the ypos so that bottom
      // row of the minutes is showing
      hours_ypos = -(font.char_height * 2) + 1;
    }
  }
}
