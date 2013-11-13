#include "pebble.h"

static Window *window;

static TextLayer *text_layer;

static void select_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Single Click");
}

static void select_multi_click_handler(ClickRecognizerRef recognizer, void *context) {
  const uint16_t count = click_number_of_clicks_counted(recognizer);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Multi-Click: count:%u", count);
}

static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Long-Click: START");
}

static void select_long_click_release_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Long-Click: RELEASE");
}

static void config_provider(void *context) {
  // single click / repeat-on-hold config:
  const uint16_t repeat_interval_ms = 1000;
  window_single_repeating_click_subscribe(BUTTON_ID_SELECT, repeat_interval_ms, select_single_click_handler);

  // multi click config:
  window_multi_click_subscribe(BUTTON_ID_SELECT, 2, 10, 0, true, select_multi_click_handler);

  // long click config:
  window_long_click_subscribe(BUTTON_ID_SELECT, 700, select_long_click_handler, select_long_click_release_handler);
}

int main(void) {
  window = window_create();
  window_stack_push(window, true /* Animated */);

  window_set_click_config_provider(window, (ClickConfigProvider) config_provider);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  text_layer = text_layer_create(bounds);
  text_layer_set_text(text_layer,
      "Press the select button to try out different clicks and watch your Bluetooth logs");
  text_layer_set_text_color(text_layer, GColorBlack);
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));

  app_event_loop();

  text_layer_destroy(text_layer);
  window_destroy(window);
}
