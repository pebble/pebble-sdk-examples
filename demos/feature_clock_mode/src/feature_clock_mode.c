#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID {0x48, 0x46, 0xA5, 0x72, 0x4E, 0xB2, 0x4E, 0xAA, 0xA9, 0x49, 0x0D, 0xC4, 0xFE, 0xA7, 0x6E, 0xAB}
PBL_APP_INFO_SIMPLE(MY_UUID,
		    "12/24 Mode", "A Company",
		    0x0002 /* App version */);

Window window;

TextLayer text;

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Window Name");
  window_stack_push(&window, true /* Animated */);

  text_layer_init(&text, GRect(29, 54, 144-40 /* width */, 168-54 /* height */));

  text_layer_set_text(&text, clock_is_24h_style() ? "Mode: 24" : "Mode: 12");

  layer_add_child(&window.layer, &text.layer);

}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init
  };
  app_event_loop(params, &handlers);
}
