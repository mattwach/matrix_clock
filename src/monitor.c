#include "monitor.h"

#include "pico/stdlib.h"

#define FPS_SAMPLES 500

static inline uint32_t uptime_ms() {
  return to_ms_since_boot(get_absolute_time());
}

void monitor_init(struct Monitor* m) {
  m->fps_idx = 0;
  m->last_fps_sample = uptime_ms();
  m->last_fps = 0;
}

void monitor_frame(struct Monitor* m) {
    ++m->fps_idx;
    if (m->fps_idx == FPS_SAMPLES) {
      const uint32_t new_fps_sample = uptime_ms();
      m->last_fps = m->fps_idx * 1000 / (new_fps_sample - m->last_fps_sample);
      m->last_fps_sample = new_fps_sample;
      m->fps_idx = 0;
    }
}