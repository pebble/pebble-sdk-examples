/*

   "Classic" Digital Watch Pebble App

 */

// Standard includes
#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID {0x54, 0xD3, 0x00, 0x8F, 0x0E, 0x46, 0x46, 0x2C, 0x99, 0x5C, 0x0D, 0x0B, 0x4E, 0x01, 0x14, 0x8C}
PBL_APP_INFO(MY_UUID, "Classio Digital", "Classio Corp.", 0x2, 0x0, INVALID_RESOURCE, APP_INFO_WATCH_FACE);


// App-specific data
Window window; // All apps must have at least one window

TextLayer timeLayer; // The clock


// Called once per second
void handle_second_tick(AppContextRef ctx, PebbleTickEvent *t) {

  (void)t;
  (void)ctx;

  static char timeText[] = "00:00:00"; // Needs to be static because it's used by the system later.

  PblTm currentTime;


  get_time(&currentTime);

  string_format_time(timeText, sizeof(timeText), "%T", &currentTime);

  text_layer_set_text(&timeLayer, timeText);

}


// Handle the start-up of the app
void handle_init_app(AppContextRef app_ctx) {

  // Create our app's base window
  window_init(&window, "Classio");
  window_stack_push(&window, true);
  window_set_background_color(&window, GColorBlack);

  // Init the text layer used to show the time
  // TODO: Wrap this boilerplate in a function?
  text_layer_init(&timeLayer, GRect(29, 54, 144-40 /* width */, 168-54 /* height */));
  text_layer_set_text_color(&timeLayer, GColorWhite);
  text_layer_set_background_color(&timeLayer, GColorClear);
  text_layer_set_font(&timeLayer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));

  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  handle_second_tick(app_ctx, NULL);

  layer_add_child(&window.layer, &timeLayer.layer);
}


// The main event/run loop for our app
void pbl_main(void *params) {
  PebbleAppHandlers handlers = {

    // Handle app start
    .init_handler = &handle_init_app,

    // Handle time updates
    .tick_info = {
      .tick_handler = &handle_second_tick,
      .tick_units = SECOND_UNIT
    }

  };
  app_event_loop(params, &handlers);
}
