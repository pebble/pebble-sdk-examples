#include "pebble.h"

static Window *window;

static TextLayer *left_layer;
static TextLayer *center_layer;
static TextLayer *right_layer;

static void init(void) {
  window = window_create();
  window_stack_push(window, true /* Animated */);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  left_layer = text_layer_create(GRect(0, 20, bounds.size.w /* width */, 28 /* height */));
  text_layer_set_text(left_layer, "Left");
  text_layer_set_font(left_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(left_layer, GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(left_layer));

  center_layer = text_layer_create(GRect(0, 60, bounds.size.w /* width */, 28 /* height */));
  text_layer_set_text(center_layer, "Center");
  text_layer_set_font(center_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(center_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(center_layer));

  right_layer = text_layer_create(GRect(0, 100, bounds.size.w /* width */, 28 /* height */));
  text_layer_set_text(right_layer, "Right");
  text_layer_set_font(right_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(right_layer, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(right_layer));
}

static void deinit(void) {
  text_layer_destroy(left_layer);
  text_layer_destroy(center_layer);
  text_layer_destroy(right_layer);
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
