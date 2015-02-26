/*
 * This application shows how to use the Compass API to build a simple watchface
 * that shows where magnetic north is.
 *
 * The compass background image source is:
 *    <http://opengameart.org/content/north-and-southalpha-chanel>
 */

#include "pebble.h"

// Vector paths for the compass needles
static const GPathInfo NEEDLE_NORTH_POINTS = { 
  3,
  (GPoint[]) { { -8, 0 }, { 8, 0 }, { 0, -36 } }
};
static const GPathInfo NEEDLE_SOUTH_POINTS = { 
  3,
  (GPoint[]) { { 8, 0 }, { 0, 36 }, { -8, 0 } }
};

static Window *s_main_window;
static BitmapLayer *s_bitmap_layer;
static GBitmap *s_background_bitmap;
static Layer *s_path_layer;
static TextLayer *s_heading_layer, *s_text_layer_calib_state;

static GPath *s_needle_north, *s_needle_south;

static void compass_heading_handler(CompassHeadingData heading_data) {
  // rotate needle accordingly
  gpath_rotate_to(s_needle_north, heading_data.magnetic_heading);
  gpath_rotate_to(s_needle_south, heading_data.magnetic_heading);

  // display heading in degrees and radians
  static char s_heading_buf[64];
  snprintf(s_heading_buf, sizeof(s_heading_buf),
    " %ld°\n%ld.%02ldpi",
    TRIGANGLE_TO_DEG(heading_data.magnetic_heading),
    // display radians, units digit
    (TRIGANGLE_TO_DEG(heading_data.magnetic_heading) * 2) / 360,
    // radians, digits after decimal
    ((TRIGANGLE_TO_DEG(heading_data.magnetic_heading) * 200) / 360) % 100
  );
  text_layer_set_text(s_heading_layer, s_heading_buf);

  // Modify alert layout depending on calibration state
  GRect bounds = layer_get_frame(window_get_root_layer(s_main_window)); 
  GRect alert_bounds; 
  if(heading_data.compass_status == CompassStatusDataInvalid) {
    // Tell user to move their arm
    alert_bounds = GRect(0, 0, bounds.size.w, bounds.size.h);
    text_layer_set_background_color(s_text_layer_calib_state, GColorBlack);
    text_layer_set_text_color(s_text_layer_calib_state, GColorWhite);
    text_layer_set_font(s_text_layer_calib_state, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_text_alignment(s_text_layer_calib_state, GTextAlignmentCenter);
  } else {
    // Show status at the top
    alert_bounds = GRect(0, -3, bounds.size.w, bounds.size.h / 7);
    text_layer_set_background_color(s_text_layer_calib_state, GColorClear);
    text_layer_set_text_color(s_text_layer_calib_state, GColorBlack);
    text_layer_set_font(s_text_layer_calib_state, fonts_get_system_font(FONT_KEY_GOTHIC_18));
    text_layer_set_text_alignment(s_text_layer_calib_state, GTextAlignmentLeft);
  }
  layer_set_frame(text_layer_get_layer(s_text_layer_calib_state), alert_bounds);

  // Display state of the compass
  static char s_valid_buf[64];
  switch (heading_data.compass_status) {
    case CompassStatusDataInvalid:
      snprintf(s_valid_buf, sizeof(s_valid_buf), "%s", "Compass is calibrating!\n\nMove your arm to aid calibration.");
      break;
    case CompassStatusCalibrating:
      snprintf(s_valid_buf, sizeof(s_valid_buf), "%s", "Fine tuning...");
      break;
    case CompassStatusCalibrated:
      snprintf(s_valid_buf, sizeof(s_valid_buf), "%s", "Calibrated");
      break;
  }
  text_layer_set_text(s_text_layer_calib_state, s_valid_buf);

  // trigger layer for refresh
  layer_mark_dirty(s_path_layer);
}

static void path_layer_update_callback(Layer *path, GContext *ctx) {
#ifdef PBL_COLOR
  graphics_context_set_fill_color(ctx, GColorRed);
#endif
  gpath_draw_filled(ctx, s_needle_north);       
#ifndef PBL_COLOR
  graphics_context_set_fill_color(ctx, GColorBlack);
#endif  
  gpath_draw_outline(ctx, s_needle_south);                     

  // creating centerpoint                 
  GRect bounds = layer_get_frame(path);          
  GPoint path_center = GPoint(bounds.size.w / 2, bounds.size.h / 2);  
  graphics_fill_circle(ctx, path_center, 3);       

  // then put a white circle on top               
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, path_center, 2);                      
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // Create the bitmap for the background and put it on the screen
  s_bitmap_layer = bitmap_layer_create(bounds);
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_COMPASS_BACKGROUND);
  bitmap_layer_set_bitmap(s_bitmap_layer, s_background_bitmap);
  
  // Make needle background 'transparent' with GCompOpAnd
  bitmap_layer_set_compositing_mode(s_bitmap_layer, GCompOpAnd);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_layer));

  // Create the layer in which we will draw the compass needles
  s_path_layer = layer_create(bounds);
  
  //  Define the draw callback to use for this layer
  layer_set_update_proc(s_path_layer, path_layer_update_callback);
  layer_add_child(window_layer, s_path_layer);

  // Initialize and define the two paths used to draw the needle to north and to south
  s_needle_north = gpath_create(&NEEDLE_NORTH_POINTS);
  s_needle_south = gpath_create(&NEEDLE_SOUTH_POINTS);

  // Move the needles to the center of the screen.
  GPoint center = GPoint(bounds.size.w / 2, bounds.size.h / 2);
  gpath_move_to(s_needle_north, center);
  gpath_move_to(s_needle_south, center);

  // Place text layers onto screen: one for the heading and one for calibration status
  s_heading_layer = text_layer_create(GRect(12, bounds.size.h * 3 / 4, bounds.size.w / 4, bounds.size.h / 5));
  text_layer_set_text(s_heading_layer, "No Data");
  layer_add_child(window_layer, text_layer_get_layer(s_heading_layer));

  s_text_layer_calib_state = text_layer_create(GRect(0, 0, bounds.size.w, bounds.size.h / 7));
  text_layer_set_text_alignment(s_text_layer_calib_state, GTextAlignmentLeft);
  text_layer_set_background_color(s_text_layer_calib_state, GColorClear);

  layer_add_child(window_layer, text_layer_get_layer(s_text_layer_calib_state));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_heading_layer);
  text_layer_destroy(s_text_layer_calib_state);
  gpath_destroy(s_needle_north);
  gpath_destroy(s_needle_south);
  layer_destroy(s_path_layer);
  gbitmap_destroy(s_background_bitmap);
  bitmap_layer_destroy(s_bitmap_layer);
}

static void init() {
  // initialize compass and set a filter to 2 degrees
  compass_service_set_heading_filter(2 * (TRIG_MAX_ANGLE / 360));
  compass_service_subscribe(&compass_heading_handler);

  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
}

static void deinit() {
  compass_service_unsubscribe();
  window_destroy(s_main_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}
