/*
 * This app uses two Windows:
 *  - s_main_window which uses a MenuLayer to display a list of fonts;
 *  - s_font_window.
 *
 * The font window uses three TextLayers:
 *  - to display a sample of text at the top of the screen (s_text_layer),
 *  - to display at the bottom of the screen the name (s_font_name_layer),
 *  - and variant (s_font_variant_layer) of the currently selected font.
 */

#include "pebble.h"

#define NUM_FONTS 16
#define NUM_MESSAGES 3

typedef struct {
 char *name;
 char *variant;
 char *res;
} PebbleFont;

static PebbleFont pebble_fonts[] = {
 { .name = "Gothic", .variant = "14", .res = FONT_KEY_GOTHIC_14 },
 { .name = "Gothic", .variant = "14 Bold", .res = FONT_KEY_GOTHIC_14_BOLD },
 { .name = "Gothic", .variant = "18", .res = FONT_KEY_GOTHIC_18 },
 { .name = "Gothic", .variant = "18 Bold", .res = FONT_KEY_GOTHIC_18_BOLD },
 { .name = "Gothic", .variant = "24", .res = FONT_KEY_GOTHIC_24 },
 { .name = "Gothic", .variant = "24 Bold", .res = FONT_KEY_GOTHIC_24_BOLD },
 { .name = "Gothic", .variant = "28", .res = FONT_KEY_GOTHIC_28 },
 { .name = "Gothic", .variant = "28 Bold", .res = FONT_KEY_GOTHIC_28_BOLD },

 { .name = "Bitham", .variant = "30 Black", .res = FONT_KEY_BITHAM_30_BLACK },
 { .name = "Bitham", .variant = "42 Bold", .res = FONT_KEY_BITHAM_42_BOLD },
 { .name = "Bitham", .variant = "42 Light", .res = FONT_KEY_BITHAM_42_LIGHT },

 { .name = "Bitham", .variant = "34 Medium Numbers", .res = FONT_KEY_BITHAM_34_MEDIUM_NUMBERS },
 { .name = "Bitham", .variant = "42 Medium Numbers", .res = FONT_KEY_BITHAM_42_MEDIUM_NUMBERS },

 { .name = "Roboto", .variant = "21 Condensed", .res = FONT_KEY_ROBOTO_CONDENSED_21 },
 { .name = "Roboto", .variant = "49 Bold Subset", .res = FONT_KEY_ROBOTO_BOLD_SUBSET_49 },
 { .name = "Droid",  .variant = "28 Bold", .res = FONT_KEY_DROID_SERIF_28_BOLD }
};

static char *s_messages[] = {
  "0123456789",
  "abcdef ABCDEF",
  "09:42"
};

static Window *s_main_window, *s_font_window;
static MenuLayer *s_menu_layer;
static TextLayer *s_text_layer, *s_font_name_layer, *s_font_variant_layer, *s_font_size_layer;

/* Store the index of the currently selected font and text message. */
static int s_current_font;
static int s_current_message;

/* Text buffer for the size required to display font and message */
static char s_size_text[256] = "size";

static uint16_t get_num_rows(struct MenuLayer* menu_layer, uint16_t section_index, void *callback_context) {
  return NUM_FONTS;
}

static void draw_row(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context) {
  PebbleFont *font_array = (PebbleFont*) callback_context;
  PebbleFont *this_font = &font_array[cell_index->row];

  menu_cell_basic_draw(ctx, cell_layer, this_font->name, this_font->variant, NULL);
}

static void select_click(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  s_current_font = cell_index->row;

  window_stack_push(s_font_window, true);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  s_menu_layer = menu_layer_create(layer_get_bounds(window_layer));
  menu_layer_set_callbacks(s_menu_layer, pebble_fonts, (MenuLayerCallbacks) {
    .get_num_rows = get_num_rows,
    .draw_row = draw_row,
    .select_click = select_click
  });
  menu_layer_set_click_config_onto_window(s_menu_layer, window);

  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}

static void main_window_unload(Window *window) {
  menu_layer_destroy(s_menu_layer);
}

/*
 * Update the font window with the currently selected font.
 *
 * This is called:
 *  - When the s_font_window is initialized
 *  - When the user presses up/down/select in the font window
 */
static void show_selected_font_and_message() {
  PebbleFont *font = &pebble_fonts[s_current_font];

  // Update the font and text for the demo message
  text_layer_set_font(s_text_layer, fonts_get_system_font(font->res));
  text_layer_set_text(s_text_layer, s_messages[s_current_message]);

  // Update the font name and font variant at the bottom of the screen
  text_layer_set_text(s_font_name_layer, font->name);
  text_layer_set_text(s_font_variant_layer, font->variant);

  // Update Font Size Layer
  GSize textSize = text_layer_get_content_size(s_text_layer);
  snprintf(s_size_text, sizeof(s_size_text), "H: %d W: %d", textSize.h, textSize.w);

  text_layer_set_text(s_font_size_layer, s_size_text);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_current_message++;
  if ((unsigned)s_current_message >= NUM_MESSAGES) {
    s_current_message = 0;
  }

  show_selected_font_and_message();
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_current_font--;
  if (s_current_font < 0) {
    s_current_font = NUM_FONTS - 1;
  }

  MenuIndex idx = menu_layer_get_selected_index(s_menu_layer);
  idx.row = s_current_font;
  menu_layer_set_selected_index(s_menu_layer, idx, MenuRowAlignCenter, false);
  
  show_selected_font_and_message();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_current_font++;
  if ((unsigned)s_current_font >= NUM_FONTS) {
    s_current_font = 0;
  }

  MenuIndex idx = menu_layer_get_selected_index(s_menu_layer);
  idx.row = s_current_font;
  menu_layer_set_selected_index(s_menu_layer, idx, MenuRowAlignCenter, false);
  
  show_selected_font_and_message();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler)select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler)up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler)down_click_handler);
}

static void font_window_load(Window *window) {
  // Define some event handlers for clicks
  window_set_click_config_provider(window, (ClickConfigProvider) click_config_provider);

  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  // Arrange the three text layers on top of each other
  window_bounds.size.h -= 40;
  s_text_layer = text_layer_create(window_bounds);

  window_bounds.origin.x = 2;
  window_bounds.size.w -= 4;

  window_bounds.origin.y += window_bounds.size.h;
  window_bounds.size.h = 18;

  s_font_name_layer = text_layer_create(window_bounds);
  text_layer_set_font(s_font_name_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));

  window_bounds.origin.y += window_bounds.size.h;
  s_font_variant_layer = text_layer_create(window_bounds);
  text_layer_set_font(s_font_variant_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));

  window_bounds.origin.y -= 2.0 * window_bounds.size.h;
  s_font_size_layer = text_layer_create(window_bounds);
  text_layer_set_font(s_font_size_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));

  // Add the child layer to the current window (s_font_window)
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_font_name_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_font_variant_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_font_size_layer));

  // Finally, update the text and font in the layers
  show_selected_font_and_message();
}

static void font_window_unload(Window *window) {
  layer_remove_child_layers(window_get_root_layer(window));

  text_layer_destroy(s_text_layer);
  text_layer_destroy(s_font_name_layer);
  text_layer_destroy(s_font_variant_layer);
}

static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  s_font_window = window_create();
  window_set_window_handlers(s_font_window, (WindowHandlers) {
    .load = font_window_load,
    .unload = font_window_unload
  });

  window_stack_push(s_main_window, true);
}

static void deinit() {
  window_destroy(s_font_window);
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
