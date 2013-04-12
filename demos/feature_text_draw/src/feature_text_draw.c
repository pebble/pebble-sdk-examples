#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID {0xBC, 0xB0, 0xC2, 0x6D, 0xCE, 0xE5, 0x48, 0x8D, 0x85, 0xF4, 0xAD, 0x2D, 0x33, 0xAA, 0xF1, 0xFF}
PBL_APP_INFO(MY_UUID,
	     "Text Draw Demo", "A Company",
	     3, 0, /* App version */
	     DEFAULT_MENU_ICON,
	     APP_INFO_STANDARD_APP);

Window window;

Layer layer;


void update_layer_callback(Layer *me, GContext* ctx) {
  (void)me;

  graphics_context_set_text_color(ctx, GColorBlack);

  graphics_text_draw(ctx,
		     "Text here.",
		     fonts_get_system_font(FONT_KEY_FONT_FALLBACK),
		     GRect(5, 5, 144-10, 100),
		     GTextOverflowModeWordWrap,
		     GTextAlignmentLeft,
		     NULL);

  graphics_text_draw(ctx,
		     "And text here as well.",
		     fonts_get_system_font(FONT_KEY_FONT_FALLBACK),
		     GRect(90, 100, 144-95, 60),
		     GTextOverflowModeWordWrap,
		     GTextAlignmentRight,
		     NULL);
}


void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Text Draw");
  window_stack_push(&window, true /* Animated */);

  layer_init(&layer, window.layer.frame);
  layer.update_proc = update_layer_callback;
  layer_add_child(&window.layer, &layer);

  layer_mark_dirty(&layer);

}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init
  };
  app_event_loop(params, &handlers);
}
