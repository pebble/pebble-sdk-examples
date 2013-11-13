#include <pebble.h>

#include "entry.h"

static Window *window;
static TextLayer *symbol_layer;
static TextLayer *price_layer;
static char symbol[5];
static char price[10];

enum {
  QUOTE_KEY_SYMBOL = 0x0,
  QUOTE_KEY_PRICE = 0x1,
  QUOTE_KEY_FETCH = 0x2,
};

static void set_symbol_msg(char *symbol) {
  Tuplet symbol_tuple = TupletCString(QUOTE_KEY_SYMBOL, symbol);

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (iter == NULL) {
    return;
  }

  dict_write_tuplet(iter, &symbol_tuple);
  dict_write_end(iter);

  app_message_outbox_send();
}

static void fetch_msg(void) {
  Tuplet fetch_tuple = TupletInteger(QUOTE_KEY_FETCH, 1);
  Tuplet price_tuple = TupletInteger(QUOTE_KEY_PRICE, 1);

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (iter == NULL) {
    return;
  }

  dict_write_tuplet(iter, &fetch_tuple);
  dict_write_tuplet(iter, &price_tuple);
  dict_write_end(iter);

  app_message_outbox_send();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  // refresh
  text_layer_set_text(price_layer, "Loading...");
  fetch_msg();
}

static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  // refresh
  entry_get_name(symbol, set_symbol_msg);
  text_layer_set_text(symbol_layer, symbol);
  text_layer_set_text(price_layer, "Loading...");
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 0, select_long_click_handler, NULL);
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *symbol_tuple = dict_find(iter, QUOTE_KEY_SYMBOL);
  Tuple *price_tuple = dict_find(iter, QUOTE_KEY_PRICE);

  if (symbol_tuple) {
    strncpy(symbol, symbol_tuple->value->cstring, 5);
    text_layer_set_text(symbol_layer, symbol);
  }
  if (price_tuple) {
    strncpy(price, price_tuple->value->cstring, 10);
    text_layer_set_text(price_layer, price);
  }
}

static void in_dropped_handler(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Dropped!");
}

static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Failed to Send!");
}

static void app_message_init(void) {
  // Register message handlers
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_failed(out_failed_handler);
  // Init buffers
  app_message_open(64, 64);
  fetch_msg();
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  symbol_layer = text_layer_create(
      (GRect) { .origin = { 0, 20 }, .size = { bounds.size.w, 50 } });
  text_layer_set_text(symbol_layer, "PBL"); text_layer_set_text_alignment(symbol_layer, GTextAlignmentCenter); text_layer_set_font(symbol_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(symbol_layer));

  price_layer = text_layer_create(
      (GRect) { .origin = { 0, 75 }, .size = { bounds.size.w, 50 } });
  text_layer_set_text(price_layer, "$0000.00");
  text_layer_set_text_alignment(price_layer, GTextAlignmentCenter);
  text_layer_set_font(price_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  layer_add_child(window_layer, text_layer_get_layer(price_layer));

  fetch_msg();
}

static void window_unload(Window *window) {
  text_layer_destroy(symbol_layer);
  text_layer_destroy(price_layer);
}

static void init(void) {
  window = window_create();
  app_message_init();
  char entry_title[] = "Enter Symbol";
  entry_init(entry_title);
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
