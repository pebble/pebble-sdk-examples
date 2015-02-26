#include <pebble.h>

static Window *s_main_window;
static Layer *s_layer;
static AppTimer *s_timer;

static char s_info_text[64] = "info";
static char s_str_example[32] = "Goodbye!";
static char s_str_copy[] = "Hello, World!";

static void update_layer_callback(Layer *layer, GContext *ctx) {
  // Get the amount of seconds and the milliseconds part since the Epoch
  // time_ms also returns the milliseconds, so you can optionally pass in null
  // for any output parameters you don't need
  time_t t = 0;
  uint16_t t_ms = 0;
  time_ms(&t, &t_ms);

  // Get the next pseudo-random number
  int r = rand();

  // Print formatted text into a buffer
  snprintf(s_info_text, sizeof(s_info_text), "str: %s\ntime: %lu\nmilliseconds: %u\nrand: %d", s_str_example, t, t_ms, r);

  GRect bounds = layer_get_frame(layer);

  // Draw the formatted text
  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx, s_info_text, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), GRect(10, 5, bounds.size.w, bounds.size.h), GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
}

static void timer_callback(void *data) {
  layer_mark_dirty(s_layer);
  s_timer = app_timer_register(50, timer_callback, NULL);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(s_main_window);
  GRect bounds = layer_get_frame(window_layer);

  s_layer = layer_create(bounds);
  layer_set_update_proc(s_layer, update_layer_callback);
  layer_add_child(window_layer, s_layer);

  // Copy the string into the example
  strcpy(s_str_example, s_str_copy);

  s_timer = app_timer_register(50, timer_callback, NULL);
}

static void main_window_unload(Window *window) {
  layer_destroy(s_layer);
}

static void init() {
  // Seed the pseudo-random number generator with the time
  srand(time(NULL));

  s_main_window = window_create();
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
