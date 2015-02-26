#include "pebble.h"

// Each duration corresponds to alternating on/off periods, starting on.
static const VibePattern CUSTOM_PATTERN = {
  .durations = (uint32_t[]) {100, 300, 300, 300, 100, 300},
  .num_segments = 6
};

static Window *s_main_window;
static TextLayer *s_text_layer;

static void select_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  vibes_enqueue_custom_pattern(CUSTOM_PATTERN);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_DOWN, select_single_click_handler);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  
  s_text_layer = text_layer_create(bounds);
  text_layer_set_text(s_text_layer, "Press the down button for a custom vibration.");
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
