/*
 * Demonstrate how to display an image with black, white and transparent sections.
 *
 * To achieve this effect you need to use GCompOp with two bitmaps: one is black
 * and transparent, the other is white and transparent.
 *
 * The two bitmaps are created during the build process from a RGBA format PNG file.
 *
 * The image can also be rotated at runtime.
 *
 * The demonstration image is a modified version of the following SVG
 * exported from Inkscape:
 *
 * <http://openclipart.org/detail/48919/panda-with-bamboo-leaves-by-adam_lowe>
 */

#include "pebble.h"

static Window *s_main_window;
static TextLayer *s_text_layer;
static GBitmap *s_white_bitmap, *s_black_bitmap;
static BitmapLayer *s_white_layer, *s_black_layer;

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // We do this to account for the offset due to the status bar
  // at the top of the app window.
  GRect layer_frame_description = layer_get_frame(window_layer);
  layer_frame_description.origin.x = 0;
  layer_frame_description.origin.y = 0;

  // Add some background content to help demonstrate transparency.
  s_text_layer = text_layer_create(layer_frame_description);
  text_layer_set_text(s_text_layer,
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
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));

  s_white_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PANDA_WHITE);
  s_black_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PANDA_BLACK);

  GPoint center = grect_center_point(&bounds);
#ifdef PBL_PLATFORM_BASALT
  GSize image_size = gbitmap_get_bounds(s_white_bitmap).size;
#else
  GSize image_size = s_white_bitmap->bounds.size;
#endif
  GRect image_frame = GRect(center.x, center.y, image_size.w, image_size.h);
  image_frame.origin.x -= image_size.w / 2;
  image_frame.origin.y -= image_size.h / 2;

  // Use GCompOpOr to display the white portions of the image
  s_white_layer = bitmap_layer_create(image_frame);
  bitmap_layer_set_bitmap(s_white_layer, s_white_bitmap);
  bitmap_layer_set_compositing_mode(s_white_layer, GCompOpOr);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_white_layer));

  // Use GCompOpClear to display the black portions of the image
  s_black_layer = bitmap_layer_create(image_frame);
  bitmap_layer_set_bitmap(s_black_layer, s_black_bitmap);
  bitmap_layer_set_compositing_mode(s_black_layer, GCompOpClear);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_black_layer));
}

static void main_window_unload(Window *window) {
  bitmap_layer_destroy(s_white_layer);
  bitmap_layer_destroy(s_black_layer);
  text_layer_destroy(s_text_layer);
  gbitmap_destroy(s_white_bitmap);
  gbitmap_destroy(s_black_bitmap);
}

static void init(void) {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
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
