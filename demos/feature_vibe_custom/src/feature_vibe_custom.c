#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID {0x97, 0x2F, 0xF7, 0x4D, 0x3A, 0x5A, 0x45, 0xED, 0x81, 0x1E, 0x09, 0x55, 0xCD, 0xE9, 0x3B, 0x03}
PBL_APP_INFO_SIMPLE(MY_UUID, "Vibe Custom", "A Company", 1 /* App version */);

Window window;

/*

 Each duration corresponds to alternating on/off periods, starting on.

 An even number of segments means the last period will be the
 minimum time between the end of this pattern and the start of the
 next queued pattern (if any). If odd, a spacing time will be used in
 its place.

 */

const VibePattern custom_pattern = {
  .durations = (uint32_t []) {100, 300, 300, 300, 100, 300},
  .num_segments = 6
};

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Demo");
  window_stack_push(&window, false /* Not animated */);

  vibes_enqueue_custom_pattern(custom_pattern);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init
  };
  app_event_loop(params, &handlers);
}
