/*

   Rumbletime Watch Pebble App

   Vibrates on the hour and every N minutes.

   (Where N is set by VIBE_INTERVAL_IN_MINUTES.)

 */

// Standard includes
#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID {0x32, 0x14, 0x76, 0xB1, 0x2E, 0xC6, 0x4D, 0x03, 0x9E, 0xF8, 0x0F, 0x8C, 0xDB, 0x2F, 0xA9, 0xE7}
PBL_APP_INFO(MY_UUID, "RumbleTime", "Pebble Technology", 0x2, 0x0, INVALID_RESOURCE, APP_INFO_WATCH_FACE);


// App-specific data
Window window; // All apps must have at least one window

TextLayer timeLayer; // The clock



const unsigned int VIBE_INTERVAL_IN_MINUTES = 15;

const VibePattern HOUR_VIBE_PATTERN = {
  .durations = (uint32_t []) {50, 200, 50, 200, 50, 200, 50, 200},
  .num_segments = 8
};

const VibePattern PART_HOUR_INTERVAL_VIBE_PATTERN = {
  .durations = (uint32_t []) {50, 200, 50, 200},
  .num_segments = 4
};


// Called once per minute
void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {

  (void)t;
  (void)ctx;

  static char timeText[] = "00:00"; // Needs to be static because it's used by the system later.

  PblTm currentTime;


  get_time(&currentTime);

  if (currentTime.tm_min == 0) {
    vibes_enqueue_custom_pattern(HOUR_VIBE_PATTERN);
  } else if ((currentTime.tm_min % VIBE_INTERVAL_IN_MINUTES) == 0) {
    vibes_enqueue_custom_pattern(PART_HOUR_INTERVAL_VIBE_PATTERN);
  }

  string_format_time(timeText, sizeof(timeText), "%R", &currentTime);

  text_layer_set_text(&timeLayer, timeText);

}


// Handle the start-up of the app
void handle_init_app(AppContextRef app_ctx) {

  // Create our app's base window
  window_init(&window, "RumbleTime");
  window_stack_push(&window, true);
  window_set_background_color(&window, GColorBlack);

  // Init the text layer used to show the time
  // TODO: Wrap this boilerplate in a function?
  text_layer_init(&timeLayer, GRect(40, 54, 144-40 /* width */, 168-54 /* height */));
  text_layer_set_text_color(&timeLayer, GColorWhite);
  text_layer_set_background_color(&timeLayer, GColorClear);
  text_layer_set_font(&timeLayer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));

  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  handle_minute_tick(app_ctx, NULL);

  layer_add_child(&window.layer, &timeLayer.layer);
}


// The main event/run loop for our app
void pbl_main(void *params) {
  PebbleAppHandlers handlers = {

    // Handle app start
    .init_handler = &handle_init_app,

    // Handle time updates
    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }

  };
  app_event_loop(params, &handlers);
}
