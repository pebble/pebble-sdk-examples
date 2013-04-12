/*

   How to use a custom non-system font.

 */

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

// The auto-generated header file with resource handle definitions
#include "resource_ids.auto.h"


#define MY_UUID {0x47, 0xD2, 0xFD, 0x2A, 0x78, 0x09, 0x4C, 0xFE, 0x91, 0x44, 0x08, 0x17, 0x33, 0x53, 0xAA, 0xAD}
PBL_APP_INFO_SIMPLE(MY_UUID, "hello world", "Some Co", 1 /* App version */);


Window window;
TextLayer text_layer;


void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Demo");
  window_stack_push(&window, true /* Animated */);

  // If you neglect to call this, all `resource_get_handle()` requests
  // will return NULL.
  resource_init_current_app(&FONT_DEMO_RESOURCES);

  GFont custom_font = \
    fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_OSP_DIN_44));

  text_layer_init(&text_layer, window.layer.frame);
  text_layer_set_text(&text_layer, "  Hello,\n  World!");
  text_layer_set_font(&text_layer, custom_font);
  layer_add_child(&window.layer, &text_layer.layer);

}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init
  };
  app_event_loop(params, &handlers);
}
