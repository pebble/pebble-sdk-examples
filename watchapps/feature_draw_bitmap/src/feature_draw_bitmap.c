/*
 * The original source image is from:
 *   <http://openclipart.org/detail/26728/aiga-litter-disposal-by-anonymous>
 *
 * The source image was converted from an SVG into a RGB bitmap using
 * Inkscape. It has no transparency and uses only black and white as
 * colors.
 */

#include "pebble.h"

static Window *s_main_window;
static Layer *s_image_layer;
static GBitmap *s_image;

static void layer_update_callback(Layer *layer, GContext* ctx) {
  // We make sure the dimensions of the GRect to draw into
  // are equal to the size of the bitmap--otherwise the image
  // will automatically tile. Which might be what *you* want.

#ifdef PBL_PLATFORM_BASALT
  GSize image_size = gbitmap_get_bounds(s_image).size;
#else 
  GSize image_size = s_image->bounds.size;
#endif

  graphics_draw_bitmap_in_rect(ctx, s_image, GRect(5, 5, image_size.w, image_size.h));
  graphics_draw_bitmap_in_rect(ctx, s_image, GRect(80, 60, image_size.w, image_size.h));
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(s_main_window);
  GRect bounds = layer_get_frame(window_layer);

  s_image_layer = layer_create(bounds);
  layer_set_update_proc(s_image_layer, layer_update_callback);
  layer_add_child(window_layer, s_image_layer);

  s_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NO_LITTER);
}

static void main_window_unload(Window *window) {
  gbitmap_destroy(s_image);
  layer_destroy(s_image_layer);
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
