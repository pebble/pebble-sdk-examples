#include "pebble.h"

static Window *window;

static Layer *path_layer;

// This defines graphics path information to be loaded as a path later
static const GPathInfo HOUSE_PATH_POINTS = {
  // This is the amount of points
  11,
  // A path can be concave, but it should not twist on itself
  // The points should be defined in clockwise order due to the rendering
  // implementation. Counter-clockwise will work in older firmwares, but
  // it is not officially supported
  (GPoint []) {
    {-40, 0},
    {0, -40},
    {40, 0},
    {28, 0},
    {28, 40},
    {10, 40},
    {10, 16},
    {-10, 16},
    {-10, 40},
    {-28, 40},
    {-28, 0}
  }
};

// This is an example of a path that looks like a compound path
// If you rotate it however, you will see it is a single shape
static const GPathInfo INFINITY_RECT_PATH_POINTS = {
  16,
  (GPoint []) {
    {-50, 0},
    {-50, -60},
    {10, -60},
    {10, -20},
    {-10, -20},
    {-10, -40},
    {-30, -40},
    {-30, -20},
    {50, -20},
    {50, 40},
    {-10, 40},
    {-10, 0},
    {10, 0},
    {10, 20},
    {30, 20},
    {30, 0}
  }
};

static GPath *house_path;

static GPath *infinity_path;

#define NUM_GRAPHIC_PATHS 2

static GPath *graphic_paths[NUM_GRAPHIC_PATHS];

static GPath *current_path = NULL;

static int current_path_index = 0;

static int path_angle = 0;

static bool outline_mode = false;

// This is the layer update callback which is called on render updates
static void path_layer_update_callback(Layer *me, GContext *ctx) {
  (void)me;

  // You can rotate the path before rendering
  gpath_rotate_to(current_path, (TRIG_MAX_ANGLE / 360) * path_angle);

  // There are two ways you can draw a GPath: outline or filled
  // In this example, only one or the other is drawn, but you can draw
  // multiple instances of the same path filled or outline.
  if (outline_mode) {
    // draw outline uses the stroke color
    graphics_context_set_stroke_color(ctx, GColorWhite);
    gpath_draw_outline(ctx, current_path);
  } else {
    // draw filled uses the fill color
    graphics_context_set_fill_color(ctx, GColorWhite);
    gpath_draw_filled(ctx, current_path);
  }
}

static int path_angle_add(int angle) {
  return path_angle = (path_angle + angle) % 360;
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  // Rotate the path counter-clockwise
  path_angle_add(-10);
  layer_mark_dirty(path_layer);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  // Rotate the path clockwise
  path_angle_add(10);
  layer_mark_dirty(path_layer);
}

static void select_raw_down_handler(ClickRecognizerRef recognizer, void *context) {
  // Show the outline of the path when select is held down
  outline_mode = true;
  layer_mark_dirty(path_layer);
}
static void select_raw_up_handler(ClickRecognizerRef recognizer, void *context) {
  // Show the path filled
  outline_mode = false;
  // Cycle to the next path
  current_path_index = (current_path_index+1) % NUM_GRAPHIC_PATHS;
  current_path = graphic_paths[current_path_index];
  layer_mark_dirty(path_layer);
}

static void config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_raw_click_subscribe(BUTTON_ID_SELECT, select_raw_down_handler, select_raw_up_handler, NULL);
}

static void init() {
  window = window_create();
  window_stack_push(window, true /* Animated */);
  window_set_background_color(window, GColorBlack);

  window_set_click_config_provider(window, config_provider);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  path_layer = layer_create(bounds);
  layer_set_update_proc(path_layer, path_layer_update_callback);
  layer_add_child(window_layer, path_layer);

  // Pass the corresponding GPathInfo to initialize a GPath
  house_path = gpath_create(&HOUSE_PATH_POINTS);
  infinity_path = gpath_create(&INFINITY_RECT_PATH_POINTS);

  // This demo allows you to cycle paths in an array
  // Try adding more GPaths to cycle through
  // You'll need to define another GPathInfo
  // Remember to update NUM_GRAPHIC_PATHS accordingly
  graphic_paths[0] = house_path;
  graphic_paths[1] = infinity_path;

  current_path = graphic_paths[0];

  // Move all paths to the center of the screen
  for (int i = 0; i < NUM_GRAPHIC_PATHS; i++) {
    gpath_move_to(graphic_paths[i], GPoint(bounds.size.w/2, bounds.size.h/2));
  }
}

static void deinit() {
  gpath_destroy(house_path);
  gpath_destroy(infinity_path);

  layer_destroy(path_layer);
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
