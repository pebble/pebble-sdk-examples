
// Font Viewer -- Cycle through some of the characters in a symbol font.

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID {0xD0, 0xF7, 0xA8, 0x8D, 0xAD, 0x3F, 0x4F, 0x2C, 0x87, 0x30, 0x86, 0xC9, 0x18, 0x1F, 0x59, 0x3C}
PBL_APP_INFO_SIMPLE(MY_UUID, "Font Viewer", "Demo Corp", 1 /* App version */);


Window window;

TextLayer textLayer;


char text_buffer[] = " Aa";

// Modify these common button handlers

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  if (text_buffer[1] != 'Z') {
    text_buffer[1]++;
    text_buffer[2]++;
    text_layer_set_text(&textLayer, text_buffer);
  }
}


void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  if (text_buffer[1] != 'A') {
    text_buffer[1]--;
    text_buffer[2]--;
    text_layer_set_text(&textLayer, text_buffer);
  }

}


void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  static bool show_symbol = false;

  show_symbol = !show_symbol;

  if (show_symbol) {
    text_layer_set_font(&textLayer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_UNICONS_30)));
  } else {
    text_layer_set_font(&textLayer, fonts_get_system_font(FONT_KEY_GOTHAM_30_BLACK));
  }
}


void select_long_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

}


// This usually won't need to be modified

void click_config_provider(ClickConfig **config, Window *window) {
  (void)window;

  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_single_click_handler;

  config[BUTTON_ID_SELECT]->long_click.handler = (ClickHandler) select_long_click_handler;

  config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler;
  config[BUTTON_ID_UP]->click.repeat_interval_ms = 100;

  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_single_click_handler;
  config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;
}


// Standard app initialisation

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Font Viewer");
  window_stack_push(&window, true /* Animated */);

  resource_init_current_app(&FONT_DEMO_RESOURCES);

  text_layer_init(&textLayer, window.layer.frame);
  text_layer_set_text(&textLayer, text_buffer);
  text_layer_set_font(&textLayer, fonts_get_system_font(FONT_KEY_GOTHAM_30_BLACK));
  layer_add_child(&window.layer, &textLayer.layer);

  // Attach our desired button functionality
  window_set_click_config_provider(&window, (ClickConfigProvider) click_config_provider);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init
  };
  app_event_loop(params, &handlers);
}
