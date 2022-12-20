#include "debug.h"
#include <stdio.h>
#include <stdarg.h>

#include "led_matrix.h"

#define RED_BIT 0x04
#define GREEN_BIT 0x02
#define BLUE_BIT 0x01
static void dump_point(uint32_t pixel) {
  if ((pixel & 0xFF000000) == 0) {
    putchar(pixel ? '?' : '.');
    return;
  }
  // only look at bit 3-7
  uint8_t color = 0x00;
  if ((pixel >> 16) & 0xF0) {
    color |= RED_BIT;
  }
  if ((pixel >> 8) & 0xF0) {
    color |= GREEN_BIT;
  }
  if (pixel & 0xF0) {
    color |= BLUE_BIT;
  }
  char c = ':';
  switch (color) {
    case BLUE_BIT:
      c = 'B';
      break;
    case GREEN_BIT:
      c = 'G';
      break;
    case BLUE_BIT | GREEN_BIT:
      c = 'A';
      break;
    case RED_BIT:
      c = 'R';
      break;
    case RED_BIT | BLUE_BIT:
      c = 'M';
      break;
    case RED_BIT | GREEN_BIT:
      c = 'Y';
      break;
    case RED_BIT | GREEN_BIT | BLUE_BIT:
      c = 'W';
      break;
  }
  putchar(c);
}

void debug_dump_led(const uint32_t* led) {
  for (int8_t y=LED_MATRIX_HEIGHT - 1; y >= 0; --y) {
    for (uint8_t x=0; x<LED_MATRIX_WIDTH; ++x) {
      dump_point(led[get_pixel_idx(x, y)]);
    }
    putchar('\n');
  }
}

void debug_wait_for_key(void) {
  printf("Press any key...\n");
  getchar();
}

void debug_printf(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}

void debug_dump_led_with_wait(const char* msg, const uint32_t* led) {
  printf("%s\n", msg);
  debug_dump_led(led);
  debug_wait_for_key();
}

