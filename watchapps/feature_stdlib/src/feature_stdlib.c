/*

   Demonstrate a few standard library functions available.

 */

#include "pebble.h"

static Window *window;

static Layer *layer;

static AppTimer *timer = NULL;

static char info_text[256] = "info";

static char str_example[32] = "Goodbye!";

static char str_copy[32] = "Hello, World!";

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
  snprintf(info_text, sizeof(info_text),
      "str: %s\n"
      "time: %lu\n"
      "milliseconds: %u\n"
      "rand: %d",
      str_example, t, t_ms, r);

  // Draw the formatted text
  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx,
      info_text,
      fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD),
      (GRect){ .origin = GPoint(10, 5), .size = layer_get_frame(layer).size },
      GTextOverflowModeWordWrap,
      GTextAlignmentLeft,
      NULL);
}

static void timer_callback(void *data) {
  // Mark the layer dirty to have it update
  layer_mark_dirty(layer);

  // Start another timer to have this happen again
  timer = app_timer_register(50 /* milliseconds */, timer_callback, NULL);
}


int main(void) {
  // Seed the pseudo-random number generator with the time
  srand(time(NULL));

  // Setup the window
  window = window_create();
  window_stack_push(window, true /* Animated */);

  // Setup the layer that will display the text
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  layer = layer_create(bounds);
  layer_set_update_proc(layer, update_layer_callback);
  layer_add_child(window_layer, layer);

  // Copy the string into the example
  strcpy(str_example, str_copy);

  // Start the timer
  timer = app_timer_register(50 /* milliseconds */, timer_callback, NULL);

  app_event_loop();

  layer_destroy(layer);
  window_destroy(window);
}
