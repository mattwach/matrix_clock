#include "clock.h"
#include "hardware/rtc.h"
#include "pico/stdlib.h"
#include "pico/util/datetime.h"
#include <stdio.h>

uint16_t clock_get_time() {
  datetime_t t;
  rtc_get_datetime(&t);
  return (t.hour * 100) + t.min;
}

void clock_set_time(uint16_t time_hhmm) {
  const uint8_t hours = time_hhmm / 100;
  const uint8_t minutes = time_hhmm % 100;

  // Date does not matter for this application.
  datetime_t t = {
    .year  = 2023,
    .month = 1,
    .day   = 7,
    .dotw  = 6, // Saturday
    .hour  = hours,
    .min   = minutes,
    .sec   = 00
  };

  // Start the RTC and wait for it to take effect
  rtc_set_datetime(&t);
  sleep_us(64);
  printf("Clock set to %02d:%02d.\n", hours, minutes);
}

void clock_init() {
  rtc_init();
  clock_set_time(1345);
}
