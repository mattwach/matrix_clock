#include "buttons.h"
#include "debounce.h"
#include "led_matrix.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

#if defined(led_matrix_dotstar)
  #define INCREMENT_BUTTON_GPIO 15
  #define SELECT_BUTTON_GPIO 14
#elif defined(led_matrix_64x32)
  #define INCREMENT_BUTTON_GPIO 26
  #define SELECT_BUTTON_GPIO 27
#else
  #error Unknown LED_MATRIX_SOURCE
#endif

#define CURRENT_OVER_GPIO 7
#define CURRENT_OVER_FRAMES (5000 / FRAME_DELAY_MS)
#define DEBOUNCE_MS 10

struct Debounce select_db;
struct Debounce increment_db;
uint8_t button_bit_array;
uint32_t current_over_frames;

static inline uint32_t uptime_ms() {
  return to_ms_since_boot(get_absolute_time());
}

// Callback for gpio_set_irq_callback.  Since it's an ISR,
// try to avoid expensive operations.
static void button_pressed_callback(uint gpio, uint32_t events) {
  switch (gpio) {
    case SELECT_BUTTON_GPIO:
      // The callback helper helps filter away debounce glitches
      // The debounce_gpio_irq_callback_helper has a side effect (updates state in
      // the debounce structure so it should always be called.
      if (debounce_gpio_irq_callback_helper(&select_db, uptime_ms(), events) &&
          select_db.val) {
        button_bit_array |= SELECT_BUTTON;
      }
      break;
    case INCREMENT_BUTTON_GPIO:
      // The comment for SELECT_BUTTON_GPIO above also apply here.
      if (debounce_gpio_irq_callback_helper(&increment_db, uptime_ms(), events) &&
          increment_db.val) {
        button_bit_array |= INCREMENT_BUTTON;
      }
      break;
    case CURRENT_OVER_GPIO:
      current_over_frames = CURRENT_OVER_FRAMES;
      break;
  }
}

// Sets up a pin to sense a button press.  Hardware-wise the button is
// connected to the pin and to ground.  The code below sets the internal
// pullup for the button so the pin will sit at 3.3V when the button
// is not pressed.  When the button is pressed, the 3.3V will be pulled
// to ground which is an event that the pico is configured to recognize
// and raise an interrupt for.
static void setup_gpio(uint gpio) {
  gpio_init(gpio);
  gpio_set_dir(gpio, GPIO_IN);
  gpio_pull_up(gpio);
  sleep_ms(1);  // give the pullup some time to do it's thing. Maybe not needed.
  gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
}

void buttons_init(void) {
  // Setup two GPIOs for select and increment buttons.
  // These are connected to ground though a button.  The chip
  // is then configured to use internal pullup resistors.
  setup_gpio(SELECT_BUTTON_GPIO);
  setup_gpio(INCREMENT_BUTTON_GPIO);

  // current over is for detecting if the current was limited
  setup_gpio(CURRENT_OVER_GPIO);

  gpio_set_irq_callback(button_pressed_callback);
  irq_set_enabled(IO_IRQ_BANK0, true);

  debounce_init(&select_db, DEBOUNCE_MS);
  debounce_init(&increment_db, DEBOUNCE_MS);
  button_bit_array = 0x00;
}

uint8_t is_current_over(void) {
  if (current_over_frames > 0) {
    --current_over_frames;
    return 1;
  }
  return 0;
}

uint8_t buttons_get(void) {
  const uint8_t value = button_bit_array;
  button_bit_array = 0x00;  // reset
  return value;
}
