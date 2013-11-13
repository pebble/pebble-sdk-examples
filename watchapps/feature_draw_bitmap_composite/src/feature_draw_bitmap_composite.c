/*

   Demonstrate graphics bitmap compositing.

*/

#include "pebble.h"

typedef struct GCompOpInfo {
  char *name;
  GCompOp op;
} GCompOpInfo;

// These are the compositing modes available
static GCompOpInfo gcompops[] = {
  {"GCompOpAssign", GCompOpAssign},
  {"GCompOpAssignInverted", GCompOpAssignInverted},
  {"GCompOpOr", GCompOpOr},
  {"GCompOpAnd", GCompOpAnd},
  {"GCompOpClear", GCompOpClear},
  {"GCompOpSet", GCompOpSet}
};

static const int num_gcompops = ARRAY_LENGTH(gcompops);

static int current_gcompop = 0;

static Window *window;

static Layer *layer;

// We will use a bitmap to composite with a large circle
static GBitmap *image;

// This is a layer update callback where compositing will take place
static void layer_update_callback(Layer *layer, GContext* ctx) {
  GRect bounds = layer_get_frame(layer);

  // Display the name of the current compositing operation
  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx,
    gcompops[current_gcompop].name,
    fonts_get_system_font(FONT_KEY_GOTHIC_18),
    bounds,
    GTextOverflowModeTrailingEllipsis,
    GTextAlignmentCenter,
    NULL);

  // Draw the large circle the image will composite with
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, GPoint(bounds.size.w/2, bounds.size.h+110), 180);

  // Use the image size to help center the image
  GRect destination = image->bounds;

  // Center horizontally using the window frame size
  destination.origin.x = (bounds.size.w-destination.size.w)/2;
  destination.origin.y = 50;

  // Set the current compositing operation
  // This will only cause bitmaps to composite
  graphics_context_set_compositing_mode(ctx, gcompops[current_gcompop].op);

  // Draw the bitmap; it will use current compositing operation set
  graphics_draw_bitmap_in_rect(ctx, image, destination);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (--current_gcompop < 0) {
    current_gcompop = num_gcompops-1;
  }
  layer_mark_dirty(layer);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (++current_gcompop >= num_gcompops) {
    current_gcompop = 0;
  }
  layer_mark_dirty(layer);
}

static void config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

int main(void) {
  // Then use the respective resource loader to obtain the resource for use
  // In this case, we load the image
  image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PUG);

  window = window_create();
  window_stack_push(window, true /* Animated */);

  window_set_click_config_provider(window, config_provider);

  // Initialize the layer
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  layer = layer_create(bounds);

  // Set up the update layer callback
  layer_set_update_proc(layer, layer_update_callback);

  // Add the layer to the window for display
  layer_add_child(window_layer, layer);

  // Enter the main loop
  app_event_loop();

  // Cleanup the image
  gbitmap_destroy(image);

  layer_destroy(layer);
  window_destroy(window);
}
