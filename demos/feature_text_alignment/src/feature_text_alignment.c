#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID {0xF2, 0xDD, 0x2F, 0x38, 0xA8, 0x10, 0x43, 0x4B, 0x82, 0x4C, 0x5F, 0x36, 0x3F, 0x53, 0xBE, 0x28}
PBL_APP_INFO_SIMPLE(MY_UUID,
		    "Alignment", "A Company",
		    0x0002 /* App version */);

Window window;

TextLayer left_layer;
TextLayer center_layer;
TextLayer right_layer;


void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Alignment");
  window_stack_push(&window, true /* Animated */);

  text_layer_init(&left_layer, GRect(0, 20, 144 /* width */, 20 /* height */));
  text_layer_set_text(&left_layer, "Left");
  text_layer_set_text_alignment(&left_layer, GTextAlignmentLeft);
  layer_add_child(&window.layer, &left_layer.layer);

  text_layer_init(&center_layer, GRect(0, 60, 144 /* width */, 20 /* height */));
  text_layer_set_text(&center_layer, "Center");
  text_layer_set_text_alignment(&center_layer, GTextAlignmentCenter);
  layer_add_child(&window.layer, &center_layer.layer);

  text_layer_init(&right_layer, GRect(0, 100, 144 /* width */, 20 /* height */));
  text_layer_set_text(&right_layer, "Right");
  text_layer_set_text_alignment(&right_layer, GTextAlignmentRight);
  layer_add_child(&window.layer, &right_layer.layer);

}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init
  };
  app_event_loop(params, &handlers);
}
