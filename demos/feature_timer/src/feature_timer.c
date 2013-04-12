/*

   Demonstrate how the timer feature works.

 */

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID {0x65, 0x6B, 0xCA, 0x9A, 0x54, 0x00, 0x42, 0xD2, 0xAF, 0xCF, 0xE9, 0xBA, 0xE1, 0x52, 0x8D, 0xF1}
PBL_APP_INFO_SIMPLE(MY_UUID, "Timer App", "Pebble Technology", 1 /* App version */);

Window window;

TextLayer text_layer;

// Can be used to cancel timer via `app_timer_cancel_event()`
AppTimerHandle timer_handle;

// Can be used to distinguish between multiple timers in your app
#define COOKIE_MY_TIMER 1


void handle_timer(AppContextRef ctx, AppTimerHandle handle, uint32_t cookie) {
  (void)ctx;
  (void)handle;

  if (cookie == COOKIE_MY_TIMER) {
      text_layer_set_text(&text_layer, "Timer happened!");
  }

  // If you want the timer to run again you need to call `app_timer_send_event()`
  // again here.
}


void handle_init(AppContextRef ctx) {
  window_init(&window, "Demo");
  window_stack_push(&window, true /* Animated */);

  text_layer_init(&text_layer, window.layer.frame);
  text_layer_set_text(&text_layer, "Waiting for timer...");
  layer_add_child(&window.layer, &text_layer.layer);

  timer_handle = app_timer_send_event(ctx, 1500 /* milliseconds */, COOKIE_MY_TIMER);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .timer_handler = &handle_timer
  };
  app_event_loop(params, &handlers);
}
