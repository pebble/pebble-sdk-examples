/*

   Demonstrate how to rotate an image with black, white and transparent sections.


   (This is identical to the `feature_image_transparent` example
   except for the rotation.)

   Note: This isn't really a practical demo for "smooth" animation as
         arbitrary image rotation is very processor intensive.


   To achieve this effect you need to use a `RotBmpPairContainer` which contains
   two bitmaps: one is black and transparent, the other is white and transparent.

   The two bitmaps are created during the build process from a RGBA format PNG file.

   The image is rotated at runtime.

   The demonstration image is a modified version of the following SVG
   exported from Inkscape:

       <http://openclipart.org/detail/48919/panda-with-bamboo-leaves-by-adam_lowe>

 */

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID {0x3F, 0x95, 0xA6, 0x02, 0xE6, 0xCF, 0x4D, 0x7C, 0xA4, 0xA5, 0x06, 0x30, 0x6E, 0xD0, 0xEE, 0xDD}
PBL_APP_INFO_SIMPLE(MY_UUID, "Rotation Demo", "Demo Co.", 1 /* App version */);


Window window;

RotBmpPairContainer bitmap_container;

TextLayer text_layer; // Used as a background to help demonstrate transparency.

int32_t current_layer_angle = 0;


void handle_timer(AppContextRef ctx, AppTimerHandle handle, uint32_t cookie) {
  (void)handle;

  // This implementation assumes only one type of timer used in the app.

  current_layer_angle = (current_layer_angle + 2) % 360;

  // This will automatically mark the layer dirty and update it.
  rotbmp_pair_layer_set_angle(&bitmap_container.layer, TRIG_MAX_ANGLE * current_layer_angle / 360);

  app_timer_send_event(ctx, 50 /* milliseconds */, cookie);

}


void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Demo");
  window_stack_push(&window, false /* Not animated */);


  // We do this to account for the offset due to the status bar
  // at the top of the app window.
  GRect layer_frame_description = layer_get_frame(&window.layer);
  layer_frame_description.origin.x = 0;
  layer_frame_description.origin.y = 0;


  // Add some background content to help demonstrate transparency.
  text_layer_init(&text_layer, layer_frame_description);
  text_layer_set_text(&text_layer, "pandamonium pandamon\n"
		                   "ium pandamonium panda\n"
		                   "monium     pandamonium\n"
		                   "pandamonium pandamon\n"
		                   "ium pandamonium panda\n"
		                   "monium     pandamonium\n"
		                   "pandamonium pandamon\n"
		                   "ium pandamonium panda\n"
		                   "monium pandamonium p\n"
		                   "andamonium pandamoni\n"
		      );
  layer_add_child(&window.layer, &text_layer.layer);


  // Display the black and white image with transparency.
  resource_init_current_app(&APP_RESOURCES);

  rotbmp_pair_init_container(RESOURCE_ID_IMAGE_PANDA_WHITE, RESOURCE_ID_IMAGE_PANDA_BLACK, &bitmap_container);

  layer_add_child(&window.layer, &bitmap_container.layer.layer);


  // Start the animation.
  app_timer_send_event(ctx, 50 /* milliseconds */, 0 /* Not using a cookie value */);

}


void handle_deinit(AppContextRef ctx) {
  (void)ctx;

  rotbmp_pair_deinit_container(&bitmap_container);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit,

    .timer_handler = &handle_timer
  };
  app_event_loop(params, &handlers);
}
