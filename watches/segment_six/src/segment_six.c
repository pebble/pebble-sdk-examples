/*

  Segment Six watch.

 */

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID {0xE1, 0x72, 0xD7, 0x10, 0xE1, 0x69, 0x49, 0x24, 0xA8, 0xB8, 0x76, 0xC3, 0x2F, 0xAA, 0x7A, 0x66}
PBL_APP_INFO(MY_UUID, "Segment Six", "Pebble Technology", 0x2, 0x0, RESOURCE_ID_IMAGE_MENU_ICON, APP_INFO_WATCH_FACE);

Window window;

Layer minute_display_layer;
Layer hour_display_layer;


const GPathInfo MINUTE_SEGMENT_PATH_POINTS = {
  3,
  (GPoint []) {
    {0, 0},
    {-8, -80}, // 80 = radius + fudge; 8 = 80*tan(6 degrees); 6 degrees per minute;
    {8,  -80},
  }
};

GPath minute_segment_path;


const GPathInfo HOUR_SEGMENT_PATH_POINTS = {
  3,
  (GPoint []) {
    {0, 0},
    {-13, -50}, // 50 = radius + fudge; _ = 50*tan(15 degrees); 30 degrees per hour;
    {13,  -50},
  }
};

GPath hour_segment_path;



void minute_display_layer_update_callback(Layer *me, GContext* ctx) {
  (void)me;


  PblTm t;

  get_time(&t);

  unsigned int angle = t.tm_min * 6;

  gpath_rotate_to(&minute_segment_path, (TRIG_MAX_ANGLE / 360) * angle);


  GPoint center = grect_center_point(&me->frame);

  graphics_context_set_fill_color(ctx, GColorWhite);

  graphics_fill_circle(ctx, center, 77);

  graphics_context_set_fill_color(ctx, GColorBlack);

  // Note: I had intended to use the `GCompOpAssignInverted` mode here
  //       but it appears it's ignored for path/shape drawing.

  gpath_draw_filled(ctx, &minute_segment_path);

  graphics_fill_circle(ctx, center, 52);
}



void hour_display_layer_update_callback(Layer *me, GContext* ctx) {
  (void)me;


  PblTm t;

  get_time(&t);

  // Because of how we're drawing the segments I'm not currently
  // accounting for per-minute moving of the hour hand.
  //unsigned int angle = (((t.tm_hour % 12) * 30) + (t.tm_min/2));
  unsigned int angle = (t.tm_hour % 12) * 30;


  GPoint center = grect_center_point(&me->frame);

  graphics_context_set_fill_color(ctx, GColorWhite);

  graphics_fill_circle(ctx, center, 48);


  for (unsigned int i = 0; i < 360; i+=15) {

    if ((i != angle) && (i != (angle + 15)) && (i != ((angle-15+360) % 360)) ) {

      gpath_rotate_to(&hour_segment_path, (TRIG_MAX_ANGLE / 360) * i);

      graphics_context_set_fill_color(ctx, GColorBlack);

      gpath_draw_filled(ctx, &hour_segment_path);

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




void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {

  (void)t; // TODO: Pass the time direct to the layers?
  (void)ctx;

  layer_mark_dirty(&minute_display_layer);
  layer_mark_dirty(&hour_display_layer);
}


void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Segment Six watch");
  window_stack_push(&window, true);
  window_set_background_color(&window, GColorBlack);


  // Init the layer for the minute display
  layer_init(&minute_display_layer, window.layer.frame);
  minute_display_layer.update_proc = &minute_display_layer_update_callback;
  layer_add_child(&window.layer, &minute_display_layer);


  // Init the minute segment path
  gpath_init(&minute_segment_path, &MINUTE_SEGMENT_PATH_POINTS);
  gpath_move_to(&minute_segment_path, grect_center_point(&minute_display_layer.frame));


  // Init the layer for the hour display
  layer_init(&hour_display_layer, window.layer.frame);
  hour_display_layer.update_proc = &hour_display_layer_update_callback;
  layer_add_child(&window.layer, &hour_display_layer);


  // Init the hour segment path
  gpath_init(&hour_segment_path, &HOUR_SEGMENT_PATH_POINTS);
  gpath_move_to(&hour_segment_path, grect_center_point(&hour_display_layer.frame));


}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,

    // Handle time updates
    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }

  };
  app_event_loop(params, &handlers);
}
