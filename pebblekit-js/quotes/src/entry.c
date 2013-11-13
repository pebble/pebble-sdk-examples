#include <pebble.h>

#include "entry.h"

static struct EntryUi {
  Window *window;
  TextLayer *title_text;

  TextLayer *chars_text[4];
  char entry_chars[4][2];
  uint8_t index;

  char entry_name[30];
  InverterLayer *invert;
} ui;

char *hs_name; // Pointer to put the name into.
EntryCallback hs_callback;

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (ui.index < 4) {
    if (ui.entry_chars[ui.index][0] == 'Z')
      ui.entry_chars[ui.index][0] = 'A';
    else
      ++ui.entry_chars[ui.index][0];
    layer_mark_dirty(text_layer_get_layer(ui.chars_text[ui.index]));
  }
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (ui.index < 4) {
    if (ui.entry_chars[ui.index][0] == 'A')
      ui.entry_chars[ui.index][0] = 'Z';
    else
      --ui.entry_chars[ui.index][0];
    layer_mark_dirty(text_layer_get_layer(ui.chars_text[ui.index]));
  }
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (ui.index == 3)
    ui.index = 0;
  else
    ++ui.index;

  inverter_layer_destroy(ui.invert);
  ui.invert = inverter_layer_create((GRect) {
        .origin = { 35 + 20 * ui.index, 66 },
        .size = { 15, 31 }
      });
  layer_add_child(window_get_root_layer(ui.window),
                  inverter_layer_get_layer(ui.invert));
}

static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (ui.entry_chars[3][0] == '\0')
    ui.entry_chars[3][0] = 'A';
  else
    ui.entry_chars[3][0] = '\0';

  layer_mark_dirty(text_layer_get_layer(ui.chars_text[ui.index]));
}

static void click_config_provider(void *context) {
  const uint16_t repeat_interval_ms = 100;
  window_single_repeating_click_subscribe(BUTTON_ID_UP, repeat_interval_ms, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 0, select_long_click_handler, NULL);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, repeat_interval_ms, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(ui.window);
  GRect bounds = layer_get_bounds(window_layer);

  ui.title_text = text_layer_create((GRect) {
        .origin = { 0, 0 },
        .size = { bounds.size.w, 64 }
      });
  text_layer_set_text(ui.title_text, ui.entry_name);
  text_layer_set_text_alignment(ui.title_text, GTextAlignmentCenter);
  text_layer_set_font(ui.title_text,
                      fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(ui.title_text));

  ui.index = 0;

  for (int i = 0; i < 4; ++i) {
    strncpy(ui.entry_chars[i], "A", 2);

    ui.chars_text[i] = text_layer_create(
        (GRect) { .origin = { 35+20*i, 64 },  .size = { 15, 50 } });
    text_layer_set_font(ui.chars_text[i],
                        fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_text_alignment(ui.chars_text[i], GTextAlignmentCenter);
    text_layer_set_text(ui.chars_text[i], ui.entry_chars[i]);
    layer_add_child(window_layer, text_layer_get_layer(ui.chars_text[i]));
  }

  ui.invert = inverter_layer_create((GRect) {
        .origin = { 35, 66 },
        .size = { 16, 31 }
      });
  layer_add_child(window_layer, inverter_layer_get_layer(ui.invert));
}

static void window_unload(Window *window) {
  text_layer_destroy(ui.title_text);
  for (int i = 0; i < 4; ++i) {
    hs_name[i] = ui.entry_chars[i][0];
    text_layer_destroy(ui.chars_text[i]);
  }
  hs_name[4] = '\0';
  inverter_layer_destroy(ui.invert);
  hs_callback(hs_name);
}

void entry_init(char *name) {
  ui.window = window_create();
  strncpy(ui.entry_name, name, 30);
  window_set_click_config_provider(ui.window, click_config_provider);
  window_set_window_handlers(ui.window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
}

void entry_deinit(void) {
  window_destroy(ui.window);
}

void entry_get_name(char *name, EntryCallback callback) {
  hs_callback = callback;
  hs_name = name;
  window_stack_push(ui.window, true);
}
