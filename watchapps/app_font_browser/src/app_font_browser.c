#include "pebble.h"


/*
 * List of Pebble's system fonts.
 */

typedef struct {
 char *name;
 char *variant;
 char *res;
} PebbleFont;

PebbleFont pebble_fonts[] = {
 { .name = "Gothic",   .variant = "14", .res = FONT_KEY_GOTHIC_14 },
 { .name = "Gothic",   .variant = "14 Bold", .res = FONT_KEY_GOTHIC_14_BOLD },
 { .name = "Gothic",   .variant = "18", .res = FONT_KEY_GOTHIC_18 },
 { .name = "Gothic",   .variant = "18 Bold", .res = FONT_KEY_GOTHIC_18_BOLD },
 { .name = "Gothic",   .variant = "24", .res = FONT_KEY_GOTHIC_24 },
 { .name = "Gothic",   .variant = "24 Bold", .res = FONT_KEY_GOTHIC_24_BOLD },
 { .name = "Gothic",   .variant = "28", .res = FONT_KEY_GOTHIC_28 },
 { .name = "Gothic",   .variant = "28 Bold", .res = FONT_KEY_GOTHIC_28_BOLD },

 { .name = "Bitham",   .variant = "30 Black", .res = FONT_KEY_BITHAM_30_BLACK },
 { .name = "Bitham",   .variant = "42 Bold", .res = FONT_KEY_BITHAM_42_BOLD },
 { .name = "Bitham",   .variant = "42 Light", .res = FONT_KEY_BITHAM_42_LIGHT },

 { .name = "Bitham",   .variant = "34 Medium Numbers", .res = FONT_KEY_BITHAM_34_MEDIUM_NUMBERS },
 { .name = "Bitham",   .variant = "42 Medium Numbers", .res = FONT_KEY_BITHAM_42_MEDIUM_NUMBERS },

 { .name = "Roboto",   .variant = "21 Condensed", .res = FONT_KEY_ROBOTO_CONDENSED_21 },
 { .name = "Roboto",   .variant = "49 Bold Subset", .res = FONT_KEY_ROBOTO_BOLD_SUBSET_49 },
 { .name = "Droid",    .variant = "28 Bold", .res = FONT_KEY_DROID_SERIF_28_BOLD }
};
#define NUM_FONTS sizeof(pebble_fonts) / sizeof(PebbleFont)


/*
 * List of messages to try the font with. The user can cycle through them.
 */
char *messages[] = {
  "0123456789",
  "abcdef ABCDEF",
  "09:42"
};
#define NUM_MESSAGES sizeof(messages) / sizeof(char*)


/*
 * Global and static UI elements.
 *
 * This app uses two window:
 *  - the main_window which uses a menu_layer to display a list of fonts;
 *  - the font_window.
 *
 * The font window uses three TextLayer:
 *  - to display a sample of text at the top of the screen (text_layer),
 *  - to display at the bottom of the screen the name (font_name_layer),
 *  - and variant (font_variant_layer) of the currently selected font.
 */
static Window *main_window;
static Window *font_window;
static MenuLayer *menu_layer;
static TextLayer *text_layer;
static TextLayer *font_name_layer;
static TextLayer *font_variant_layer;
static TextLayer *font_size_layer;

/* Store the index of the currently selected font and text message. */
static int current_font = 0;
static int current_message = 0;

/* Text buffer for the size required to display font and message */
static char size_text[256] = "size";

/** Menu layer callbacks **/

/*
 * Return the number of rows in the menu: one for each font.
 */
static uint16_t get_num_rows(struct MenuLayer* menu_layer, uint16_t section_index, void *callback_context) {
  return NUM_FONTS;
}

/*
 * Draw one row (ie: one font) using the convenience function menu_cell_basic_draw()
 */
static void draw_row(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context) {
  PebbleFont *fonts = (PebbleFont*) callback_context;
  PebbleFont *font = &fonts[cell_index->row];

  menu_cell_basic_draw(ctx, cell_layer, font->name, font->variant, NULL);
}

/*
 * When a window is clicked, update the currently selected row and push the font window to the front.
 */
static void select_click(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  current_font = cell_index->row;

  window_stack_push(font_window, true);
}

/* Group our callbacks in a MenuLayerCallbacks struct that we will pass to menu_layer_set_callbacks() */
MenuLayerCallbacks menu_callbacks = {
  .get_num_rows = get_num_rows,
  .draw_row = draw_row,
  .select_click = select_click
};


/*
 * Main window load callback
 */
static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  menu_layer = menu_layer_create(layer_get_bounds(window_layer));
  menu_layer_set_callbacks(menu_layer, pebble_fonts, menu_callbacks);
  menu_layer_set_click_config_onto_window(menu_layer, window);

  layer_add_child(window_layer, menu_layer_get_layer(menu_layer));
}

/*
 * Called when the main window is unloaded.
 */
static void main_window_unload(Window *window) {
  menu_layer_destroy(menu_layer);
}

/*
 * Update the font window with the currently selected font.
 *
 * This is called:
 *  - When the font_window is initialized
 *  - When the user presses up/down/select in the font window
 */
static void show_selected_font_and_message() {
  PebbleFont *font = &pebble_fonts[current_font];

  // Update the font and text for the demo message
  text_layer_set_font(text_layer, fonts_get_system_font(font->res));
  text_layer_set_text(text_layer, messages[current_message]);

  // Update the font name and font variant at the bottom of the screen
  text_layer_set_text(font_name_layer, font->name);
  text_layer_set_text(font_variant_layer, font->variant);

  // Update Font Size Layer
  GSize textSize = text_layer_get_content_size(text_layer);
  snprintf(size_text, sizeof(size_text), "H: %d W: %d", textSize.h, textSize.w);

  text_layer_set_text(font_size_layer, size_text);
}

/*
 * Callbacks for select / up / down button on the font window
 */

static void cycle_text_handler(ClickRecognizerRef recognizer, void *context) {
  current_message++;
  if ((unsigned)current_message >= NUM_MESSAGES) {
    current_message = 0;
  }
  show_selected_font_and_message();
}

static void select_previous_font_handler(ClickRecognizerRef recognizer, void *context) {
  current_font--;
  if (current_font < 0) {
    current_font = NUM_FONTS - 1;
  }
  MenuIndex idx = menu_layer_get_selected_index(menu_layer);
  idx.row = current_font;
  menu_layer_set_selected_index(menu_layer, idx, MenuRowAlignCenter, false);
  show_selected_font_and_message();
}

static void select_next_font_handler(ClickRecognizerRef recognizer, void *context) {
  current_font++;
  if ((unsigned)current_font >= NUM_FONTS) {
    current_font = 0;
  }
  MenuIndex idx = menu_layer_get_selected_index(menu_layer);
  idx.row = current_font;
  menu_layer_set_selected_index(menu_layer, idx, MenuRowAlignCenter, false);
  show_selected_font_and_message();
}

/*
 * Provides the click configuration for the font_window
 */
static void font_window_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler)cycle_text_handler);
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler)select_previous_font_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler)select_next_font_handler);
}

/*
 * Called every time the font window is loaded (pushed to front) to setup its layers.
 */
static void font_window_load(Window *window) {
  // Define some event handlers for clicks
  window_set_click_config_provider(window, (ClickConfigProvider) font_window_config_provider);

  Layer *window_layer = window_get_root_layer(window);

  // Arrange the three text layers on top of each other
  GRect text_bounds = layer_get_bounds(window_layer);
  text_bounds.size.h -= 40;
  text_layer = text_layer_create(text_bounds);

  text_bounds.origin.x = 2;
  text_bounds.size.w -= 4;

  text_bounds.origin.y += text_bounds.size.h;
  text_bounds.size.h = 18;

  font_name_layer = text_layer_create(text_bounds);
  text_layer_set_font(font_name_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));

  text_bounds.origin.y += text_bounds.size.h;
  font_variant_layer = text_layer_create(text_bounds);
  text_layer_set_font(font_variant_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));

  text_bounds.origin.y -= 2.0 * text_bounds.size.h;
  font_size_layer = text_layer_create(text_bounds);
  text_layer_set_font(font_size_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));

  // Add the child layer to the current window (font_window)
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
  layer_add_child(window_layer, text_layer_get_layer(font_name_layer));
  layer_add_child(window_layer, text_layer_get_layer(font_variant_layer));

  layer_add_child(window_layer, text_layer_get_layer(font_size_layer));

  // Finally, update the text and font in the layers
  show_selected_font_and_message();
}

/*
 * Called when the font window is unloaded.
 *
 * We need to remove the child layers here, or we would add them twice when
 * the window is reloaded (that would be bad!).
 */
static void font_window_unload(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  layer_remove_child_layers(window_layer);

  text_layer_destroy(text_layer);
  text_layer_destroy(font_name_layer);
  text_layer_destroy(font_variant_layer);
}

/*
 * Declare our handlers for both window
 */
static WindowHandlers main_window_handlers = {
  .load = main_window_load,
  .unload = main_window_unload
};
static WindowHandlers font_window_handlers = {
  .load = font_window_load,
  .unload = font_window_unload
};

/*
 * Initialize our app and both windows
 */
static void init() {
  current_font = 0;
  current_message = 0;

  main_window = window_create();
  window_set_window_handlers(main_window, main_window_handlers);

  font_window = window_create();
  window_set_window_handlers(font_window, font_window_handlers);

  window_stack_push(main_window, true /* Animated */);
}

/*
 * Deinitialize both windows
 */
static void deinit() {
  window_destroy(font_window);
  window_destroy(main_window);
}

/*
 * Our main function just calls initializers and the event loop.
 */
int main(void) {
  init();
  app_event_loop();
  deinit();
}
