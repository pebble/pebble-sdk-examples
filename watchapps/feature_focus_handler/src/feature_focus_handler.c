#include <pebble.h>
#include <pebble_fonts.h>
#include <stdio.h>
#include <time.h>

#define BUF_LEN 32

static char in_buf[BUF_LEN];
static char out_buf[BUF_LEN];
static TextLayer *in_time;
static TextLayer *out_time;

static Window *window;

static void focus_handler(bool in_focus) {
  static char time_txt[32];
  time_t t = time(NULL);
  struct tm *lt = localtime(&t);
  strftime(time_txt, 32, "%H:%M:%S", lt);

  if (in_focus) {
       snprintf(in_buf, BUF_LEN, "Last in focus:\n%s", time_txt);
       layer_mark_dirty(text_layer_get_layer(in_time));
  }
  else {
       snprintf(out_buf, BUF_LEN, "Last out focus:\n%s", time_txt);
       layer_mark_dirty(text_layer_get_layer(out_time));
  }
}

static void init(void) {
  window = window_create();
  const bool animated = true;
  window_stack_push(window, animated);
  Layer *window_layer = window_get_root_layer(window);
  GRect frame = layer_get_frame(window_layer);

  in_time = text_layer_create(frame);
  text_layer_set_text(in_time, in_buf);
  text_layer_set_font(in_time, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(in_time));
 
  frame.origin.y = 50;
  out_time = text_layer_create(frame);
  text_layer_set_text(out_time, out_buf);
  text_layer_set_font(out_time, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(out_time));

  snprintf(in_buf, BUF_LEN, "Last in focus:\nNever");
  snprintf(out_buf, BUF_LEN, "Last out focus:\nNever");
  app_focus_service_subscribe(focus_handler);
}

static void deinit(void) {
  app_focus_service_unsubscribe();
  text_layer_destroy(in_time);
  text_layer_destroy(out_time);
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
