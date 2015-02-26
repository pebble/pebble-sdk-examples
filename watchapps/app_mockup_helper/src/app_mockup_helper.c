/*
 * Helps with watch face mockups by displaying the s_mockup_bitmap `mockup.png` as a
 * fullscreen image in a watch app so you can see how it looks without writing
 * any code.
 */

#include "pebble.h"

static Window *s_main_window;

static GBitmap *s_mockup_bitmap;
static BitmapLayer *s_mockup_layer;

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(s_main_window);
  GRect bounds = layer_get_frame(window_layer);

  s_mockup_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MOCKUP);

  s_mockup_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(s_mockup_layer, s_mockup_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_mockup_layer));
}

static void main_window_unload(Window *window) {
  gbitmap_destroy(s_mockup_bitmap);
  bitmap_layer_destroy(s_mockup_layer);
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
