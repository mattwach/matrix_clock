#include "clock.h"
#include "hardware/i2c.h"
#include <stdio.h>

// GP4 - Pin6 is the default
#define SDA_PIN PICO_DEFAULT_I2C_SDA_PIN
// GP5 - Pin 7 is the default
#define SCL_PIN PICO_DEFAULT_I2C_SCL_PIN

#define DS3231_I2C_ADDRESS 0x68

uint16_t clock_get_time() {
  // Need two bytes of data to capture hours and minutes from tje
  // DS3231.
  uint8_t buf[2];
  const uint8_t addr = 0x01;  // register address that starts with minute

  // Read address 0x01 and 0x02 from the DS3231
  i2c_write_blocking(i2c0, DS3231_I2C_ADDRESS, &addr, 1, true);
  const int err = i2c_read_blocking(i2c0, DS3231_I2C_ADDRESS, buf, 2, false);
  if (err < 0) {
      return 100 - err;
  }
  // The minute byte is not a direct binary number but is instead
  // has a bit format of 0HHHLLLL, where H is the "tens" place and
  // L is the "ones"
  const uint8_t minute = (buf[0] & 0x0F) + ((buf[0] >> 4) * 10);

  // Hours is a bit more involved to process due to the possibility
  // of 12h and 24h modes.  If you know what mode the clock is going
  // to be in, then the code below can be simplified.
  uint8_t hours = buf[1] & 0x0F;
  if (buf[1] & 0x10) {
    hours += 10;
  }
  if (buf[1] & 0x40) {
    // 12 hour mode
    if (buf[1] & 0x20) {
      hours += 12;
    }
  } else {
    if (buf[1] & 0x20) {
      hours += 20;
    }
  }
  return (hours * 100) + minute;
}

void clock_set_time(uint16_t time_hhmm) {
  const uint8_t minutes_ones = time_hhmm % 10;
  const uint8_t minutes_tens = (time_hhmm / 10) % 10;
  const uint8_t hours_ones = (time_hhmm / 100) % 10;
  const uint8_t hours_tens = (time_hhmm / 1000) % 10;
  // set to 15:04:05
  uint8_t buf[4];
  buf[0] = 0x00;  // Register address
  buf[1] = 0x00;  // seconds
  buf[2] = (minutes_tens << 4) | minutes_ones;  // minutes
  buf[3] = (hours_tens << 4) | hours_ones;  // hours, in 24h format
  int err = i2c_write_blocking(i2c0, DS3231_I2C_ADDRESS, buf, 4, false);
  if (err >= 0) {
    printf("Clock set to %02d:%02d.\n", time_hhmm / 100, time_hhmm % 100);
  } else {
    printf("Clock set error %d\n", err);
  }
}

void clock_init() {
  i2c_init(i2c0, 100 * 1000);
  gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
}
