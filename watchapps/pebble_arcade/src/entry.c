#include <pebble.h>
#include "entry.h"

#define NUM_ENTRY_CHARS 3

#define REPEAT_INTERVAL_MS 100

static Window *s_entry_window;

static char* s_highscore_name;

typedef struct {
  TextLayer *title_text;
  TextLayer *chars_text[NUM_ENTRY_CHARS];
  InverterLayer *invert;

  char entry_chars[NUM_ENTRY_CHARS][2];
  uint8_t index;
} EntryUi;

static void up_click_handler(ClickRecognizerRef recognizer, void* context) {
  Window* window = (Window*)context;
  EntryUi* ui_data = (EntryUi*)window_get_user_data(window);
  if (ui_data->index < NUM_ENTRY_CHARS) {
    if (ui_data->entry_chars[ui_data->index][0] == 'Z') {
      ui_data->entry_chars[ui_data->index][0] = 'A';
    } else {
      ++ui_data->entry_chars[ui_data->index][0];
    }
    layer_mark_dirty(text_layer_get_layer(ui_data->chars_text[ui_data->index]));
  }
}

static void down_click_handler(ClickRecognizerRef recognizer, void* context) {
  Window* window = (Window*)context;
  EntryUi* ui_data = (EntryUi*)window_get_user_data(window);
  if (ui_data->index < NUM_ENTRY_CHARS) {
    if (ui_data->entry_chars[ui_data->index][0] == 'A') {
      ui_data->entry_chars[ui_data->index][0] = 'Z';
    } else {
      --ui_data->entry_chars[ui_data->index][0];
    }
    layer_mark_dirty(text_layer_get_layer(ui_data->chars_text[ui_data->index]));
  }
}

static void select_click_handler(ClickRecognizerRef recognizer, void* context) {
  Window* window = (Window*)context;
  EntryUi* ui_data = (EntryUi*)window_get_user_data(window);
  if (ui_data->index == 2) {
    ui_data->index = 0;
  } else {
    ++ui_data->index;
  }

  inverter_layer_destroy(ui_data->invert);
  ui_data->invert = inverter_layer_create(GRect(41 + 20 * ui_data->index, 66, 15, 31));
  layer_add_child(window_get_root_layer(window), inverter_layer_get_layer(ui_data->invert));
}

static void click_config_provider(void* context) {
  window_set_click_context(BUTTON_ID_UP, context);
  window_single_repeating_click_subscribe(BUTTON_ID_UP, REPEAT_INTERVAL_MS, up_click_handler);

  window_set_click_context(BUTTON_ID_SELECT, context);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);

  window_set_click_context(BUTTON_ID_DOWN, context);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, REPEAT_INTERVAL_MS, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  EntryUi* ui_data = (EntryUi*)window_get_user_data(window);
  ui_data->title_text = text_layer_create(GRect(0, 0, bounds.size.w, 64));
  text_layer_set_text(ui_data->title_text, "NEW HIGH SCORE!");
  text_layer_set_text_alignment(ui_data->title_text, GTextAlignmentCenter);
  text_layer_set_font(ui_data->title_text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(ui_data->title_text));

  ui_data->index = 0;

  for (int i = 0; i < NUM_ENTRY_CHARS; ++i) {
    strncpy(ui_data->entry_chars[i], "A", 2);

    ui_data->chars_text[i] = text_layer_create(GRect(42 + 20 * i, 64, 15, 50));
    text_layer_set_font(ui_data->chars_text[i], fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_text_alignment(ui_data->chars_text[i], GTextAlignmentCenter);
    text_layer_set_text(ui_data->chars_text[i], ui_data->entry_chars[i]);
    layer_add_child(window_layer, text_layer_get_layer(ui_data->chars_text[i]));
  }

  ui_data->invert = inverter_layer_create(GRect(41, 66, 16, 31));
  layer_add_child(window_layer, inverter_layer_get_layer(ui_data->invert));
}

static void window_unload(Window *window) {
  EntryUi* ui_data = (EntryUi*)window_get_user_data(window);

  text_layer_destroy(ui_data->title_text);
  for (int i = 0; i < NUM_ENTRY_CHARS; ++i) {
    s_highscore_name[i] = ui_data->entry_chars[i][0];
    text_layer_destroy(ui_data->chars_text[i]);
  }
  s_highscore_name[3] = '\0';
  inverter_layer_destroy(ui_data->invert);
}

void entry_init() {
  s_entry_window = window_create();

  // Store ui data as window user_data
  EntryUi* ui_data = malloc(sizeof(EntryUi));
  window_set_user_data(s_entry_window, ui_data);

  window_set_click_config_provider_with_context(s_entry_window, click_config_provider, (void*)s_entry_window);
  window_set_window_handlers(s_entry_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
}

void entry_deinit() {
  // Delete stored window user_data
  EntryUi* ui_data = (EntryUi*)window_get_user_data(s_entry_window);
  free(ui_data);
  window_destroy(s_entry_window);
}

void entry_get_name(char *name) {
  s_highscore_name = name;
  window_stack_push(s_entry_window, true);
}
