#include "pebble.h"

static Window *s_main_window;
static TextLayer *s_text_layer;

static void init() {
  s_main_window = window_create();
  window_stack_push(s_main_window, true);

  Layer *window_layer = window_get_root_layer(s_main_window);
  GRect bounds = layer_get_bounds(window_layer);

  GFont custom_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_OSP_DIN_44));

  s_text_layer = text_layer_create(bounds);
  text_layer_set_text(s_text_layer, "  Hello,\n  World!");
  text_layer_set_font(s_text_layer, custom_font);
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));
}

static void deinit() {
  text_layer_destroy(s_text_layer);
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
