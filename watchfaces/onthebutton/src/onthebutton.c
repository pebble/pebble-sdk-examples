/*

   Customisable Watch Pebble App

   Shows how to use button functionality--as a standard app.

   Also shows adding a second window.

   Also shows how to have a full screen window.

   (Note: Doesn't currently preserve location.)

 */

// Standard includes
#include "pebble.h"


// App-specific data
static Window *window; // All apps must have at least one window

static TextLayer *timeLayer; // The clock

static GRect clockFrame = {.origin = {.x = 29, .y = 54}, .size = {.w = 144-40, .h = 168-54}};


#define EDIT_MODE_OFF 0
#define EDIT_MODE_X   1
#define EDIT_MODE_Y   2
#define EDIT_MODE_MAX 3

static TextLayer *editModeDisplayLayer;
static short editMode = EDIT_MODE_OFF;


// Called once per second
static void handle_second_tick(struct tm *currentTime, TimeUnits units_changed) {
  static char timeText[] = "00:00:00"; // Needs to be static because it's used by the system later.
  strftime(timeText, sizeof(timeText), "%T", currentTime);
  text_layer_set_text(timeLayer, timeText);
}


static void updateEditModeDisplay(void) {
  // TODO: Add time out to revert to EDIT_MODE_OFF?

  switch (editMode) {
    case EDIT_MODE_OFF:
      layer_set_hidden(text_layer_get_layer(editModeDisplayLayer), true);
      break;

    case EDIT_MODE_X:
      layer_set_hidden(text_layer_get_layer(editModeDisplayLayer), false);
      text_layer_set_text(editModeDisplayLayer, "Edit X-axis");
      break;

    case EDIT_MODE_Y:
      layer_set_hidden(text_layer_get_layer(editModeDisplayLayer), false);
      text_layer_set_text(editModeDisplayLayer, "Edit Y-axis");
      break;

    default:
      break;
  }

}


static void helpWindow_update_callback(Layer *me, GContext* ctx ) {
  graphics_context_set_stroke_color(ctx, GColorBlack); // TODO: Needed?
  graphics_context_set_fill_color(ctx, GColorBlack);

  // TODO: Modify this when we have more sensible bounds values.
  // TODO: Make this not inset so far.
  GRect me_bounds = layer_get_bounds(me);
  graphics_fill_rect(ctx, GRect(0, 0, me_bounds.size.w-10, me_bounds.size.h-10), 4, GCornersAll);

  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, GRect(2, 2, me_bounds.size.w-10-(2*2), me_bounds.size.h-10-(2*2)), 4, GCornersAll);

  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(2+2, 2+2, me_bounds.size.w-10-((2+2)*2), me_bounds.size.h-10-((2+2)*2)), 4, GCornersAll);

}

static Window *helpWindow;
static Layer *helpChromeLayer;
static TextLayer *helpTextLayer;

static void displayHelp(void) {
  window_stack_push(helpWindow, false /* Not animated */);
}

static void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  displayHelp();
}


static void select_long_click_handler(ClickRecognizerRef recognizer, Window *window) {
  editMode = (editMode + 1) % EDIT_MODE_MAX;

  updateEditModeDisplay();
}


static void updateLocation(bool upButtonPressed) {
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
      Layer *timeLayer_layer = text_layer_get_layer(timeLayer);
      layer_set_frame(timeLayer_layer, clockFrame);
      layer_mark_dirty(timeLayer_layer);

      updateEditModeDisplay();
  }

}


static void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  updateLocation(true);
}


static void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  updateLocation(false);
}


static void config_provider(Window *window) {
  const uint16_t repeat_interval_ms = 100;

  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) select_single_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 0, (ClickHandler) select_long_click_handler, NULL);
  window_single_repeating_click_subscribe(BUTTON_ID_UP, repeat_interval_ms, (ClickHandler) up_single_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, repeat_interval_ms, (ClickHandler) down_single_click_handler);
}


// Initialize the app
static void app_init(void) {
  // Create our app's base window
  window = window_create();
  Layer *window_layer = window_get_root_layer(window);
  window_stack_push(window, true /* Animated */);
  window_set_fullscreen(window, true);
  window_set_background_color(window, GColorBlack);

  // Init the text layer used to show the time
  // TODO: Wrap this boilerplate in a function?
  timeLayer = text_layer_create(clockFrame);
  text_layer_set_text_color(timeLayer, GColorWhite);
  text_layer_set_background_color(timeLayer, GColorClear);
  text_layer_set_font(timeLayer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));

  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);
  handle_second_tick(tick_time, SECOND_UNIT);

  layer_add_child(window_layer, text_layer_get_layer(timeLayer));

  // Layer to show the current edit mode
  editModeDisplayLayer = text_layer_create(GRect(4, 4, 144-4, 20));
  text_layer_set_text_color(editModeDisplayLayer, GColorWhite);
  text_layer_set_background_color(editModeDisplayLayer, GColorClear);
  text_layer_set_font(editModeDisplayLayer, fonts_get_system_font(FONT_KEY_GOTHIC_18));

  updateEditModeDisplay();

  layer_add_child(window_layer, text_layer_get_layer(editModeDisplayLayer));

  // Attach our desired button functionality
  window_set_click_config_provider(window, (ClickConfigProvider) config_provider);

  // Init the help window
  helpWindow = window_create();
  window_set_fullscreen(helpWindow, true);
  Layer *helpWindow_layer = window_get_root_layer(helpWindow);

  GRect helpWindow_frame = layer_get_frame(helpWindow_layer);
  helpWindow_frame.origin.x = 5;
  helpWindow_frame.origin.y = 5;
  layer_set_frame(helpWindow_layer, helpWindow_frame);

  GRect helpWindow_bounds = layer_get_bounds(window_layer);
  helpWindow_bounds.size.w -= 10;
  helpWindow_bounds.size.h -= 10;
  layer_set_bounds(helpWindow_layer, helpWindow_bounds);

  window_set_background_color(helpWindow, GColorClear);

  // Layer for the help screen
  // TODO: Set proper/better frame/bounds values here?
  helpChromeLayer = layer_create(helpWindow_frame);
  layer_set_update_proc(helpChromeLayer, &helpWindow_update_callback);
  layer_add_child(helpWindow_layer, helpChromeLayer);

  helpTextLayer = text_layer_create(GRect(16, 16, 144-24-8 /* width */, 168-24-8 /* height */));
  text_layer_set_text_color(helpTextLayer, GColorWhite);
  text_layer_set_background_color(helpTextLayer, GColorBlack);
  text_layer_set_font(helpTextLayer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text(helpTextLayer, ">Select (long press)<\nChange edit mode.\n\n>Up/Down<\nIncrease/decrease axis value\n\n(Press back to close)");

  layer_add_child(helpWindow_layer, text_layer_get_layer(helpTextLayer));

  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
}

static void app_deinit(void) {
  text_layer_destroy(editModeDisplayLayer);
  text_layer_destroy(timeLayer);
  layer_destroy(helpChromeLayer);
  text_layer_destroy(helpTextLayer);
  window_destroy(helpWindow);
  window_destroy(window);
}

int main(void) {
    app_init();
    app_event_loop();
    app_deinit();
}
