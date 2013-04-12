/*

   Demonstrate how the timer feature works with animation.

 */

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID {0xFE, 0xCC, 0x5F, 0x22, 0x37, 0xBF, 0x4B, 0x42, 0xA8, 0x4D, 0x5B, 0x6B, 0x12, 0xA4, 0xED, 0x09}
PBL_APP_INFO_SIMPLE(MY_UUID, "Timer Animation", "Pebble Technology", 1 /* App version */ );

Window window;

TextLayer text_layer;

Layer square_layer;

// Can be used to cancel timer via `app_timer_cancel_event()`
AppTimerHandle timer_handle;

// Can be used to distinguish between multiple timers in your app
#define COOKIE_MY_TIMER 1


const GPathInfo SQUARE_POINTS = {
  4,
  (GPoint []) {
    {-25, -25},
    {-25,  25},
    { 25,  25},
    { 25, -25}
  }
};

GPath square_path;


void update_square_layer(Layer *me, GContext* ctx) {
  (void)me;

  static unsigned int angle = 0;

  gpath_rotate_to(&square_path, (TRIG_MAX_ANGLE / 360) * angle);

  angle = (angle + 5) % 360;

  graphics_context_set_stroke_color(ctx, GColorBlack);
  gpath_draw_outline(ctx, &square_path);
}


void handle_timer(AppContextRef ctx, AppTimerHandle handle, uint32_t cookie) {
  (void)ctx;
  (void)handle;

  if (cookie == COOKIE_MY_TIMER) {
    layer_mark_dirty(&square_layer);
    timer_handle = app_timer_send_event(ctx, 50 /* milliseconds */, COOKIE_MY_TIMER);
  }

}


void handle_init(AppContextRef ctx) {
  window_init(&window, "Demo");
  window_stack_push(&window, true /* Animated */);

  layer_init(&square_layer, GRect(0,0, window.layer.frame.size.w, window.layer.frame.size.h));
  square_layer.update_proc = update_square_layer;
  layer_add_child(&window.layer, &square_layer);

  gpath_init(&square_path, &SQUARE_POINTS);
  gpath_move_to(&square_path, grect_center_point(&square_layer.frame));

  timer_handle = app_timer_send_event(ctx, 50 /* milliseconds */, COOKIE_MY_TIMER);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .timer_handler = &handle_timer
  };
  app_event_loop(params, &handlers);
}
