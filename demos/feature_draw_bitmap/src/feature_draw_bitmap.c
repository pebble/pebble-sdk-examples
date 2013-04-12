/*

   The original source image is from:

      <http://openclipart.org/detail/26728/aiga-litter-disposal-by-anonymous>

   The source image was converted from an SVG into a RGB bitmap using
   Inkscape. It has no transparency and uses only black and white as
   colors.

*/

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID {0x2F, 0xF7, 0xFA, 0x04, 0x60, 0x11, 0x4A, 0x98, 0x8A, 0x3B, 0xA8, 0x26, 0xA4, 0xB8, 0x99, 0xF8}
PBL_APP_INFO_SIMPLE(MY_UUID,
		    "Draw Bitmap", "A Company",
		    0x002 /* App version */);

Window window;

Layer layer;

// Using this is a workaround as there's currently no other way to
// allocate storage for a bitmap--but the extra automatically created
// layer is useful for getting dimension information anyway.
BmpContainer image;


void layer_update_callback(Layer *me, GContext* ctx) {
  (void)me;
  (void)ctx;

  // We make sure the dimensions of the GRect to draw into
  // are equal to the size of the bitmap--otherwise the image
  // will automatically tile. Which might be what *you* want.

  GRect destination = layer_get_frame(&image.layer.layer);

  destination.origin.y = 5;
  destination.origin.x = 5;

  graphics_draw_bitmap_in_rect(ctx, &image.bmp, destination);


  destination.origin.x = 80;
  destination.origin.y = 60;

  graphics_draw_bitmap_in_rect(ctx, &image.bmp, destination);
}


void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Draw Bitmap");
  window_stack_push(&window, true /* Animated */);

  // Init the layer for the minute display
  layer_init(&layer, window.layer.frame);
  layer.update_proc = &layer_update_callback;
  layer_add_child(&window.layer, &layer);

  resource_init_current_app(&APP_RESOURCES);
  bmp_init_container(RESOURCE_ID_IMAGE_NO_LITTER, &image);

}


void handle_deinit(AppContextRef ctx) {
  (void)ctx;

  bmp_deinit_container(&image);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit
  };
  app_event_loop(params, &handlers);
}
