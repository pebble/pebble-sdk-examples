#include <pebble.h>

#define BUF_LEN 32

static char s_in_buf[BUF_LEN];
static char s_out_buf[BUF_LEN];
static TextLayer *s_in_time;
static TextLayer *s_out_time;
static Window *s_main_window;

static void focus_handler(bool in_focus) {
  static char time_str[BUF_LEN];
  time_t t = time(NULL);
  struct tm *lt = localtime(&t);
  strftime(time_str, BUF_LEN, "%H:%M:%S", lt);

  if (in_focus) {
    snprintf(s_in_buf, sizeof(s_in_buf), "Last in focus:\n%s", time_str);
    layer_mark_dirty(text_layer_get_layer(s_in_time));
  }
  else {
    snprintf(s_out_buf, sizeof(s_out_buf), "Last out focus:\n%s", time_str);
    layer_mark_dirty(text_layer_get_layer(s_out_time));
  }
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(s_main_window);
  GRect bounds = layer_get_frame(window_layer);

  s_in_time = text_layer_create(bounds);
  text_layer_set_text(s_in_time, s_in_buf);
  text_layer_set_font(s_in_time, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(s_in_time));
 
  bounds.origin.y = 50;

  s_out_time = text_layer_create(bounds);
  text_layer_set_text(s_out_time, s_out_buf);
  text_layer_set_font(s_out_time, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(s_out_time));

  snprintf(s_in_buf, sizeof(s_in_buf), "Last in focus:\nNever");
  snprintf(s_out_buf, sizeof(s_out_buf), "Last out focus:\nNever");

  app_focus_service_subscribe(focus_handler);
}

static void main_window_unload(Window *window) {
  app_focus_service_unsubscribe();
  text_layer_destroy(s_in_time);
  text_layer_destroy(s_out_time);
}

static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
}

static void deinit(void) {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
