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

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID {0xDF, 0x3C, 0xF9, 0x8C, 0x36, 0xBC, 0x4E, 0x9A, 0xA0, 0x5E, 0xAC, 0x9D, 0x7D, 0xC5, 0x97, 0x7E}
PBL_APP_INFO_SIMPLE(MY_UUID, "Transparent Demo", "Demo Co.", 1 /* App version */);


Window window;

RotBmpPairContainer bitmap_container;

TextLayer text_layer; // Used as a background to help demonstrate transparency.


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

}


void handle_deinit(AppContextRef ctx) {
  (void)ctx;

  rotbmp_pair_deinit_container(&bitmap_container);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit
  };
  app_event_loop(params, &handlers);
}
