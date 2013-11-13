/*

   Demonstrate how to display an image with black, white and transparent sections.

   To achieve this effect you need to use a `RotBmpPairContainer` which contains
   two bitmaps: one is black and transparent, the other is white and transparent.

   The two bitmaps are created during the build process from a RGBA format PNG file.

   The image can also be rotated at runtime.

   The demonstration image is a modified version of the following SVG
   exported from Inkscape:

       <http://openclipart.org/detail/48919/panda-with-bamboo-leaves-by-adam_lowe>

 */

#include "pebble.h"

static Window *window;

static GBitmap *white_image;
static GBitmap *black_image;
static BitmapLayer *white_image_layer;
static BitmapLayer *black_image_layer;

static TextLayer *text_layer; // Used as a background to help demonstrate transparency.

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // We do this to account for the offset due to the status bar
  // at the top of the app window.
  GRect layer_frame_description = layer_get_frame(window_layer);
  layer_frame_description.origin.x = 0;
  layer_frame_description.origin.y = 0;

  // Add some background content to help demonstrate transparency.
  text_layer = text_layer_create(layer_frame_description);
  text_layer_set_text(text_layer,
      "pandamonium pandamon\n"
      "ium pandamonium panda\n"
      "monium     pandamonium\n"
      "pandamonium pandamon\n"
      "ium pandamonium panda\n"
      "monium     pandamonium\n"
      "pandamonium pandamon\n"
      "ium pandamonium panda\n"
      "monium pandamonium p\n"
      "andamonium pandamoni\n");
  layer_add_child(window_layer, text_layer_get_layer(text_layer));

  white_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PANDA_WHITE);
  black_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PANDA_BLACK);

  const GPoint center = grect_center_point(&bounds);
  GRect image_frame = (GRect) { .origin = center, .size = white_image->bounds.size };
  image_frame.origin.x -= white_image->bounds.size.w/2;
  image_frame.origin.y -= white_image->bounds.size.h/2;

  // Use GCompOpOr to display the white portions of the image
  white_image_layer = bitmap_layer_create(image_frame);
  bitmap_layer_set_bitmap(white_image_layer, white_image);
  bitmap_layer_set_compositing_mode(white_image_layer, GCompOpOr);
  layer_add_child(window_layer, bitmap_layer_get_layer(white_image_layer));

  // Use GCompOpClear to display the black portions of the image
  black_image_layer = bitmap_layer_create(image_frame);
  bitmap_layer_set_bitmap(black_image_layer, black_image);
  bitmap_layer_set_compositing_mode(black_image_layer, GCompOpClear);
  layer_add_child(window_layer, bitmap_layer_get_layer(black_image_layer));
}

static void window_unload(Window *window) {
  bitmap_layer_destroy(white_image_layer);
  bitmap_layer_destroy(black_image_layer);

  gbitmap_destroy(white_image);
  gbitmap_destroy(black_image);

}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload
  });
  window_stack_push(window, true /* Animated */);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
