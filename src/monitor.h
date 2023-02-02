#ifndef MONITOR_H
#define MONITOR_H

#include <inttypes.h>

// usage:
// struct Monitor m;
//
// int main() {
//   monitor_init(&m);
//   while (1) {
//      render_frame();
//      monitor_frame(&m)
//      // m.last_fps is periodically updated with an fps value
//   }
//   ... 
//}

struct Monitor {
  uint32_t last_fps;
  uint32_t fps_idx;
  uint32_t last_fps_sample;
};

void monitor_init(struct Monitor* m);

void monitor_frame(struct Monitor* m);

#endif
