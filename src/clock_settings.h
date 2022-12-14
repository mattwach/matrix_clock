// Contains program settings, stored in flash
#ifndef CLOCK_SETTINGS_H
#define CLOCK_SETTINGS_H

#include <inttypes.h>

//Change this when adding fields.  It will cause all settings to reset
#define CLOCK_SETTINGS_VERSION 2

struct ClockSettings {
  uint32_t checksum;  // the sum of all byte in this structure except for this one
  uint8_t eyecatcher[4];  // 'MCLK'
  uint32_t version;  // bump this when structure changes

  uint8_t brightness;  // brightness value
  uint8_t startup_display_mode;
  
  // sleep settings in hhmm format.  If they are equal, then sleep is OFF
  uint16_t sleep_time;
  uint16_t wake_time;
};

void clock_settings_init(void);
// Call this regularly.  Returns a 1 if the time needs to be updated
void clock_settings_poll(uint16_t time_hhmm);

const struct ClockSettings* clock_settings(void);

// Converts a ClockSettings->brightness to 0-255 brightness
uint8_t brightness_step_to_brightness(const struct ClockSettings* settings);

#endif
