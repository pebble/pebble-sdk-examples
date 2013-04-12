/*

   Demonstrate how to display a two color, black and white bitmap
   image with no transparency.

   The original source image is from:

      <http://openclipart.org/detail/26728/aiga-litter-disposal-by-anonymous>

   The source image was converted from an SVG into a RGB bitmap using
   Inkscape. It has no transparency and uses only black and white as
   colors.

 */

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID {0xC3, 0x0D, 0xBA, 0xF1, 0x5F, 0x6F, 0x4F, 0x22, 0xBA, 0xAA, 0x8C, 0x2A, 0x96, 0x8C, 0xFC, 0x28}
PBL_APP_INFO_SIMPLE(MY_UUID, "Image Demo", "Demo Co.", 1 /* App version */);

Window window;

BmpContainer image_container;


void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Demo");
  window_stack_push(&window, true /* Animated */);

  resource_init_current_app(&FEATURE_DEMO_IMAGE_RESOURCES);

  // Note: This needs to be "de-inited" in the application's
  //       deinit handler.
  bmp_init_container(RESOURCE_ID_IMAGE_NO_LITTER, &image_container);

  layer_add_child(&window.layer, &image_container.layer.layer);

}


void handle_deinit(AppContextRef ctx) {
  (void)ctx;

  // Note: Failure to de-init this here will result in instability and
  //       unable to allocate memory errors.
  bmp_deinit_container(&image_container);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit
  };
  app_event_loop(params, &handlers);
}
