#include "pebble.h"

static Window *window;

static Layer *layer;

static void update_layer_callback(Layer *layer, GContext* ctx) {
  graphics_context_set_text_color(ctx, GColorBlack);

  GRect bounds = layer_get_frame(layer);

  graphics_draw_text(ctx,
      "Text here.",
      fonts_get_system_font(FONT_KEY_FONT_FALLBACK),
      GRect(5, 5, bounds.size.w-10, 100),
      GTextOverflowModeWordWrap,
      GTextAlignmentLeft,
      NULL);

  graphics_draw_text(ctx,
      "And text here as well.",
      fonts_get_system_font(FONT_KEY_FONT_FALLBACK),
      GRect(90, 100, bounds.size.w-95, 60),
      GTextOverflowModeWordWrap,
      GTextAlignmentRight,
      NULL);
}

int main(void) {
  window = window_create();
  window_stack_push(window, true /* Animated */);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  layer = layer_create(bounds);
  layer_set_update_proc(layer, update_layer_callback);
  layer_add_child(window_layer, layer);

  app_event_loop();

  layer_destroy(layer);
  window_destroy(window);
}
