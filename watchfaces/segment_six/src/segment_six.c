/*

  Segment Six watch.

 */

#include "pebble.h"

Window *window;

Layer *minute_display_layer;
Layer *hour_display_layer;


const GPathInfo MINUTE_SEGMENT_PATH_POINTS = {
  3,
  (GPoint []) {
    {0, 0},
    {-8, -80}, // 80 = radius + fudge; 8 = 80*tan(6 degrees); 6 degrees per minute;
    {8,  -80},
  }
};

static GPath *minute_segment_path;


const GPathInfo HOUR_SEGMENT_PATH_POINTS = {
  3,
  (GPoint []) {
    {0, 0},
    {-13, -50}, // 50 = radius + fudge; _ = 50*tan(15 degrees); 30 degrees per hour;
    {13,  -50},
  }
};

static GPath *hour_segment_path;



static void minute_display_layer_update_callback(Layer *layer, GContext* ctx) {

  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  unsigned int angle = t->tm_min * 6;

  gpath_rotate_to(minute_segment_path, (TRIG_MAX_ANGLE / 360) * angle);

  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);

  graphics_context_set_fill_color(ctx, GColorWhite);

  graphics_fill_circle(ctx, center, 77);

  graphics_context_set_fill_color(ctx, GColorBlack);

  // Note: I had intended to use the `GCompOpAssignInverted` mode here
  //       but it appears it's ignored for path/shape drawing.

  gpath_draw_filled(ctx, minute_segment_path);

  graphics_fill_circle(ctx, center, 52);
}



static void hour_display_layer_update_callback(Layer *layer, GContext* ctx) {


  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  // Because of how we're drawing the segments I'm not currently
  // accounting for per-minute moving of the hour hand.
  //unsigned int angle = (((t.tm_hour % 12) * 30) + (t.tm_min/2));
  unsigned int angle = (t->tm_hour % 12) * 30;


  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);

  graphics_context_set_fill_color(ctx, GColorWhite);

  graphics_fill_circle(ctx, center, 48);


  for (unsigned int i = 0; i < 360; i+=15) {

    if ((i != angle) && (i != (angle + 15)) && (i != ((angle-15+360) % 360)) ) {

      gpath_rotate_to(hour_segment_path, (TRIG_MAX_ANGLE / 360) * i);

      graphics_context_set_fill_color(ctx, GColorBlack);

      gpath_draw_filled(ctx, hour_segment_path);

    }

  }

  // Due to (I assume) rounding/precision errors with rotation
  // of paths there's a few stray pixels left behind after the
  // above method of drawing segments.

  // This draws a circle over almost all the stray pixels--at the
  // cost of not quite matching the mock-up.
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, center, 5);

  // But there's still two stray pixels left--and enlarging the cover
  // up circle to fit them starts to be obvious so we touch them up
  // individually.
  graphics_context_set_stroke_color(ctx, GColorBlack);

  if ((angle != 0) && (angle != 330)) {
    graphics_draw_pixel(ctx, GPoint(71, 77));
    graphics_draw_pixel(ctx, GPoint(71, 78));
  }

}




static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {

  layer_mark_dirty(minute_display_layer);
  layer_mark_dirty(hour_display_layer);
}


static void init(void) {

  window = window_create();
  window_set_background_color(window, GColorBlack);
  window_stack_push(window, true);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Init the layer for the minute display
  minute_display_layer = layer_create(bounds);
  layer_set_update_proc(minute_display_layer, minute_display_layer_update_callback);
  layer_add_child(window_layer, minute_display_layer);


  // Init the minute segment path
  minute_segment_path = gpath_create(&MINUTE_SEGMENT_PATH_POINTS);
  gpath_move_to(minute_segment_path, grect_center_point(&bounds));


  // Init the layer for the hour display
  hour_display_layer = layer_create(bounds);
  layer_set_update_proc(hour_display_layer, hour_display_layer_update_callback);
  layer_add_child(window_layer, hour_display_layer);


  // Init the hour segment path
  hour_segment_path = gpath_create(&HOUR_SEGMENT_PATH_POINTS);
  gpath_move_to(hour_segment_path, grect_center_point(&bounds));

  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}

static void deinit(void) {
  gpath_destroy(minute_segment_path);
  gpath_destroy(hour_segment_path);

  tick_timer_service_unsubscribe();
  window_destroy(window);
  layer_destroy(minute_display_layer);
  layer_destroy(hour_display_layer);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
