#include "pebble.h"

static Window *window;

static TextLayer *text_layer;

/*

 Each duration corresponds to alternating on/off periods, starting on.

 An even number of segments means the last period will be the
 minimum time between the end of this pattern and the start of the
 next queued pattern (if any). If odd, a spacing time will be used in
 its place.

 */

static const VibePattern custom_pattern = {
  .durations = (uint32_t []) {100, 300, 300, 300, 100, 300},
  .num_segments = 6
};

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  vibes_enqueue_custom_pattern(custom_pattern);
}

void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) select_single_click_handler);
}

int main(void) {
  window = window_create();
  window_stack_push(window, false /* Not animated */);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  text_layer = text_layer_create(bounds);
  text_layer_set_text(text_layer, "Press the down button for a custom vibration.");
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));

  window_set_click_config_provider(window, click_config_provider);

  vibes_enqueue_custom_pattern(custom_pattern);

  app_event_loop();

  window_destroy(window);
}
