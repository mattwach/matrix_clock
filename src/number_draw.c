#include "number_draw.h"
#include "colors.h"
#include "led_matrix.h"

static void pixel(
  uint32_t* led,
  uint8_t x,
  uint8_t y,
  uint32_t color
) {
  if (x >= LED_MATRIX_WIDTH) {
    return;
  }
  if (y >= LED_MATRIX_HEIGHT) {
    return;
  }
  led[(y * LED_MATRIX_HEIGHT) + x] = color;
}

static void hline(
  uint32_t* led,
  uint8_t x,
  uint8_t y,
  uint8_t w,
  uint32_t color
) {
  const uint8_t endx = x + w;
  for (; x < endx; ++x) {
    pixel(led, x, y, color);
  }
}

static void vline(
  uint32_t* led,
  uint8_t x,
  uint8_t y,
  uint8_t h,
  uint32_t color
) {
  const uint8_t endy = y + h;
  for (; y < endy; ++y) {
    pixel(led, x, y, color);
  }
}

static void draw0(uint32_t* led, uint8_t x, uint8_t y, uint32_t color) {
  hline(led, x, y, NUMBER_WIDTH, color);  // bottom line
  vline(led, x, y+1, NUMBER_HEIGHT - 2, color);  // left side
  vline(led, x + NUMBER_WIDTH - 1, y+1, NUMBER_HEIGHT - 2, color);  // right side
  hline(led, x, y + NUMBER_HEIGHT - 1, NUMBER_WIDTH, color);  // top line
}

static void draw1(uint32_t* led, uint8_t x, uint8_t y, uint32_t color) {
  vline(led, x + NUMBER_WIDTH / 2, y, NUMBER_HEIGHT, color);
}

static void draw2(uint32_t* led, uint8_t x, uint8_t y, uint32_t color) {
  hline(led, x, y, NUMBER_WIDTH, color);  // bottom line
  vline(led, x, y + 1, NUMBER_HEIGHT / 2 - 1, color);
  hline(led, x, y + NUMBER_HEIGHT / 2, NUMBER_WIDTH, color);  // middle line
  vline(led, x + NUMBER_WIDTH - 1, y + NUMBER_HEIGHT / 2 + 1, NUMBER_HEIGHT / 2 - 1, color);
  hline(led, x, y + NUMBER_HEIGHT - 1, NUMBER_WIDTH, color);  // top line
}

static void draw3(uint32_t* led, uint8_t x, uint8_t y, uint32_t color) {
  hline(led, x, y, NUMBER_WIDTH, color);  // bottom line
  hline(led, x, y + NUMBER_HEIGHT / 2, NUMBER_WIDTH - 1, color);  // middle line
  hline(led, x, y + NUMBER_HEIGHT - 1, NUMBER_WIDTH, color);  // top line
  vline(led, x + NUMBER_WIDTH - 1, y+1, NUMBER_HEIGHT - 2, color);  // right side
}

static void draw4(uint32_t* led, uint8_t x, uint8_t y, uint32_t color) {
  vline(led, x + NUMBER_WIDTH - 1, y, NUMBER_HEIGHT, color);  // right edge
  vline(led, x, y + NUMBER_HEIGHT / 2, NUMBER_HEIGHT / 2 + 1, color); // left side
  hline(led, x + 1, y + NUMBER_HEIGHT / 2, NUMBER_WIDTH - 2, color); // center bar
}

static void draw5(uint32_t* led, uint8_t x, uint8_t y, uint32_t color) {
  hline(led, x, y, NUMBER_WIDTH, color);  // bottom line
  vline(led, x + NUMBER_WIDTH - 1, y + 1, NUMBER_HEIGHT / 2 - 1, color);
  hline(led, x, y + NUMBER_HEIGHT / 2, NUMBER_WIDTH, color);  // middle line
  vline(led, x, y + NUMBER_HEIGHT / 2 + 1, NUMBER_HEIGHT / 2 - 1, color);
  hline(led, x, y + NUMBER_HEIGHT - 1, NUMBER_WIDTH, color);  // top line
}

static void draw6(uint32_t* led, uint8_t x, uint8_t y, uint32_t color) {
  vline(led, x, y, NUMBER_HEIGHT, color);  // left
  hline(led, x + 1, y, NUMBER_WIDTH - 1, color); // bottom
  hline(led, x + 1, y + NUMBER_HEIGHT / 2, NUMBER_WIDTH - 1, color); // middle
  hline(led, x + 1, y + NUMBER_HEIGHT - 1, NUMBER_WIDTH - 1, color); // top
  vline(led, x + NUMBER_WIDTH - 1, y + 1, y + NUMBER_HEIGHT / 2 - 1, color); // right
}

static void draw7(uint32_t* led, uint8_t x, uint8_t y, uint32_t color) {
  vline(led, x + NUMBER_WIDTH - 1, y, NUMBER_HEIGHT, color);  // right
  hline(led, x, y + NUMBER_HEIGHT - 1, NUMBER_WIDTH - 1, color); // top
}

static void draw8(uint32_t* led, uint8_t x, uint8_t y, uint32_t color) {
  vline(led, x, y, NUMBER_HEIGHT, color);  // left
  vline(led, x + NUMBER_WIDTH - 1, y, NUMBER_HEIGHT, color);  // right
  hline(led, x + 1, y + NUMBER_HEIGHT - 1, NUMBER_WIDTH - 2, color); // top
  hline(led, x + 1, y + NUMBER_HEIGHT / 2, NUMBER_WIDTH - 2, color); // middle
  hline(led, x + 1, y, NUMBER_WIDTH - 2, color); // bottom
}

static void draw9(uint32_t* led, uint8_t x, uint8_t y, uint32_t color) {
  vline(led, x, y + NUMBER_HEIGHT / 2, NUMBER_HEIGHT / 2 + 1, color);  // left
  vline(led, x + NUMBER_WIDTH - 1, y, NUMBER_HEIGHT, color);  // right
  hline(led, x + 1, y + NUMBER_HEIGHT - 1, NUMBER_WIDTH - 2, color); // top
  hline(led, x + 1, y + NUMBER_HEIGHT / 2, NUMBER_WIDTH - 2, color); // middle
  hline(led, x, y, NUMBER_WIDTH - 1, color); // bottom
}

void (*drawfn[10])(uint32_t* led, uint8_t x, uint8_t y, uint32_t color) = {
  draw0,
  draw1,
  draw2,
  draw3,
  draw4,
  draw5,
  draw6,
  draw7,
  draw8,
  draw9,
};

void number_draw(uint32_t* led, uint8_t digit, uint8_t x, uint8_t y, uint32_t color) {
  if (digit > 9) {
    return;
  }
  drawfn[digit](led, x, y, color);
}

// Draws val to led memory.  val must be 0-99.
void draw_numbers(uint32_t* led, uint8_t val, int8_t brightness) {
  const uint8_t tens = val / 10;
  number_draw(led, tens, 0, 0, ((uint32_t)brightness << 24) | get_color(tens));
  const uint8_t ones = val % 10; 
  number_draw(led, ones, 4, 0, ((uint32_t)brightness << 24) | get_color(ones));
}
