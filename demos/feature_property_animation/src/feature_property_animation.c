#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID {0xBB, 0xFE, 0x8A, 0xE2, 0x9E, 0x98, 0x4E, 0x66, 0xB9, 0xF8, 0x77, 0xA5, 0x5A, 0x57, 0x3F, 0x66}
PBL_APP_INFO_SIMPLE(MY_UUID,
		    "Animation Demo", "A Company",
		    0x0002 /* App version */);

Window window;

TextLayer text_layer;

PropertyAnimation prop_animation;


void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Animation Demo");
  window_stack_push(&window, false);

  text_layer_init(&text_layer, GRect(0, 0, 60, 60));
  text_layer_set_text(&text_layer, "Text!");
  layer_add_child(&window.layer, &text_layer.layer);

  GRect to_rect = GRect(84, 92, 60, 60);

  property_animation_init_layer_frame(&prop_animation, &text_layer.layer, NULL, &to_rect);

  animation_schedule(&prop_animation.animation);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init
  };
  app_event_loop(params, &handlers);
}
