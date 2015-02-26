#include "pebble.h"

#define TIMER_INTERVAL_MS 50

static const GPathInfo SQUARE_POINTS = {
  4,
  (GPoint []) {
    {-25, -25},
    {-25,  25},
    { 25,  25},
    { 25, -25}
  }
};

static Window *s_main_window;
static Layer *s_square_layer;
static GPath *s_square_path;

static int s_angle;

static void update_square_layer(Layer *layer, GContext* ctx) {
  gpath_rotate_to(s_square_path, (TRIG_MAX_ANGLE / 360) * s_angle);
  s_angle = (s_angle + 5) % 360;
#ifdef PBL_COLOR
  graphics_context_set_stroke_color(ctx, GColorDukeBlue);
#else
  graphics_context_set_stroke_color(ctx, GColorBlack);
#endif
  gpath_draw_outline(ctx, s_square_path);
}

static void timer_callback(void *context) {
  layer_mark_dirty(s_square_layer);
  app_timer_register(TIMER_INTERVAL_MS, timer_callback, NULL);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_square_layer = layer_create(bounds);
  layer_set_update_proc(s_square_layer, update_square_layer);
  layer_add_child(window_layer, s_square_layer);

  s_square_path = gpath_create(&SQUARE_POINTS);
  gpath_move_to(s_square_path, grect_center_point(&bounds));
}

static void main_window_unload(Window *window) {
  gpath_destroy(s_square_path);
  layer_destroy(s_square_layer);
}

static void init(void) {
  s_main_window = window_create();
#ifdef PBL_COLOR
  window_set_background_color(s_main_window, GColorJaegerGreen);
#endif
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);

  app_timer_register(TIMER_INTERVAL_MS, timer_callback, NULL);
}

static void deinit(void) {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
