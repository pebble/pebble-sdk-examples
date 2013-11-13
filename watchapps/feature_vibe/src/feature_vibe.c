#include "pebble.h"


static Window *window;

static TextLayer *text_layer;

void up_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  vibes_short_pulse();
}

void down_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  vibes_long_pulse();
}

void select_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  vibes_double_pulse();
}

void click_config_provider(void *context) {
  const uint16_t repeat_interval_ms = 1000;
  window_single_repeating_click_subscribe(BUTTON_ID_SELECT, repeat_interval_ms, select_single_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_UP, repeat_interval_ms, up_single_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, repeat_interval_ms, down_single_click_handler);
}

int main(void) {
  window = window_create();
  window_stack_push(window, true /* Animated */ );

  window_set_click_config_provider(window, click_config_provider);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  text_layer = text_layer_create(bounds);
  text_layer_set_text(text_layer, "Press buttons for different vibes. Hold for repeating.");
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));

  app_event_loop();

  text_layer_destroy(text_layer);
  window_destroy(window);
}
