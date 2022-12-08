#include "clock_settings.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include <stdio.h>
#include <string.h>

#define FLASH_OFFSET (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)
#define FLASH_ADDRESS ((uint8_t*)(XIP_BASE + FLASH_OFFSET))

uint32_t calc_checksum(const struct ClockSettings* cs) {
  uint8_t* start = ((uint8_t*)cs) + sizeof(uint32_t);
  size_t len = sizeof(struct ClockSettings) - sizeof(uint32_t);
  uint32_t sum = 0;
  for (size_t i=0; i<len; ++i) {
    sum += start[i];
  }
  return sum;
}

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

static void init_default_settings(struct ClockSettings* cs) {
  memset(cs, 0, sizeof(struct ClockSettings));
  cs->brightness = 3;
}

void clock_settings_init(struct ClockSettings* cs) {
  struct ClockSettings flash_settings;
  memcpy(&flash_settings, FLASH_ADDRESS, sizeof(struct ClockSettings));
  if (!validate_settings(&flash_settings)) {
    init_default_settings(cs);
  } else {
    memcpy(cs, &flash_settings, sizeof(struct ClockSettings));
  }
}

void clock_settings_save(const struct ClockSettings* cs) {
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
