// LED matrix implementation for dotstar hardware
#include "led_matrix.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"

#define SCK_PIN 18
#define MOSI_PIN 19
#define SPI_SPEED 1000000

void led_matrix_init() {
  spi_init(spi0, SPI_SPEED);
  gpio_set_function(SCK_PIN, GPIO_FUNC_SPI);
  gpio_set_function(MOSI_PIN, GPIO_FUNC_SPI);
}

// writes 4 bytes via SPI
static inline void write_bytes(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4) {
  uint8_t msg[4] = {b1, b2, b3, b4};
  spi_write_blocking(spi0, msg, 4);
}

// starts a new frame
static inline void start_frame() {
  write_bytes(0, 0, 0, 0);
}

// displays a given color
static inline void color(uint32_t pixel) {
  const uint8_t br = pixel >> 27;  // The harware handles 0-31 only
  const uint8_t red = (pixel >> 16) & 0xFF;
  const uint8_t green = (pixel >> 8) & 0xFF;
  const uint8_t blue = pixel & 0xFF;
  write_bytes(0xE0 | br, blue, green, red);
}

void led_matrix_render(uint32_t* data) {
  start_frame();
  for (uint8_t i=0; i < LED_MATRIX_COUNT; ++i) {
    color(data[i]);
  }
}

void led_matrix_stop(void) { }

