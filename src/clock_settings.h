// Contains program settings, stored in flash
#ifndef CLOCK_SETTINGS_H
#define CLOCK_SETTINGS_H

#include <inttypes.h>

struct ClockSettings {
  uint32_t checksum;  // the sum of all byte in this structure except for this one
  uint8_t eyecatcher[4];  // 'MCLK'
  uint32_t version;  // bump this when structure changes

  uint8_t brightness;  // brightness value
  uint8_t startup_page;  // startup page.  Can be used to automatically show helpers
  
  // sleep settings in hhmm format.  If they are equal, then sleep is OFF
  uint16_t sleep_time;
  uint16_t wake_time;
};

void clock_settings_init(struct ClockSettings* cs);
void clock_settings_save(const struct ClockSettings* cs);

#endif
