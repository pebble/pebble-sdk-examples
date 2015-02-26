#include "pebble.h"

typedef struct GCompOpInfo {
  char *name;
  GCompOp op;
} GCompOpInfo;

// These are the compositing modes available
static GCompOpInfo s_gcompops[] = {
  {"GCompOpAssign", GCompOpAssign},
  {"GCompOpAssignInverted", GCompOpAssignInverted},
  {"GCompOpOr", GCompOpOr},
  {"GCompOpAnd", GCompOpAnd},
  {"GCompOpClear", GCompOpClear},
  {"GCompOpSet", GCompOpSet}
};

#define NUM_COMP_OP_TYPES 6

static int s_current_gcompop = 0;

static Window *s_main_window;
static Layer *s_image_layer;
static GBitmap *s_image;

// This is a layer update callback where compositing will take place
static void layer_update_callback(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_frame(layer);

  // Display the name of the current compositing operation
  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx, s_gcompops[s_current_gcompop].name, fonts_get_system_font(FONT_KEY_GOTHIC_18), bounds, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);

  // Draw the large circle the image will composite with
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, GPoint(bounds.size.w / 2, bounds.size.h + 110), 180);

  // Use the image size to help center the image
#ifdef PBL_PLATFORM_BASALT
  GRect destination = gbitmap_get_bounds(s_image);
#else
  GRect destination = s_image->bounds;
#endif

  // Center horizontally using the window frame size
  destination.origin.x = (bounds.size.w - destination.size.w) / 2;
  destination.origin.y = 50;

  // Set the current compositing operation
  // This will only cause bitmaps to composite
  graphics_context_set_compositing_mode(ctx, s_gcompops[s_current_gcompop].op);

  // Draw the bitmap; it will use current compositing operation set
  graphics_draw_bitmap_in_rect(ctx, s_image, destination);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (--s_current_gcompop < 0) {
    s_current_gcompop = NUM_COMP_OP_TYPES - 1;
  }
  layer_mark_dirty(s_image_layer);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (++s_current_gcompop >= NUM_COMP_OP_TYPES) {
    s_current_gcompop = 0;
  }
  layer_mark_dirty(s_image_layer);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(s_main_window);
  GRect bounds = layer_get_frame(window_layer);

  s_image_layer = layer_create(bounds);
  layer_set_update_proc(s_image_layer, layer_update_callback);
  layer_add_child(window_layer, s_image_layer);

  s_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PUG);
}

static void main_window_unload(Window *window) {
  gbitmap_destroy(s_image);
  layer_destroy(s_image_layer);
}

static void init() {
  s_main_window = window_create();
  window_set_click_config_provider(s_main_window, click_config_provider);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
