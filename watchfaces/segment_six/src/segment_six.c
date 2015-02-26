#include "pebble.h"

static const GPathInfo MINUTE_SEGMENT_PATH_POINTS = {
  3,
  (GPoint[]) {
    {0, 0},
    {-8, -80}, // 80 = radius + fudge; 8 = 80*tan(6 degrees); 6 degrees per minute;
    {8,  -80},
  }
};

static const GPathInfo HOUR_SEGMENT_PATH_POINTS = {
  3,
  (GPoint[]) {
    {0, 0},
    {-13, -50}, // 50 = radius + fudge; _ = 50*tan(15 degrees); 30 degrees per hour;
    {13,  -50},
  }
};

static Window *s_main_window;
static Layer *s_minute_display_layer, *s_hour_display_layer;

static GPath *s_minute_segment_path, *s_hour_segment_path;

static void minute_display_update_proc(Layer *layer, GContext* ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  unsigned int angle = t->tm_min * 6;
  gpath_rotate_to(s_minute_segment_path, (TRIG_MAX_ANGLE / 360) * angle);

  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, center, 77);
  graphics_context_set_fill_color(ctx, GColorBlack);
  gpath_draw_filled(ctx, s_minute_segment_path);
  graphics_fill_circle(ctx, center, 52);
}

static void hour_display_update_proc(Layer *layer, GContext* ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  unsigned int angle = (t->tm_hour % 12) * 30;
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);

  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, center, 48);

  for (unsigned int i = 0; i < 360; i += 15) {
    if ((i != angle) && (i != (angle + 15)) && (i != ((angle - 15 + 360) % 360)) ) {
      gpath_rotate_to(s_hour_segment_path, (TRIG_MAX_ANGLE / 360) * i);
      graphics_context_set_fill_color(ctx, GColorBlack);
      gpath_draw_filled(ctx, s_hour_segment_path);
    }
  }

  // Stray pixels
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, center, 5);
  graphics_context_set_stroke_color(ctx, GColorBlack);
  if ((angle != 0) && (angle != 330)) {
    graphics_draw_pixel(ctx, GPoint(71, 77));
    graphics_draw_pixel(ctx, GPoint(71, 78));
  }
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(s_minute_display_layer);
  layer_mark_dirty(s_hour_display_layer);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_minute_display_layer = layer_create(bounds);
  layer_set_update_proc(s_minute_display_layer, minute_display_update_proc);
  layer_add_child(window_layer, s_minute_display_layer);

  s_minute_segment_path = gpath_create(&MINUTE_SEGMENT_PATH_POINTS);
  gpath_move_to(s_minute_segment_path, grect_center_point(&bounds));

  s_hour_display_layer = layer_create(bounds);
  layer_set_update_proc(s_hour_display_layer, hour_display_update_proc);
  layer_add_child(window_layer, s_hour_display_layer);

  s_hour_segment_path = gpath_create(&HOUR_SEGMENT_PATH_POINTS);
  gpath_move_to(s_hour_segment_path, grect_center_point(&bounds));
}

static void main_window_unload(Window *window) {
  gpath_destroy(s_minute_segment_path);
  gpath_destroy(s_hour_segment_path);

  layer_destroy(s_minute_display_layer);
  layer_destroy(s_hour_display_layer);
}

static void init() {
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}

static void deinit() {
  window_destroy(s_main_window);

  tick_timer_service_unsubscribe();
}

int main() {
  init();
  app_event_loop();
  deinit();
}
