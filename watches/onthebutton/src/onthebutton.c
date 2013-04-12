/*

   Customisable Watch Pebble App

   Shows how to use button functionality--as a standard app.

   Also shows adding a second window.

   Also shows how to have a full screen window.

   (Note: Doesn't currently preserve location.)

 */

// Standard includes
#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID {0xBD, 0x6A, 0x13, 0xD1, 0xE8, 0x9A, 0x49, 0x9A, 0x8D, 0x48, 0xAB, 0x36, 0xDD, 0xBA, 0x40, 0x34}
PBL_APP_INFO(MY_UUID, "On The Button", "A Company", 0x3, 0x0, DEFAULT_MENU_ICON, APP_INFO_STANDARD_APP);


// App-specific data
Window window; // All apps must have at least one window

TextLayer timeLayer; // The clock

GRect clockFrame = {.origin = {.x = 29, .y = 54}, .size = {.w = 144-40, .h = 168-54}};


#define EDIT_MODE_OFF 0
#define EDIT_MODE_X   1
#define EDIT_MODE_Y   2
#define EDIT_MODE_MAX 3

TextLayer editModeDisplayLayer;
short editMode = EDIT_MODE_OFF;


// Called once per second
void handle_second_tick(AppContextRef ctx, PebbleTickEvent *t) {

  (void)ctx;
  (void)t;

  static char timeText[] = "00:00:00"; // Needs to be static because it's used by the system later.

  PblTm currentTime;


  get_time(&currentTime);

  string_format_time(timeText, sizeof(timeText), "%T", &currentTime);

  text_layer_set_text(&timeLayer, timeText);

}


void updateEditModeDisplay() {

  // TODO: Add time out to revert to EDIT_MODE_OFF?

  switch (editMode) {
    case EDIT_MODE_OFF:
      layer_set_hidden(&editModeDisplayLayer.layer, true);
      break;

    case EDIT_MODE_X:
      layer_set_hidden(&editModeDisplayLayer.layer, false);
      text_layer_set_text(&editModeDisplayLayer, "Edit X-axis");
      break;

    case EDIT_MODE_Y:
      layer_set_hidden(&editModeDisplayLayer.layer, false);
      text_layer_set_text(&editModeDisplayLayer, "Edit Y-axis");
      break;

    default:
      break;
  }

}


void help_window_update_callback(Layer *me, GContext* ctx ) {

  graphics_context_set_stroke_color(ctx, GColorBlack); // TODO: Needed?
  graphics_context_set_fill_color(ctx, GColorBlack);

  // TODO: Modify this when we have more sensible bounds values.
  // TODO: Make this not inset so far.
  graphics_fill_rect(ctx, GRect(0, 0, me->bounds.size.w-10, me->bounds.size.h-10), 4, GCornersAll);

  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, GRect(2, 2, me->bounds.size.w-10-(2*2), me->bounds.size.h-10-(2*2)), 4, GCornersAll);

  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(2+2, 2+2, me->bounds.size.w-10-((2+2)*2), me->bounds.size.h-10-((2+2)*2)), 4, GCornersAll);

}


void displayHelp() {
  static Window window;
  static Layer chromeLayer;
  static TextLayer helpTextLayer;


  window_init(&window, "Help");
  window_set_fullscreen(&window, true);

  window.layer.frame.origin.x = 5;
  window.layer.frame.origin.y = 5;

  // TODO: Should have get/set bounds functions public?
  window.layer.bounds.size.w -= 10;
  window.layer.bounds.size.h -= 10;

  window_set_background_color(&window, GColorClear);

  // TODO: Set proper/better frame/bounds values here?
  layer_init(&chromeLayer, window.layer.frame);
  chromeLayer.update_proc = &help_window_update_callback;
  layer_add_child(&window.layer, &chromeLayer);


  text_layer_init(&helpTextLayer, GRect(16, 16, 144-24-8 /* width */, 168-24-8 /* height */));
  text_layer_set_text_color(&helpTextLayer, GColorWhite);
  text_layer_set_background_color(&helpTextLayer, GColorBlack);
  text_layer_set_font(&helpTextLayer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text(&helpTextLayer, ">Select (long press)<\nChange edit mode.\n\n>Up/Down<\nIncrease/decrease axis value\n\n(Press back to close)");

  layer_add_child(&window.layer, &helpTextLayer.layer);

  window_stack_push(&window, false /* Not animated */);
}


void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  displayHelp();
}


void select_long_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  editMode = (editMode + 1) % EDIT_MODE_MAX;

  updateEditModeDisplay();
}


void updateLocation(bool upButtonPressed) {

  bool stateChanged = false;

  // TODO: Swap out different click configurators instead?
  switch(editMode) {

    case EDIT_MODE_X:
      clockFrame.origin.x = clockFrame.origin.x + (upButtonPressed ? 1 : -1);
      stateChanged = true;
      break;

    case EDIT_MODE_Y:
      // Sign matches button orientation
      clockFrame.origin.y = clockFrame.origin.y + (upButtonPressed ? -1 : 1);
      stateChanged = true;
      break;

    default:
      break;
  }

  if (stateChanged) {
      layer_set_frame(&timeLayer.layer, clockFrame);
      layer_mark_dirty(&timeLayer.layer);

      updateEditModeDisplay();
  }

}


void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  updateLocation(true);
}


void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  updateLocation(false);
}


void config_provider(ClickConfig **config, Window *window) {
  (void)window;

  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_single_click_handler;

  config[BUTTON_ID_SELECT]->long_click.handler = (ClickHandler) select_long_click_handler;

  config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler;
  config[BUTTON_ID_UP]->click.repeat_interval_ms = 100;

  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_single_click_handler;
  config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;
}


// Handle the start-up of the app
void handle_init_app(AppContextRef app_ctx) {

  // Create our app's base window
  window_init(&window, "On The Button");
  window_stack_push(&window, true /* Animated */);
  window_set_fullscreen(&window, true);
  window_set_background_color(&window, GColorBlack);

  // Init the text layer used to show the time
  // TODO: Wrap this boilerplate in a function?
  text_layer_init(&timeLayer, clockFrame);
  text_layer_set_text_color(&timeLayer, GColorWhite);
  text_layer_set_background_color(&timeLayer, GColorClear);
  text_layer_set_font(&timeLayer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));

  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  handle_second_tick(app_ctx, NULL);

  layer_add_child(&window.layer, &timeLayer.layer);


  // Layer to show the current edit mode
  text_layer_init(&editModeDisplayLayer, GRect(4, 4, 144-4, 20));
  text_layer_set_text_color(&editModeDisplayLayer, GColorWhite);
  text_layer_set_background_color(&editModeDisplayLayer, GColorClear);
  text_layer_set_font(&editModeDisplayLayer, fonts_get_system_font(FONT_KEY_GOTHIC_18));

  updateEditModeDisplay();

  layer_add_child(&window.layer, &editModeDisplayLayer.layer);


  // Attach our desired button functionality
  window_set_click_config_provider(&window, (ClickConfigProvider) config_provider);
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
