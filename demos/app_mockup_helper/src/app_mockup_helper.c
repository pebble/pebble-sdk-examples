/*

  Helps with watch face mockups by displaying the image `mockup.png`
  as a fullscreen image in a watch app so you can see how it looks
  without writing any code.

 */

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID {0x6C, 0xC4, 0x96, 0x7F, 0xA0, 0x6E, 0x4A, 0x00, 0xBF, 0x46, 0x8A, 0x32, 0x6A, 0xCE, 0xAF, 0x31}
PBL_APP_INFO(MY_UUID, "Mockup Helper", "A Company", 1, 0 /* App version */, INVALID_RESOURCE, APP_INFO_WATCH_FACE);


Window window;

BmpContainer image_mockup;


void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Mockup Helper");
  window_stack_push(&window, true /* Animated */);

  resource_init_current_app(&APP_RESOURCES);

  bmp_init_container(RESOURCE_ID_IMAGE_MOCKUP, &image_mockup);

  layer_add_child(&window.layer, &image_mockup.layer.layer);
}


void handle_deinit(AppContextRef ctx) {
  (void)ctx;

  bmp_deinit_container(&image_mockup);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit
  };
  app_event_loop(params, &handlers);
}
