/*
 * This application shows how to use the Compass API to build a simple watchface
 * that shows where magnetic north is.
 *
 * The compass background image source is:
 *    <http://opengameart.org/content/north-and-southalpha-chanel>
 *
 */

#include "pebble.h"

// Group all UI elements in a global struct
static struct CompassUI {
  Window *window;
  BitmapLayer *bitmap_layer;
  GBitmap *background;
  Layer *path_layer;
  TextLayer *text_layer;
  TextLayer *text_layer_calib_state;
  GPath *needle_north, *needle_south;
} s_ui;

// Vector paths for the compass needles
static const GPathInfo NEEDLE_NORTH_POINTS = { 3,
  (GPoint []) { { -8, 0 }, { 8, 0 }, { 0, -36 } }
};
static const GPathInfo NEEDLE_SOUTH_POINTS = { 3,
  (GPoint []) { { 8, 0 }, { 0, 36 }, { -8, 0 } }
};

// This is the function called by the Compass Service every time the compass heading
// changes by more than the filter (2 degrees in this example).
void compass_heading_handler(CompassHeadingData heading_data){
  // rotate needle accordingly
  gpath_rotate_to(s_ui.needle_north, heading_data.magnetic_heading);
  gpath_rotate_to(s_ui.needle_south, heading_data.magnetic_heading);

  // display heading in degrees and radians
  static char heading_buf[64];
  snprintf(heading_buf, sizeof(heading_buf),
    " %ld°\n%ld.%02ldpi",
    TRIGANGLE_TO_DEG(heading_data.magnetic_heading),
    // display radians, units digit
    (TRIGANGLE_TO_DEG(heading_data.magnetic_heading) * 2) / 360,
    // radians, digits after decimal
    ((TRIGANGLE_TO_DEG(heading_data.magnetic_heading) * 200) / 360) % 100
  );
  text_layer_set_text(s_ui.text_layer, heading_buf);


  static char valid_buf[18];
  switch (heading_data.compass_status) {
    case CompassStatusDataInvalid:
      snprintf(valid_buf, sizeof(valid_buf), "%s", "Calibrating");
      break;
    case CompassStatusCalibrating:
      snprintf(valid_buf, sizeof(valid_buf), "%s", "Fine calibrating");
      break;
    case CompassStatusCalibrated:
      snprintf(valid_buf, sizeof(valid_buf), "%s", "Calibrated");
  }
  text_layer_set_text(s_ui.text_layer_calib_state, valid_buf);

  // trigger layer for refresh
  layer_mark_dirty(s_ui.path_layer);
}

// This is the draw callback for the path_layer function. This function will draw
// both compass needles.
static void path_layer_update_callback(Layer *path, GContext *ctx) {
  gpath_draw_filled(ctx, s_ui.needle_north);                      // north filled
  gpath_draw_outline(ctx, s_ui.needle_south);                     // south outlined
  GRect bounds = layer_get_frame(path);                           // grabbing frame of current layer
  GPoint path_center = GPoint(bounds.size.w/2, bounds.size.h/2);  // creating centerpoint
  graphics_fill_circle(ctx, path_center, 3);                      // use it to make a black, centered circle
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, path_center, 2);                      // then put a white circle on top
}

// Initializes the window and all the UI elements
static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // Create the bitmap for the background and put it on the screen
  s_ui.bitmap_layer = bitmap_layer_create(bounds);
  s_ui.background = gbitmap_create_with_resource(RESOURCE_ID_COMPASS_BACKGROUND);
  bitmap_layer_set_bitmap(s_ui.bitmap_layer, s_ui.background);
  // Make needle background 'transparent' with GCompOpAnd
  bitmap_layer_set_compositing_mode(s_ui.bitmap_layer, GCompOpAnd);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_ui.bitmap_layer));

  // Create the layer in which we will draw the compass needles
  s_ui.path_layer = layer_create(bounds);
  //  Define the draw callback to use for this layer
  layer_set_update_proc(s_ui.path_layer, path_layer_update_callback);
  layer_add_child(window_layer, s_ui.path_layer);

  // Initialize and define the two paths used to draw the needle to north and to south
  // see Examples/watchapps/feature_gpath for more info
  s_ui.needle_north = gpath_create(&NEEDLE_NORTH_POINTS);
  s_ui.needle_south = gpath_create(&NEEDLE_SOUTH_POINTS);

  // Move the needles to the center of the screen.
  GPoint center = GPoint(bounds.size.w/2, bounds.size.h/2);
  gpath_move_to(s_ui.needle_north, center);
  gpath_move_to(s_ui.needle_south, center);

  // Place text layers onto screen: one for the heading and one for calibration status
  s_ui.text_layer = text_layer_create( (GRect){
    .origin = {.x = 12, .y = bounds.size.h*3/4},
    .size = {.w = bounds.size.w/4, .h = bounds.size.h/5}
  });
  text_layer_set_text(s_ui.text_layer, "No Data");

  s_ui.text_layer_calib_state = text_layer_create( (GRect){
    .origin = {.x = 0, .y = 0},
    .size = {.w = bounds.size.w, .h = bounds.size.h/7}
  });
  text_layer_set_text_alignment(s_ui.text_layer_calib_state, GTextAlignmentRight);
  text_layer_set_background_color(s_ui.text_layer_calib_state, GColorClear);

  layer_add_child(window_layer, text_layer_get_layer(s_ui.text_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_ui.text_layer_calib_state));
}

// Free all memory initialized in window_load()
static void window_unload(Window *window) {
  text_layer_destroy(s_ui.text_layer);
  text_layer_destroy(s_ui.text_layer_calib_state);
  gpath_destroy(s_ui.needle_north);
  gpath_destroy(s_ui.needle_south);
  layer_destroy(s_ui.path_layer);
  gbitmap_destroy(s_ui.background);
  bitmap_layer_destroy(s_ui.bitmap_layer);
}

static void init(void) {
  // initialize compass and set a filter to 2 degrees
  compass_service_set_heading_filter(2 * (TRIG_MAX_ANGLE/360));
  compass_service_subscribe(&compass_heading_handler);

  // initialize base window
  s_ui.window = window_create();
  window_set_window_handlers(s_ui.window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  window_stack_push(s_ui.window, true);
}

static void deinit(void) {
  compass_service_unsubscribe();
  window_destroy(s_ui.window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
