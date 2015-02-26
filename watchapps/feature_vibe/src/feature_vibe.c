#include "pebble.h"

#define REPEAT_INTERVAL_MS 1000

static Window *s_main_window;
static TextLayer *s_text_layer;

static void up_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  vibes_short_pulse();
}

static void down_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  vibes_long_pulse();
}

static void select_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  vibes_double_pulse();
}

static void click_config_provider(void *context) {
  window_single_repeating_click_subscribe(BUTTON_ID_SELECT, REPEAT_INTERVAL_MS, select_single_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_UP, REPEAT_INTERVAL_MS, up_single_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, REPEAT_INTERVAL_MS, down_single_click_handler);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  
  s_text_layer = text_layer_create(bounds);
  text_layer_set_text(s_text_layer, "Press buttons for different vibes. Hold for repeating.");
  text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_text_layer);
}

static void init() {
  s_main_window = window_create();
  window_set_click_config_provider(s_main_window, click_config_provider);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();  
}
