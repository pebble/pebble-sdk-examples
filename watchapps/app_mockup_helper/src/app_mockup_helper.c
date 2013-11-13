/*

  Helps with watch face mockups by displaying the image `mockup.png`
  as a fullscreen image in a watch app so you can see how it looks
  without writing any code.

 */

#include "pebble.h"


static Window *window;

static GBitmap *image;
static BitmapLayer *image_layer;


static void init() {
  window = window_create();
  window_stack_push(window, true /* Animated */);

  image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MOCKUP);

  Layer* window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  image_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(image_layer, image);
  layer_add_child(window_layer, bitmap_layer_get_layer(image_layer));
}

static void deinit() {
  gbitmap_destroy(image);
  bitmap_layer_destroy(image_layer);
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
