#include "pebble.h"

Window *window;
TextLayer *text_time_layer;


// There are two of these because we have to schedule the next animation
// while the other is still in motion because otherwise we cancel the
// in-progress animation.
// (Actually, with the current implementation this is probably no longer
// necessary.)
static PropertyAnimation *up_animation;
static PropertyAnimation *down_animation;


// This works around the inability to use the current GRect macro
// for constants.
#define ConstantGRect(x, y, w, h) {{(x), (y)}, {(w), (h)}}

GRect UPPER_TO_RECT = ConstantGRect(0, 5, 144, 168-5);
GRect LOWER_TO_RECT = ConstantGRect(0, 112, 144, 168-112);


#define ANIMATION_DURATION_IN_MS 1500

static void init_animations() {
  Layer *layer = text_layer_get_layer(text_time_layer);

  up_animation = property_animation_create_layer_frame(layer, &LOWER_TO_RECT, &UPPER_TO_RECT);
  animation_set_duration((Animation*) up_animation, ANIMATION_DURATION_IN_MS);
  animation_set_curve((Animation*) up_animation, AnimationCurveEaseOut);

  down_animation = property_animation_create_layer_frame(layer, &UPPER_TO_RECT, &LOWER_TO_RECT);
  animation_set_duration((Animation*) down_animation, ANIMATION_DURATION_IN_MS);
  animation_set_curve((Animation*) down_animation, AnimationCurveEaseOut);
}

static void schedule_animation(struct tm *tick_time) {

  static bool use_upper_to_rect = false;

  int32_t animation_delay_required_in_ms = 60000 - (tick_time->tm_sec * 1000) - (ANIMATION_DURATION_IN_MS - 100);

  if (animation_delay_required_in_ms < 0) {
    // In the hopefully rare case that we don't have time to schedule the animation
    // before the value is due to change we just let it change in place.
    return;
  }

  if (use_upper_to_rect) {
    animation_set_delay((Animation*) up_animation, animation_delay_required_in_ms);
    animation_schedule((Animation*) up_animation);
  } else {
    animation_set_delay((Animation*) down_animation, animation_delay_required_in_ms);
    animation_schedule((Animation*) down_animation);
  }

  use_upper_to_rect = !use_upper_to_rect;

}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {

  // Need to be static because it's used by the system later.
  static char time_text[] = "00:00";

  strftime(time_text, sizeof(time_text), "%R", tick_time);
  text_layer_set_text(text_time_layer, time_text);

  schedule_animation(tick_time);
}

static void init(void) {
  window = window_create();
  window_stack_push(window, true /* Animated */);
  window_set_background_color(window, GColorBlack);

  text_time_layer = text_layer_create(UPPER_TO_RECT);
  text_layer_set_text_color(text_time_layer, GColorWhite);
  text_layer_set_background_color(text_time_layer, GColorClear);
  text_layer_set_text_alignment(text_time_layer, GTextAlignmentCenter);
  text_layer_set_font(text_time_layer,
      fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_CONDENSED_SUBSET_40)));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_time_layer));

  init_animations();
  tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);
}

static void deinit(void) {
  property_animation_destroy(up_animation);
  property_animation_destroy(down_animation);

  text_layer_destroy(text_time_layer);
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
