#include "pebble.h"

static Window *window;

static TextLayer *text_layer;

static void init() {
  window = window_create();
  window_stack_push(window, true /* Animated */);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  text_layer = text_layer_create((GRect){ .origin = { 0, 30 }, .size = bounds.size });
  text_layer_set_text(text_layer, clock_is_24h_style() ? "Mode:\n24" : "Mode:\n12");
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void deinit() {
  text_layer_destroy(text_layer);
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
