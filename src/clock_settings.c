#include "clock_settings.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include "uart_console/console.h"
#include "colors.h"
#include "clock.h"
#include "clock_render.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// brightness is 0-255 and changed to 0-31 in led_matrix_dotstar.c
#define MIN_BRIGHTNESS 40   // 0-255
#define BRIGHTNESS_STEP_SIZE 20   // 0-255
#define BRIGHTNESS_STEPS 10   // 0-255

#define FLASH_OFFSET (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)
#define FLASH_ADDRESS ((uint8_t*)(XIP_BASE + FLASH_OFFSET))

// Most of the USB-console logic is offloaded into the uart_console
// library.
struct ConsoleConfig console;
struct ClockSettings settings;

// A simple checksum calculation that also incorporates
// CLOCK_SETTINGS_VERSION.  If CLOCK_SETTINGS_VERSION is changed
// then the checksum will fail (by design) until the data
// is rewritten with the updated version.
static uint32_t calc_checksum(const struct ClockSettings* cs) {
  uint8_t* start = ((uint8_t*)cs) + sizeof(uint32_t);
  size_t len = sizeof(struct ClockSettings) - sizeof(uint32_t);
  uint32_t sum = CLOCK_SETTINGS_VERSION * 1000000;
  for (size_t i=0; i<len; ++i) {
    sum += start[i];
  }
  return sum;
}

// saves current settingsw to flash
static void clock_settings_save(const struct ClockSettings* cs) {
  uint8_t buff[FLASH_PAGE_SIZE];
  memset(buff, 0, sizeof(buff));
  memcpy(buff, cs, sizeof(struct ClockSettings));
  struct ClockSettings* settings = (struct ClockSettings*)buff;
  settings->eyecatcher[0] = 'M';
  settings->eyecatcher[1] = 'C';
  settings->eyecatcher[2] = 'L';
  settings->eyecatcher[3] = 'K';
  settings->checksum = calc_checksum(settings);
  uint32_t ints = save_and_disable_interrupts();
  flash_range_erase(FLASH_OFFSET, FLASH_SECTOR_SIZE);
  flash_range_program(FLASH_OFFSET, buff, FLASH_PAGE_SIZE);
  printf("Settings Saved\n");
  restore_interrupts (ints);
}

// dumps current sleep settings to the console
static void output_sleep_data(void) {
  const char* sleep_enabled =
      (settings.sleep_time == settings.wake_time) ? "disabled" : "enabled";
  printf("sleep_time = %d (%s)\n", settings.sleep_time, sleep_enabled);
  printf("wake_time = %d (%s)\n", settings.wake_time, sleep_enabled);
}

// callback for the "get" shell command
static void get_cmd(uint8_t argc, char* argv[]) {
  printf("brightness = %d\n", settings.brightness_step);
  printf(
      "startup_display_mode = %s\n",
      clock_render_display_mode_name(settings.startup_display_mode));
  output_sleep_data();
}

// callback for the "brightness" shell command
static void brightness_cmd(uint8_t argc, char* argv[]) {
  int brightness = 0;
  if (strcmp(argv[0], "0")) {
    brightness = atoi(argv[0]);
    if ((brightness < 0) || (brightness > 10)) {
      printf("Please choose a brightness value between 0 and 10\n");
      return;
    }
  }
  if (brightness != settings.brightness_step) {
    settings.brightness_step = brightness;
    clock_settings_save(&settings);
  }
}

// helper function to convert a string to hhmm format
// e.g. "1234" -> 1234.  Does various sanity checks and
// returns -1 if an of these checks fail.
static int16_t parse_hhmm(const char* t) {
  if (strlen(t) != 4) {
    printf("Expected HHMM format (e.g. 1205), got %s\n", t);
    return -1;
  }
  for (uint8_t i=0; i<4; ++i) {
    if ((t[i] < '0') || (t[i] > '9')) {
      printf("Illegal character in HHMM, expected 0-9, got %c\n", t[i]);
      return -1;
    }
  }
  const uint8_t hour = (t[0] - '0') * 10 + (t[1] - '0');
  if (hour > 23) {
    printf("Expected HHMM hour to be 00-23, got %d\n", hour);
    return -1;
  }

  const uint8_t minute = (t[2] - '0') * 10 + (t[3] - '0');
  if (hour > 59) {
    printf("Expected HHMM minute to be 00-59, got %d\n", minute);
    return -1;
  }
  return hour * 100 + minute;
}

// callback for the "set_time" shell command
static void time_cmd(uint8_t argc, char* argv[]) {
  int16_t t = parse_hhmm(argv[0]);
  if (t >= 0) {
    // No flash update needed since the RTC hardware tracks the time
    clock_set_time(t);
    printf("Time updated\n");
  }
}

// callback for the "sleep_time" shell command
static void sleep_time_cmd(uint8_t argc, char* argv[]) {
  int16_t t = parse_hhmm(argv[0]);
  if (t >= 0) {
    settings.sleep_time = t;
    output_sleep_data();
    clock_settings_save(&settings);
  }
}

// callback for the "wake_time" shell command
static void wake_time_cmd(uint8_t argc, char* argv[]) {
  int16_t t = parse_hhmm(argv[0]);
  if (t >= 0) {
    settings.wake_time = t;
    output_sleep_data();
    clock_settings_save(&settings);
  }
}

// callback for the "list_display_modes" shell command
static void list_display_modes_cmd(uint8_t argc, char* argv[]) {
  const uint8_t num_modes = clock_render_num_display_modes();
  for (uint8_t i=0; i < num_modes; ++i) {
    printf("%s\n", clock_render_display_mode_name(i));
  }
}

// callback for the "startup_display_mode" shell command
static void startup_display_mode_cmd(uint8_t argc, char* argv[]) {
  const char* mode = argv[0];
  const uint8_t num_modes = clock_render_num_display_modes();
  for (uint8_t i=0; i < num_modes; ++i) {
    if (!strcmp(clock_render_display_mode_name(i), mode)) {
      settings.startup_display_mode = i;
      printf("Startup display mode set\n");
      clock_settings_save(&settings);
      return;
    }
  }
  printf("Unknown display mode: %s.  Try list_display_modes\n", mode);
}

// The list of supported shell commands, along with a short description and
// function callback.
struct ConsoleCallback callbacks[] = {
  {"brightness", "Change brightness from 0-10", 1, brightness_cmd},
  {"get", "Get current settings", 0, get_cmd},
  {"list_display_modes", "List display modes", 0, list_display_modes_cmd},
  {"time", "Sets the time as HHMM.  example: time 1307.", 1, time_cmd},
  {"sleep_time", "Sets the sleep (screen off) time as HHMM.  "
   "Disabled if sleep_time == wake_time.", 1, sleep_time_cmd},
  {"startup_display_mode", "Sets the startup display mode.", 1, startup_display_mode_cmd},
  {"wake_time", "Sets the wake (screen on) time as HHMM.  "
   "Disabled if sleep_time == wake_time.", 1, wake_time_cmd},
};
#define NUM_CALLBACKS (sizeof(callbacks) / sizeof(callbacks[0]))

// Looks at checksum and the eyecatcher to validate if the stored settings
// are good or bad.
static uint8_t validate_settings(const struct ClockSettings* cs) {
  if ((cs->eyecatcher[0] != 'M') ||
      (cs->eyecatcher[1] != 'C') ||
      (cs->eyecatcher[2] != 'L') ||
      (cs->eyecatcher[3] != 'K')) {
    return 0;
  }
  uint32_t checksum = calc_checksum(cs);
  if (checksum != cs->checksum) {
    return 0;
  }
  return 1;
}

// Called when the stored setting are invalid, maybe they were never
// written?  Sets "reasonable" defaults.
static void init_default_settings(void) {
  memset(&settings, 0, sizeof(struct ClockSettings));
  settings.brightness_step = 3;
}

void clock_settings_init() {
  uart_console_init(&console, callbacks, NUM_CALLBACKS, CONSOLE_VT102);
  struct ClockSettings flash_settings;
  memcpy(&flash_settings, FLASH_ADDRESS, sizeof(struct ClockSettings));
  if (!validate_settings(&flash_settings)) {
    init_default_settings();
  } else {
    memcpy(&settings, &flash_settings, sizeof(struct ClockSettings));
  }
}

void clock_settings_poll(uint16_t time_hhmm) {
  char prompt[80];
  sprintf(prompt, "%02d:%02d (%s,%s,%s)> ",
       time_hhmm / 100,
       time_hhmm % 100,
       get_color_name((time_hhmm / 100) % 10),
       get_color_name((time_hhmm / 10) % 10),
       get_color_name(time_hhmm % 10)
  );
  uart_console_poll(&console, prompt);
}

const struct ClockSettings* clock_settings(void) {
  return &settings;
}

uint8_t brightness_step_to_brightness(const struct ClockSettings* settings) {
  uint8_t brightness_step = settings->brightness_step;
  if (brightness_step > BRIGHTNESS_STEPS) {
    brightness_step = BRIGHTNESS_STEPS;
  } 
  return MIN_BRIGHTNESS + (BRIGHTNESS_STEP_SIZE * brightness_step);
}


