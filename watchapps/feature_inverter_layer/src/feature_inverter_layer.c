#include "pebble.h"

static Window *window;

static TextLayer *text_below;
static TextLayer *text_above;
static InverterLayer *inverter_layer;

static void init() {
  window = window_create();
  window_stack_push(window, true /* Animated */);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  text_below = text_layer_create(GRect(0, 54, bounds.size.w, bounds.size.h-54 /* height */));
  text_layer_set_text(text_below, "below");
  text_layer_set_font(text_below, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_text_alignment(text_below, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_below));

  inverter_layer = inverter_layer_create(GRect(bounds.size.w/2, 0, bounds.size.w, bounds.size.h));
  layer_add_child(window_layer, inverter_layer_get_layer(inverter_layer));

  text_above = text_layer_create(GRect(0, 84, bounds.size.w, 168-84 /* height */));
  text_layer_set_text(text_above, "above");
  text_layer_set_font(text_above, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_text_alignment(text_above, GTextAlignmentCenter);
  text_layer_set_background_color(text_above, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(text_above));
}

static void deinit() {
  text_layer_destroy(text_above);
  inverter_layer_destroy(inverter_layer);
  text_layer_destroy(text_below);
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
