/*

   Demonstrate how the timer feature works with animation.

 */

#include "pebble.h"

static Window *window;

static TextLayer *text_layer;

static Layer *square_layer;

// Timers can be canceled with `app_timer_cancel()`
static AppTimer *timer;

static const GPathInfo SQUARE_POINTS = {
  4,
  (GPoint []) {
    {-25, -25},
    {-25,  25},
    { 25,  25},
    { 25, -25}
  }
};

static GPath *square_path;

static void update_square_layer(Layer *layer, GContext* ctx) {
  static unsigned int angle = 0;

  gpath_rotate_to(square_path, (TRIG_MAX_ANGLE / 360) * angle);

  angle = (angle + 5) % 360;

  graphics_context_set_stroke_color(ctx, GColorBlack);
  gpath_draw_outline(ctx, square_path);
}


static void timer_callback(void *context) {
  layer_mark_dirty(square_layer);

  const uint32_t timeout_ms = 50;
  timer = app_timer_register(timeout_ms, timer_callback, NULL);
}

static void init(void) {
  window = window_create();

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  square_layer = layer_create(bounds);
  layer_set_update_proc(square_layer, update_square_layer);
  layer_add_child(window_layer, square_layer);

  square_path = gpath_create(&SQUARE_POINTS);
  gpath_move_to(square_path, grect_center_point(&bounds));

  const bool animated = true;
  window_stack_push(window, animated);

  const uint32_t timeout_ms = 50;
  timer = app_timer_register(timeout_ms, timer_callback, NULL);
}

static void deinit(void) {
  gpath_destroy(square_path);

  layer_destroy(square_layer);
  window_destroy(window);
}

int main(void) {
  init();

  app_event_loop();

  deinit();
}
