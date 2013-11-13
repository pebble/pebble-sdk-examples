/*

   Rumbletime Watch Pebble App

   Vibrates on the hour and every N minutes.

   (Where N is set by VIBE_INTERVAL_IN_MINUTES.)

 */

// Standard includes
#include "pebble.h"


// App-specific data
Window *window; // All apps must have at least one window

TextLayer *timeLayer; // The clock



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
void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {

  static char timeText[] = "00:00"; // Needs to be static because it's used by the system later.

  time_t now = time(NULL);
  struct tm * currentTime = localtime(&now);

  if (currentTime->tm_min == 0) {
    vibes_enqueue_custom_pattern(HOUR_VIBE_PATTERN);
  } else if ((currentTime->tm_min % VIBE_INTERVAL_IN_MINUTES) == 0) {
    vibes_enqueue_custom_pattern(PART_HOUR_INTERVAL_VIBE_PATTERN);
  }

  strftime(timeText, sizeof(timeText), "%R", currentTime);

  text_layer_set_text(timeLayer, timeText);

}


// Handle the start-up of the app
static void init(void) {

  // Create our app's base window
  window = window_create();
  window_stack_push(window, true);
  window_set_background_color(window, GColorBlack);

  // Init the text layer used to show the time
  // TODO: Wrap this boilerplate in a function?
  timeLayer = text_layer_create(GRect(40, 54, 144-40 /* width */, 168-54 /* height */));
  text_layer_set_text_color(timeLayer, GColorWhite);
  text_layer_set_background_color(timeLayer, GColorClear);
  text_layer_set_font(timeLayer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));

  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  handle_minute_tick(NULL, 0);

  layer_add_child(window_get_root_layer(window), text_layer_get_layer(timeLayer));

  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}

static void deinit(void) {
  window_destroy(window);
  text_layer_destroy(timeLayer);
}

// The main event/run loop for our app
int main(void) {
  init();
  app_event_loop();
  deinit();
}
