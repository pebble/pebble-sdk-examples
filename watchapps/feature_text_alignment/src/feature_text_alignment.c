#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_left_layer;
static TextLayer *s_center_layer;
static TextLayer *s_right_layer;

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  s_left_layer = text_layer_create(GRect(0, 20, bounds.size.w, 28));
  text_layer_set_text(s_left_layer, "Left");
  text_layer_set_font(s_left_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_left_layer, GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(s_left_layer));

  s_center_layer = text_layer_create(GRect(0, 60, bounds.size.w, 28));
  text_layer_set_text(s_center_layer, "Center");
  text_layer_set_font(s_center_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_center_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_center_layer));

  s_right_layer = text_layer_create(GRect(0, 100, bounds.size.w, 28));
  text_layer_set_text(s_right_layer, "Right");
  text_layer_set_font(s_right_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_right_layer, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(s_right_layer));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_left_layer);
  text_layer_destroy(s_center_layer);
  text_layer_destroy(s_right_layer);
}

static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
