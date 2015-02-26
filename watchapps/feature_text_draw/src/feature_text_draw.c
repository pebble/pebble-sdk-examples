#include <pebble.h>

static Window *s_main_window;
static Layer *s_layer;

static void update_layer_callback(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_frame(layer);

  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx, "Text here.", fonts_get_system_font(FONT_KEY_FONT_FALLBACK), GRect(5, 5, bounds.size.w - 10, 100), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  graphics_draw_text(ctx, "And text here as well.", fonts_get_system_font(FONT_KEY_FONT_FALLBACK), GRect(90, 100, bounds.size.w - 95, 60), GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  s_layer = layer_create(bounds);
  layer_set_update_proc(s_layer, update_layer_callback);
  layer_add_child(window_layer, s_layer);
}

static void main_window_unload(Window *window) {
  layer_destroy(s_layer);
}

static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
}

static void deinit(void) {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
