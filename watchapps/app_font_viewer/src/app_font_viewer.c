// Font Viewer -- Cycle through some of the characters in a symbol font.

#include <pebble.h>

static Window *window;

static TextLayer *text_layer;
static TextLayer *char_text_layer;

static char text_buffer[] = "Aa";

void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  if (text_buffer[0] != 'Z') {
    text_buffer[0]++;
    text_buffer[1]++;
    text_layer_set_text(text_layer, text_buffer);
    text_layer_set_text(char_text_layer, text_buffer);
  }
}

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  if (text_buffer[0] != 'A') {
    text_buffer[0]--;
    text_buffer[1]--;
    text_layer_set_text(text_layer, text_buffer);
    text_layer_set_text(char_text_layer, text_buffer);
  }
}

void click_config_provider(Window *window) {
  const uint16_t repeat_interval_ms = 100;
  window_single_repeating_click_subscribe(BUTTON_ID_UP, repeat_interval_ms, (ClickHandler) up_single_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, repeat_interval_ms, (ClickHandler) down_single_click_handler);
}

static void init() {
  window = window_create();
  window_stack_push(window, true /* Animated */);

  window_set_click_config_provider(window, (ClickConfigProvider) click_config_provider);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  static const int pad = 5;

  text_layer = text_layer_create((GRect) { { pad, 0 }, { bounds.size.w - 2*pad, bounds.size.h } });
  text_layer_set_text(text_layer, text_buffer);
  text_layer_set_font(text_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_UNICONS_30)));
  layer_add_child(window_layer, text_layer_get_layer(text_layer));

  char_text_layer = text_layer_create((GRect) { { pad, bounds.size.h - 60 }, { bounds.size.w - 2*pad, 40 } });
  text_layer_set_text(char_text_layer, text_buffer);
  text_layer_set_font(char_text_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  layer_add_child(window_layer, text_layer_get_layer(char_text_layer));
}

static void deinit() {
  text_layer_destroy(text_layer);
  text_layer_destroy(char_text_layer);
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
