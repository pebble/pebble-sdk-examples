#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID {0x6C, 0x97, 0x99, 0xCF, 0x34, 0x04, 0x4B, 0xFF, 0x96, 0xBA, 0xFF, 0x4C, 0xD6, 0xC2, 0xA1, 0x50}
PBL_APP_INFO_SIMPLE(MY_UUID, "Vibe Feature", "A Company", 1 /* App version */);

Window window;


void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Demo");
  window_stack_push(&window, false /* Not animated */);

  vibes_double_pulse();
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init
  };
  app_event_loop(params, &handlers);
}
