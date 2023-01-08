// LED matrix implementation for 64x32 panels
// These panels are naturally programed in "landscape" mode
// but (IMO) the LED rain effect looks better when they are rotated.
// Thus the actual driver works in landscape mode and
// the interface is in portrait mode.

#include "led_matrix.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/mutex.h"
#include <string.h>

#if LED_MATRIX_WIDTH != 32
#error led_matrix_32x64 requires LED_MATRIX_WIDTH be set to 32
#endif

#if LED_MATRIX_HEIGHT != 64
#error led_matrix_32x64 requires LED_MATRIX_HEIGHT be set to 64
#endif

// The driver works in landscape mode.
#define LED_COLUMNS 64
#define LED_ROWS 32

// GPIO for address seclection line.  Sometimes called
// A or HA.  B, C, and D will follow this sequentially.
// e.g. if GPIO_HA = 9, then GPIO_HB = 10, GPIO_HC = 11, GPIO_HD = 12
#define ADDRESS_COUNT 16  // some displays set this to 8, if you only have A, B, C
#define GPIO_HA 9

// outputs for red and green rows
#define GPIO_R1 16
#define GPIO_G1 19
#define GPIO_B1 17

#define GPIO_R2 18
#define GPIO_G2 20
#define GPIO_B2 8

// other lines
#define GPIO_CLK 13
#define GPIO_LAT 15  // Sometimes called STB
#define GPIO_OE  14

// pixels are 0x00RRGGBB but each one is shifted << 8 before
// comparing to gamma.
#define BRIGHTNESS_STEP 0x1800
#define BRIGHTNESS_GAMMA 91

#define GPIO_ALL_PINKS_MASK ( \
  ((ADDRESS_COUNT - 1) << GPIO_HA) | \
  (1 << GPIO_R1)  | \
  (1 << GPIO_G1)  | \
  (1 << GPIO_B1)  | \
  (1 << GPIO_R2)  | \
  (1 << GPIO_G2)  | \
  (1 << GPIO_B2)  | \
  (1 << GPIO_CLK) | \
  (1 << GPIO_LAT) | \
  (1 << GPIO_OE))

static struct LEDMatrixFrame {
  mutex_t mut;
  uint32_t value;  // current value for on/off comparisons
  uint32_t step;
  uint32_t frame_a[LED_ROWS * LED_COLUMNS];
  uint32_t frame_b[LED_ROWS * LED_COLUMNS];
  uint32_t* active_frame;
  uint32_t* draw_frame;
} led;

static inline void set_rgb(
  uint16_t value,
  uint32_t pixel,
  uint8_t red_pin,
  uint8_t green_pin,
  uint8_t blue_pin) {
  // Note, I was using gpio_put_masked but switching to
  // gpio_put improved FPS from 1788 to 1863.
  gpio_put(blue_pin, (value <= (pixel & 0xFF)));
  pixel >>= 8;
  gpio_put(green_pin, (value <= (pixel & 0xFF)));
  pixel >>= 8;
  gpio_put(red_pin, (value <= pixel));
}

static inline void wait_ns(void) {
  __asm volatile ("nop\n");
}

static inline void wait_us(void) {
  sleep_us(1);
}

static void program_rows(
  uint32_t address,
  uint16_t value,
  const uint32_t* row0,
  const uint32_t* row1) {
  for (uint8_t column = 0; column < LED_COLUMNS; ++column) {
    const uint16_t idx = LED_COLUMNS - column - 1;
    set_rgb(value, row0[idx], GPIO_R1, GPIO_G1, GPIO_B1);
    set_rgb(value, row1[idx], GPIO_R2, GPIO_G2, GPIO_B2);
    // strobe the clock
    wait_ns();
    gpio_put(GPIO_CLK, 1);
    //wait_ns();
    gpio_put(GPIO_CLK, 0);
  }

  // latch the data
  //wait_ns();
  gpio_put(GPIO_OE, 1);
  //wait_ns();
  gpio_put(GPIO_LAT, 1);
  wait_ns();
  gpio_put(GPIO_LAT, 0);
  //wait_ns();
  // disable, write address, reenable
  // Note: This originally used gpio_put_masked but perf
  // improved from 1863FPS to 1974FPS after changing to gpio_put
  gpio_put(GPIO_HA, address & 1);
  address >>= 1;
  gpio_put(GPIO_HA + 1, address & 1);
  address >>= 1;
  gpio_put(GPIO_HA + 2, address & 1);
  address >>= 1;
  gpio_put(GPIO_HA + 3, address & 1);
  wait_us();  // a slightly longer wait is needed to avoid ghosting
  gpio_put(GPIO_OE, 0);
  //wait_ns();
}

void led_matrix_frame_update() {
  led.value += led.step;
  led.step = (led.step * BRIGHTNESS_GAMMA / 100);
  if (led.value > 0xFFFF) {
    led.value = BRIGHTNESS_STEP;
    led.step = BRIGHTNESS_STEP;
  }
  for (uint16_t address = 0; address < ADDRESS_COUNT; ++address) {
    // address zero, row zero is the bottom row if the panel.  Thus 0 -> (LED_ROWS - 1)
    const uint32_t* row0 = led.active_frame + (LED_ROWS - 1 - address) * LED_COLUMNS;
    // address zero, row one is the middle of the pannel.  This 0 -> (ADDRESS_COUNT - 1)
    const uint32_t* row1 = led.active_frame + (ADDRESS_COUNT - 1 - address) * LED_COLUMNS;
    program_rows(address, (uint16_t)led.value >> 8, row0, row1);
  }
}

static void start_core1(void) {
  while (1) {
    mutex_enter_blocking(&led.mut);
    led_matrix_frame_update(); 
    mutex_exit(&led.mut);
  }
}

void led_matrix_init() {
  gpio_init_mask(GPIO_ALL_PINKS_MASK);
  gpio_set_dir_out_masked(GPIO_ALL_PINKS_MASK);
  gpio_put(GPIO_OE, 1); // disable by default
  led.value = 0;
  led.step = BRIGHTNESS_STEP;
  memset(led.frame_a, 80, sizeof(led.frame_a));
  memset(led.frame_b, 0, sizeof(led.frame_b));
  led.active_frame = led.frame_a;
  led.draw_frame = led.frame_b;
  mutex_init(&led.mut);
  multicore_launch_core1(start_core1);    
}

void led_matrix_render(uint32_t* data) {
  // Rotate date from portrait mode to the landscape mode
  // that the driver needs.
  //
  // Also the data format is 0xIIRRGGBB and the
  // driver expected 0x00RRGGBB.  The logic below
  // converts from the first format to the second
  // one.
  for (uint16_t y=0; y<LED_MATRIX_HEIGHT; ++y) {
    for (uint16_t x=0; x<LED_MATRIX_WIDTH; ++x) {
      const uint32_t pixel_in = data[y * LED_MATRIX_WIDTH + x]; 
      const uint32_t br = pixel_in >> 24;
      const uint32_t r = (pixel_in >> 16) & 0xFF;
      const uint32_t g = (pixel_in >> 8) & 0xFF;
      const uint32_t b = pixel_in & 0xFF;
      const uint32_t pixel_out =
        ((r * br / 0xFF) << 16) |
        ((b * br / 0xFF) << 8) |
        (g * br / 0xFF);
      led.draw_frame[x * LED_COLUMNS + (LED_COLUMNS - y - 1)] = pixel_out;
    }
  }

  // swap the frames
  mutex_enter_blocking(&led.mut);
  uint32_t* tmp_frame = led.active_frame;
  led.active_frame = led.draw_frame;
  led.draw_frame = tmp_frame;
  mutex_exit(&led.mut);
}
