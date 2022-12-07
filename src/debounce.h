#ifndef LIB_UTIL_DEBOUNCE_H
#define LIB_UTIL_DEBOUNCE_H
// Simple debounce logic that throttles the update of a value.
//
// Poll example:
//
// void main(void) {
//    struct Debounce db;
//    debounce_init(&db, 8);
//    uint32_t time_ms = 0;
//    for (;; ++time_ms) {
//      // PB0 will only update every 8ms
//      if (debounce_sample(&db, time_ms, PINB & 0x01)) {
//        if (db.val) {
//          // button was pushed
//        } else {
//          // button was released
//        }
//      }
//      _delay_ms(1);
//    }
// }
//
// Interrupt example
//
// volatile uint32_t counter;
// struct Debounce db;
// void main(void) {
//    time_measure_init();
//    debounce_init(&db, 8);
//    // Setup pin change interrupt on PB0
//    GIMSK |= 1 << INT0;
//
//    while (1) {
//      lowpower_powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); 
//      cli();
//      const uint32_t local_counter = counter;
//      sei();
//      // do something with counter (print it, etc)
//      _delay_ms(1000);
//    }
// }
//
// ISR(PCINT0_vect)
// {
//   // called on real (and bouncy) rising and falling edges.
//   // rising is intersting to register the press
//   // falling is interesting to filter away decontact bounces.
//   if (debounce_sample(&db, time_measure_ms(), PINB & 0x01)) {
//     if (db.val) {
//       ++counter;
//     } else {
//       // do something on on up position if you want to
//     }
//   }
// }

#include <inttypes.h>

struct Debounce {
  volatile uint32_t last_sample_time_ms;  // the last time a sample was recorded
  uint8_t hold_time_ms;  // How long to filter away debounce signals
  volatile uint8_t val;
};

// Initialize
//
//  db: Debounce structure
//  hold_time_ms: The length of time to filter away debounce signals
void debounce_init(struct Debounce* db, uint8_t hold_time_ms);


// Provides a new value.  If the time difference is < hold_time_ms, then
// the old value will be returned (debounce).
//
//   db: Debounce structure
//   time_ms: A timetamp, in milliseconds
//   val: Latest value
//
// Returns: 1 if the value changed AND time_ms is high enough, zero otherwise
uint8_t debounce_sample(struct Debounce* db, uint32_t time_ms, uint8_t val);

// Callback helper for pi pico.
//
// Bascially the gpio_set_irq_callback is tricky to deal with because of the
// surprising behavoir of GPIO_IRQ_EDGE_FALL and GPIO_IRQ_EDGE_RISE sometimes
// both being active on the same callback.  This quickly lead to the realization
// of "which one happened first?"  "does it matter?"
//
// I went through this entire exersize in an isolated project named button_interrupt_analyze
// where all callback events were held in a ring buffer.  This allowed me to
// work out and verify the handling logic that is in this helper function.
//
// Usage:
//
// struct Debounce db;
//
// void main() {
//   ...
//   // can also to all of this with gpio_set_irq_enabled_with callback if you only
//   // have one button.
//   gpio_set_irq_enabled(BUTTON_GPIO, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
//   gpio_set_irq_callback(button_pressed_callback);
//   irq_set_enabled(IO_IRQ_BANK0, true);
//   ...
// }
//
// static void button_pressed_callback(uint gpio, uint32_t events) {
//   const uint8_t triggered = gpio_irq_callback_helper(&db, events);
//   const uint8_t is_pressed = db.val;
//   ...
// }
uint8_t debounce_gpio_irq_callback_helper(struct Debounce* db, uint32_t time_ms, uint32_t events);

#endif
