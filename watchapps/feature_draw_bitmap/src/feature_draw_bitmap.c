/*

   The original source image is from:

      <http://openclipart.org/detail/26728/aiga-litter-disposal-by-anonymous>

   The source image was converted from an SVG into a RGB bitmap using
   Inkscape. It has no transparency and uses only black and white as
   colors.

*/

#include "pebble.h"

static Window *window;

static Layer *layer;

static GBitmap *image;

static void layer_update_callback(Layer *me, GContext* ctx) {
  // We make sure the dimensions of the GRect to draw into
  // are equal to the size of the bitmap--otherwise the image
  // will automatically tile. Which might be what *you* want.

  GRect bounds = image->bounds;

  graphics_draw_bitmap_in_rect(ctx, image, (GRect) { .origin = { 5, 5 }, .size = bounds.size });

  graphics_draw_bitmap_in_rect(ctx, image, (GRect) { .origin = { 80, 60 }, .size = bounds.size });
}

int main(void) {
  window = window_create();
  window_stack_push(window, true /* Animated */);

  // Init the layer for display the image
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  layer = layer_create(bounds);
  layer_set_update_proc(layer, layer_update_callback);
  layer_add_child(window_layer, layer);

  image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NO_LITTER);

  app_event_loop();

  gbitmap_destroy(image);

  window_destroy(window);
  layer_destroy(layer);
}
