// Contains program settings, stored in flash
#ifndef CLOCK_SETTINGS_H
#define CLOCK_SETTINGS_H

// This module handles user settings.  It handles both the user interface
// (shell prompt over USB) and storing/loading from flash.

#include <inttypes.h>

//Change this when adding fields.  It will cause all settings to reset
#define CLOCK_SETTINGS_VERSION 4

// Here are the settings.  The checksum, eyecatcher and version fields
// are there to detect the cases where setting have not ever been written
// or were updated in an incompatible way.  Bad/unreliable flash could also
// cause mismatches here. Having the eyscatcher is probably caution overkill
// but it can help in troubleshooting efforts.
struct ClockSettings {
  uint32_t checksum;  // the sum of all byte in this structure except for this one
  uint8_t eyecatcher[4];  // 'MCLK'
  uint32_t version;  // bump this when structure changes

  uint8_t brightness_step;  // brightness value
  uint8_t startup_display_mode; // This is "normal" by default but the user can
                                // change it if preferred.
  
  // sleep settings in hhmm format.  If they are equal, then sleep is OFF
  uint16_t sleep_time;
  uint16_t wake_time;

  // mode change settings
  uint16_t enabled_modes;
  uint16_t mode_change_minutes;
};

void clock_settings_init(void);

// Call this regularly to give the user a responsive USB shell.
// returns a 1 if the frame_index needs to be reset
uint8_t clock_settings_poll(uint16_t time_hhmm, uint32_t last_fps);

// Get a read-only copy of the settings.
const struct ClockSettings* clock_settings(void);

// Converts a ClockSettings->brightness to 0-255 brightness
uint8_t brightness_step_to_brightness(const struct ClockSettings* settings);

#endif
