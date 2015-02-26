/*
 * Rumbletime Watch Pebble App
 * Vibrates on the hour and every N minutes.
 * (Where N is set by VIBE_INTERVAL_IN_MINUTES.)
 */

#include "pebble.h"

#define VIBE_INTERVAL_IN_MINUTES 15

static const VibePattern HOUR_VIBE_PATTERN = {
  .durations = (uint32_t[]) {50, 200, 50, 200, 50, 200, 50, 200},
  .num_segments = 8
};

static const VibePattern PART_HOUR_INTERVAL_VIBE_PATTERN = {
  .durations = (uint32_t[]) {50, 200, 50, 200},
  .num_segments = 4
};

static Window *s_main_window; 
static TextLayer *s_time_layer; 

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  // Needs to be static because it's used by the system later.
  static char s_time_text[] = "00:00"; 
  strftime(s_time_text, sizeof(s_time_text), "%R", tick_time);
  text_layer_set_text(s_time_layer, s_time_text);
  
  if (tick_time->tm_min == 0) {
    vibes_enqueue_custom_pattern(HOUR_VIBE_PATTERN);
  } else if ((tick_time->tm_min % VIBE_INTERVAL_IN_MINUTES) == 0) {
    vibes_enqueue_custom_pattern(PART_HOUR_INTERVAL_VIBE_PATTERN);
  }
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  s_time_layer = text_layer_create(GRect(40, 54, 104, 114));
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_time_layer);
}

static void init() {
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);

  // Ensures time is displayed immediately
  time_t now = time(NULL);
  struct tm * time_now = localtime(&now);
  handle_minute_tick(time_now, MINUTE_UNIT);

  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}
