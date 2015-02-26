#include <pebble.h>

#define TEA_TEXT_GAP 14

static Window *s_menu_window, *s_countdown_window, *s_wakeup_window;
static MenuLayer *s_menu_layer;
static TextLayer *s_error_text_layer, *s_tea_text_layer, *s_countdown_text_layer, *s_cancel_text_layer;
static BitmapLayer *s_bitmap_layer;
static GBitmap *s_tea_bitmap;

static WakeupId s_wakeup_id = -1;
static time_t s_wakeup_timestamp = 0;
static char s_tea_text[32];
static char s_countdown_text[32];

typedef struct {
  char name[16];  // Name of this tea
  int mins;       // Minutes to steep this tea
} TeaInfo;

// Array of different teas for tea timer
// {<Tea Name>, <Brew time in minutes>}
TeaInfo tea_array[] = {
  {"Green Tea", 1},
  {"Black Tea", 2},
  {"Oolong Tea", 3},
  {"Darjeeling", 4},
  {"Herbal Tea", 5},
  {"Mate Tea", 6},
  {"Chai Tea", 10}
};

enum {
  PERSIST_WAKEUP // Persistent storage key for wakeup_id
};

static void select_callback(struct MenuLayer *s_menu_layer, MenuIndex *cell_index, void *callback_context) {
  // If we were displaying s_error_text_layer, remove it and return
  if (!layer_get_hidden(text_layer_get_layer(s_error_text_layer))) {
    layer_set_hidden(text_layer_get_layer(s_error_text_layer), true);
    return;
  }

  // Wakeup time is a timestamp in the future
  // so time(NULL) + delay_time_in_seconds = wakeup_time
  time_t wakeup_time = time(NULL) + tea_array[cell_index->row].mins * 60;

  // Use the tea_array index as the wakeup reason, so on wakeup trigger
  // we know which tea is brewed
  s_wakeup_id = wakeup_schedule(wakeup_time, cell_index->row, true);

  // If we couldn't schedule the wakeup event, display error_text overlay
  if (s_wakeup_id <= 0) {
    layer_set_hidden(text_layer_get_layer(s_error_text_layer), false);
    return;
  }

  // Store the handle so we can cancel if necessary, or look it up next launch
  persist_write_int(PERSIST_WAKEUP, s_wakeup_id);

  // Switch to countdown window
  window_stack_push(s_countdown_window, false);
}

static uint16_t get_sections_count_callback(struct MenuLayer *menulayer, uint16_t section_index, void *callback_context) {
  int count = sizeof(tea_array) / sizeof(TeaInfo);
  return count;
}

static void draw_row_handler(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context) {
  char* name = tea_array[cell_index->row].name;
  int text_gap_size = TEA_TEXT_GAP - strlen(name);
  int mins = tea_array[cell_index->row].mins;

  // Using simple space padding between name and s_tea_text for appearance of edge-alignment
  snprintf(s_tea_text, sizeof(s_tea_text), "%s%*s%d min", name, text_gap_size, "", mins);

  menu_cell_basic_draw(ctx, cell_layer, s_tea_text, NULL, NULL);
}

static void menu_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_menu_layer = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_rows = get_sections_count_callback,
    .draw_row = draw_row_handler,
    .select_click = select_callback
  }); 
  menu_layer_set_click_config_onto_window(s_menu_layer,	window);
  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));

  s_error_text_layer = text_layer_create((GRect) { .origin = {0, 44}, .size = {bounds.size.w, 60}});
  text_layer_set_text(s_error_text_layer, "Cannot\nschedule");
  text_layer_set_text_alignment(s_error_text_layer, GTextAlignmentCenter);
  text_layer_set_font(s_error_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_color(s_error_text_layer, GColorWhite);
  text_layer_set_background_color(s_error_text_layer, GColorBlack);
  layer_set_hidden(text_layer_get_layer(s_error_text_layer), true);
  layer_add_child(window_layer, text_layer_get_layer(s_error_text_layer));
}

static void menu_window_unload(Window *window) {
  menu_layer_destroy(s_menu_layer);
  text_layer_destroy(s_error_text_layer);
}

static void timer_handler(void *data) {
  if (s_wakeup_timestamp == 0) {
    // get the wakeup timestamp for showing a countdown
    wakeup_query(s_wakeup_id, &s_wakeup_timestamp);
  }
  int countdown = s_wakeup_timestamp - time(NULL);
  snprintf(s_countdown_text, sizeof(s_countdown_text), "%d seconds", countdown);
  layer_mark_dirty(text_layer_get_layer(s_countdown_text_layer));
  app_timer_register(1000, timer_handler, data);
}

static void countdown_back_handler(ClickRecognizerRef recognizer, void *context) {
  window_stack_pop_all(true); // Exit app while waiting for tea to brew
}

// Cancel the current wakeup event on the countdown screen
static void countdown_cancel_handler(ClickRecognizerRef recognizer, void *context) {
  wakeup_cancel(s_wakeup_id);
  s_wakeup_id = -1;
  persist_delete(PERSIST_WAKEUP);
  window_stack_pop(true); // Go back to tea selection window
}

static void countdown_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_BACK, countdown_back_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, countdown_cancel_handler);
}

static void countdown_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  window_set_click_config_provider(window, countdown_click_config_provider);

  s_tea_text_layer = text_layer_create(GRect(0, 32, bounds.size.w, 20));
  text_layer_set_text(s_tea_text_layer, "Steeping time left");
  text_layer_set_text_alignment(s_tea_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_tea_text_layer));

  s_countdown_text_layer = text_layer_create(GRect(0, 72, bounds.size.w, 20));
  text_layer_set_text(s_countdown_text_layer, s_countdown_text);
  text_layer_set_text_alignment(s_countdown_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_countdown_text_layer));

  // Place a cancel "X" next to the bottom button to cancel wakeup timer
  s_cancel_text_layer = text_layer_create(GRect(124, 116, 24, 28));
  text_layer_set_text(s_cancel_text_layer, "X");
  text_layer_set_font(s_cancel_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(s_cancel_text_layer, GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(s_cancel_text_layer));

  s_wakeup_timestamp = 0;
  app_timer_register(0, timer_handler, NULL);
}

static void countdown_window_unload(Window *window) {
  text_layer_destroy(s_countdown_text_layer);
  text_layer_destroy(s_cancel_text_layer);
  text_layer_destroy(s_tea_text_layer);
}

static void wakeup_click_handler(ClickRecognizerRef recognizer, void *context) {
  // Exit app after tea is done
  window_stack_pop_all(true);
}

static void wakeup_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, wakeup_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, wakeup_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, wakeup_click_handler);
  window_single_click_subscribe(BUTTON_ID_BACK, wakeup_click_handler);
}

static void wakeup_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  window_set_click_config_provider(window, wakeup_click_config_provider);

  // Bitmap layer for wakeup "tea is ready" image
  s_bitmap_layer = bitmap_layer_create(bounds);
  s_tea_bitmap = gbitmap_create_with_resource(RESOURCE_ID_TEA_SIGN);
  bitmap_layer_set_bitmap(s_bitmap_layer, s_tea_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_layer));
}

static void wakeup_window_unload(Window *window) {
  gbitmap_destroy(s_tea_bitmap);
  bitmap_layer_destroy(s_bitmap_layer);
}

static void wakeup_handler(WakeupId id, int32_t reason) {
  //Delete persistent storage value
  persist_delete(PERSIST_WAKEUP);
  window_stack_push(s_wakeup_window, false);
  vibes_double_pulse();
}

static void init(void) {
  bool wakeup_scheduled = false;

  // Check if we have already scheduled a wakeup event
  // so we can transition to the countdown window
  if (persist_exists(PERSIST_WAKEUP)) {
    s_wakeup_id = persist_read_int(PERSIST_WAKEUP);
    // query if event is still valid, otherwise delete
    if (wakeup_query(s_wakeup_id, NULL)) {
      wakeup_scheduled = true;
    } else {
      persist_delete(PERSIST_WAKEUP);
      s_wakeup_id = -1;
    }
  }

  s_menu_window = window_create();
  window_set_window_handlers(s_menu_window, (WindowHandlers){
    .load = menu_window_load,
    .unload = menu_window_unload,
  });

  s_countdown_window = window_create();
  window_set_window_handlers(s_countdown_window, (WindowHandlers){
    .load = countdown_window_load,
    .unload = countdown_window_unload,
  });

  s_wakeup_window = window_create();
  window_set_window_handlers(s_wakeup_window, (WindowHandlers){
    .load = wakeup_window_load,
    .unload = wakeup_window_unload,
  });

  // Check to see if we were launched by a wakeup event
  if (launch_reason() == APP_LAUNCH_WAKEUP) {
    // If woken by wakeup event, get the event display "tea is ready"
    WakeupId id = 0;
    int32_t reason = 0;
    if (wakeup_get_launch_event(&id, &reason)) {
      wakeup_handler(id, reason);
    }
  } else if (wakeup_scheduled) {
    window_stack_push(s_countdown_window, false);
  } else {
    window_stack_push(s_menu_window, false);
  }

  // subscribe to wakeup service to get wakeup events while app is running
  wakeup_service_subscribe(wakeup_handler);
}

static void deinit(void) {
  window_destroy(s_menu_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
