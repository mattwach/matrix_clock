#include "buttons.h"
#include "debounce.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

#define SELECT_GPIO 14
#define INCREMENT_GPIO 15
#define DEBOUNCE_MS 10

struct Debounce select_db;
struct Debounce increment_db;
uint8_t button_bit_array;

static inline uint32_t uptime_ms() {
  return to_ms_since_boot(get_absolute_time());
}

static void button_pressed_callback(uint gpio, uint32_t events) {
  switch (gpio) {
    case SELECT_GPIO:
      if (debounce_gpio_irq_callback_helper(&select_db, uptime_ms(), events) &&
          select_db.val) {
        button_bit_array |= SELECT_BUTTON;
      }
      break;
    case INCREMENT_GPIO:
      // Note debounce_gpio_irq_callback_helper has a side effect (updates state in
      // the debounce structure so it should always be called.
      if (debounce_gpio_irq_callback_helper(&increment_db, uptime_ms(), events) &&
          increment_db.val) {
        button_bit_array |= INCREMENT_BUTTON;
      }
      break;
  }
}

static void setup_gpio(uint gpio) {
  //gpio_init(gpio);  // maybe not needed
  //gpio_set_dir(gpio, GPIO_IN);  // maybe not needed
  gpio_pull_up(gpio);
  sleep_ms(1);  // give the pullup some time to do it's thing. Maybe not needed.
  gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
}

void buttons_init(void) {
  // Setup two GPIOs for select and increment buttons.
  // These are connected to ground though a button.  The chip
  // is then configured to use internal pullup resistors.
  setup_gpio(SELECT_GPIO);
  setup_gpio(INCREMENT_GPIO);
  gpio_set_irq_callback(button_pressed_callback);
  irq_set_enabled(IO_IRQ_BANK0, true);

  debounce_init(&select_db, DEBOUNCE_MS);
  debounce_init(&increment_db, DEBOUNCE_MS);
  button_bit_array = 0x00;
}

uint8_t buttons_get(void) {
  const uint8_t value = button_bit_array;
  button_bit_array = 0x00;  // reset
  return value;
}