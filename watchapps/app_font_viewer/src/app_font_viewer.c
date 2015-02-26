// Font Viewer -- Cycle through some of the characters in a symbol font.

#include <pebble.h>

#define PADDING 5
#define REPEAT_INTERVAL_MS 100

static Window *s_main_window;
static TextLayer *s_text_layer;
static TextLayer *s_char_text_layer;

static char s_text_buffer[] = "Aa";

static void down_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_text_buffer[0] != 'Z') {
    s_text_buffer[0]++;
    s_text_buffer[1]++;

    text_layer_set_text(s_text_layer, s_text_buffer);
    text_layer_set_text(s_char_text_layer, s_text_buffer);
  }
}

static void up_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_text_buffer[0] != 'A') {
    s_text_buffer[0]--;
    s_text_buffer[1]--;

    text_layer_set_text(s_text_layer, s_text_buffer);
    text_layer_set_text(s_char_text_layer, s_text_buffer);
  }
}

static void click_config_provider(void *context) {
  window_single_repeating_click_subscribe(BUTTON_ID_UP, REPEAT_INTERVAL_MS, up_single_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, REPEAT_INTERVAL_MS, down_single_click_handler);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_text_layer = text_layer_create((GRect) { { PADDING, 0 }, { bounds.size.w - 2*PADDING, bounds.size.h } });
  text_layer_set_text(s_text_layer, s_text_buffer);
  text_layer_set_font(s_text_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_UNICONS_30)));
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));

  s_char_text_layer = text_layer_create((GRect) { { PADDING, bounds.size.h - 60 }, { bounds.size.w - 2*PADDING, 40 } });
  text_layer_set_text(s_char_text_layer, s_text_buffer);
  text_layer_set_font(s_char_text_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  layer_add_child(window_layer, text_layer_get_layer(s_char_text_layer));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_text_layer);
  text_layer_destroy(s_char_text_layer);
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
