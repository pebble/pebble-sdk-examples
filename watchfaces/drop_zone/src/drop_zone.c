#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;

static PropertyAnimation *s_up_animation, *s_down_animation;

#define UPPER_TO_RECT GRect(0, 5, 144, 163)
#define LOWER_TO_RECT GRect(0, 112, 144, 56)

#define ANIMATION_DURATION_IN_MS 1500

static void init_animations() {
  Layer *layer = text_layer_get_layer(s_time_layer);

  s_up_animation = property_animation_create_layer_frame(layer, &LOWER_TO_RECT, &UPPER_TO_RECT);
  animation_set_duration((Animation*) s_up_animation, ANIMATION_DURATION_IN_MS);
  animation_set_curve((Animation*) s_up_animation, AnimationCurveEaseOut);

  s_down_animation = property_animation_create_layer_frame(layer, &UPPER_TO_RECT, &LOWER_TO_RECT);
  animation_set_duration((Animation*) s_down_animation, ANIMATION_DURATION_IN_MS);
  animation_set_curve((Animation*) s_down_animation, AnimationCurveEaseOut);
}

static void schedule_animation(struct tm *tick_time) {
  static bool s_use_upper_to_rect = false;

  int32_t animation_delay_required_in_ms = 60000 - (tick_time->tm_sec * 1000) - (ANIMATION_DURATION_IN_MS - 100);

  if (animation_delay_required_in_ms < 0) {
    // In the hopefully rare case that we don't have time to schedule the animation
    // before the value is due to change we just let it change in place.
    return;
  }

  if (s_use_upper_to_rect) {
    animation_set_delay((Animation*) s_up_animation, animation_delay_required_in_ms);
    animation_schedule((Animation*) s_up_animation);
  } else {
    animation_set_delay((Animation*) s_down_animation, animation_delay_required_in_ms);
    animation_schedule((Animation*) s_down_animation);
  }

  s_use_upper_to_rect = !s_use_upper_to_rect;
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  // Need to be static because it's used by the system later.
  static char s_time_text[] = "00:00";

  strftime(s_time_text, sizeof(s_time_text), "%R", tick_time);
  text_layer_set_text(s_time_layer, s_time_text);

  schedule_animation(tick_time);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  s_time_layer = text_layer_create(UPPER_TO_RECT);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_font(s_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_CONDENSED_SUBSET_40)));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));

  init_animations();
  tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);
}

static void main_window_unload(Window *window) {
  property_animation_destroy(s_up_animation);
  property_animation_destroy(s_down_animation);
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
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
