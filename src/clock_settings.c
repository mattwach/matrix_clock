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

#define FLASH_OFFSET (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)
#define FLASH_ADDRESS ((uint8_t*)(XIP_BASE + FLASH_OFFSET))

struct ConsoleConfig console;
struct ClockSettings settings;

static uint32_t calc_checksum(const struct ClockSettings* cs) {
  uint8_t* start = ((uint8_t*)cs) + sizeof(uint32_t);
  size_t len = sizeof(struct ClockSettings) - sizeof(uint32_t);
  uint32_t sum = CLOCK_SETTINGS_VERSION * 1000000;
  for (size_t i=0; i<len; ++i) {
    sum += start[i];
  }
  return sum;
}

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

static void get_cmd(uint8_t argc, char* argv[]) {
  printf("brightness = %d\n", settings.brightness);
  printf(
      "startup_display_mode = %s\n",
      clock_render_display_mode_name(settings.startup_display_mode));
}

static void brightness_cmd(uint8_t argc, char* argv[]) {
  int brightness = 0;
  if (strcmp(argv[0], "0")) {
    brightness = atoi(argv[0]);
    if ((brightness < 0) || (brightness > 10)) {
      printf("Please choose a brightness value between 0 and 10\n");
      return;
    }
  }
  if (brightness != settings.brightness) {
    settings.brightness = brightness;
    clock_settings_save(&settings);
  }
}

static void set_time_cmd(uint8_t argc, char* argv[]) {
  const char* t = argv[0];
  if (strlen(t) != 4) {
    printf("Expected HHMM format (e.g. 1205), got %s\n", t);
    return;
  }
  for (uint8_t i=0; i<4; ++i) {
    if ((t[i] < '0') || (t[i] > '9')) {
      printf("Illegal character in HHMM, expected 0-9, got %c\n", t[i]);
      return;
    }
  }
  const uint8_t hour = (t[0] - '0') * 10 + (t[1] - '0');
  if (hour > 23) {
    printf("Expected HHMM hour to be 00-23, got %d\n", hour);
    return;
  }

  const uint8_t minute = (t[2] - '0') * 10 + (t[3] - '0');
  if (hour > 59) {
    printf("Expected HHMM minute to be 00-59, got %d\n", minute);
    return;
  }
  clock_set_time(hour * 100 + minute);
  printf("Time updated\n");
}

static void list_display_modes_cmd(uint8_t argc, char* argv[]) {
  const uint8_t num_modes = clock_render_num_display_modes();
  for (uint8_t i=0; i < num_modes; ++i) {
    printf("%s\n", clock_render_display_mode_name(i));
  }
}

static void startup_display_mode_cmd(uint8_t argc, char* argv[]) {
  const char* mode = argv[0];
  const uint8_t num_modes = clock_render_num_display_modes();
  for (uint8_t i=0; i < num_modes; ++i) {
    if (!strcmp(clock_render_display_mode_name(i), mode)) {
      settings.startup_display_mode = i;
      printf("Startup display mode set\n");
      return;
    }
  }
  printf("Unknown display mode: %s.  Try list_display_modes\n", mode);
}

struct ConsoleCallback callbacks[] = {
  {"brightness", "Change brightness from 0-10", 1, brightness_cmd},
  {"get", "Get current settings", 0, get_cmd},
  {"list_display_modes", "List display modes", 0, list_display_modes_cmd},
  {"set_time", "Sets the time as HHMM.  example: set_time 1307.", 1, set_time_cmd},
  {"startup_display_mode", "Sets the startup display mode.", 1, startup_display_mode_cmd},
};
#define NUM_CALLBACKS (sizeof(callbacks) / sizeof(callbacks[0]))

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

static void init_default_settings(void) {
  memset(&settings, 0, sizeof(struct ClockSettings));
  settings.brightness = 3;
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
