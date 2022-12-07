#include "debounce.h"
#ifdef LIB_PICO_PLATFORM
  #include "hardware/gpio.h" 
#else
  #define GPIO_IRQ_EDGE_FALL 4
  #define GPIO_IRQ_EDGE_RISE 8
#endif


void debounce_init(struct Debounce* db, uint8_t hold_time_ms) {
  db->hold_time_ms = hold_time_ms;
  db->last_sample_time_ms = 0;
  db->val = 0;
}

uint8_t debounce_sample(struct Debounce* db, uint32_t time_ms, uint8_t val) {
  if (val == db->val) {
    return 0;  // didn't change
  }
  if ((db->last_sample_time_ms == 0) ||
      (time_ms < db->last_sample_time_ms) ||
      time_ms >= (db->last_sample_time_ms + db->hold_time_ms)) {
    db->val = val;
    db->last_sample_time_ms = time_ms;
    return 1;
  }
  return 0;
}

uint8_t debounce_gpio_irq_callback_helper(struct Debounce* db, uint32_t time_ms, uint32_t events) {
  uint8_t val = 0;
  switch (events) {
    case 0x00:
      val = db->val;  // keep it the same
      break;
    case GPIO_IRQ_EDGE_FALL:
      val = 1;  // it's pressed
      break;
    case GPIO_IRQ_EDGE_RISE:
      break;  // not pressed
    case GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL:
      val = 1 - db->val;  // invert the current state
      break;
  }
  return debounce_sample(db, time_ms, val);
}

