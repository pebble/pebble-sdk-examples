#include "pebble.h"


static Window *window;
static MenuLayer *menu_layer;

enum {
  TODO_KEY_APPEND,
  TODO_KEY_DELETE,
  TODO_KEY_MOVE,
  TODO_KEY_TOGGLE_STATE,
  TODO_KEY_FETCH,
};

#define MAX_TODO_LIST_ITEMS (10)
#define MAX_ITEM_TEXT_LENGTH (16)

typedef enum {
  TodoListItemStateIncomplete = 0x00,
  TodoListItemStateComplete = 0x01,
} TodoListItemState;

typedef struct {
  TodoListItemState state;
  char text[MAX_ITEM_TEXT_LENGTH];
} TodoListItem;

static TodoListItem s_todo_list_items[MAX_TODO_LIST_ITEMS];
static int s_active_item_count = 0;

static TodoListItem* get_todo_list_item_at_index(int index) {
  if (index < 0 || index >= MAX_TODO_LIST_ITEMS) {
    return NULL;
  }

  return &s_todo_list_items[index];
}

static void todo_list_append(char *data) {
  if (s_active_item_count == MAX_TODO_LIST_ITEMS) { 
    return;
  }

  s_todo_list_items[s_active_item_count].state = TodoListItemStateIncomplete;
  strcpy(s_todo_list_items[s_active_item_count].text, data);
  s_active_item_count++;
}

static void todo_list_delete(uint8_t list_idx) {
  if (s_active_item_count < 1) {
    return;
  }

  s_active_item_count--;

  memmove(&s_todo_list_items[list_idx], &s_todo_list_items[list_idx + 1],
      ((s_active_item_count - list_idx) * sizeof(TodoListItem)));
}

static void todo_list_insert(uint8_t list_idx, TodoListItem *item) {
  if (s_active_item_count == MAX_TODO_LIST_ITEMS) {
    return;
  }

  memmove(&s_todo_list_items[list_idx + 1], &s_todo_list_items[list_idx],
      ((s_active_item_count - list_idx) * sizeof(TodoListItem)));
  s_todo_list_items[list_idx] = *item;
  s_active_item_count++;
}

static void todo_list_move(uint8_t first_idx, uint8_t second_idx) {
  if (first_idx >= s_active_item_count ||
      second_idx >= s_active_item_count ||
      first_idx == second_idx) {
    return;
  }

  TodoListItem temp_item = s_todo_list_items[first_idx];
  todo_list_delete(first_idx);
  todo_list_insert(second_idx, &temp_item);
}

static void todo_list_toggle_state(uint8_t list_idx) {
  if (list_idx >= s_active_item_count) {
    return;
  }
  s_todo_list_items[list_idx].state ^= 0x01;
}

static void todo_list_init(void) {
  DictionaryIterator *iter;

  if (app_message_outbox_begin(&iter) != APP_MSG_OK) {
    return;
  }
  if (dict_write_uint8(iter, TODO_KEY_FETCH, 0) != DICT_OK) {
    return;
  }
  app_message_outbox_send();
}

static int16_t get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  return 44;
}

static void draw_strikethrough_on_item(GContext* ctx, Layer* cell_layer, TodoListItem* item) {
  graphics_context_set_compositing_mode(ctx, GCompOpClear);
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_fill_color(ctx, GColorBlack);

  GRect cell_bounds = layer_get_frame(cell_layer);

  static const int menu_cell_margin = 5;
  GSize text_cell_size = cell_bounds.size;
  text_cell_size.w -= 2 * menu_cell_margin;

  GRect text_cell_rect;
  text_cell_rect.origin = GPointZero;
  text_cell_rect.size = text_cell_size;

  GSize max_used_size = graphics_text_layout_get_max_used_size(ctx,
      item->text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), text_cell_rect,
      GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);

  GRect strike_through;
  strike_through.origin = cell_bounds.origin;
  strike_through.origin.x += menu_cell_margin;
  strike_through.origin.y += cell_bounds.size.h / 2;
  strike_through.size = (GSize) { max_used_size.w, 2 };

  // Stretch the strikethrough to be slightly wider than the text
  static const int pixel_nudge = 2;
  strike_through.origin.x -= pixel_nudge;
  strike_through.size.w += 2 * pixel_nudge;

  graphics_fill_rect(ctx, strike_through, 0, GCornerNone);
}

static void draw_row_callback(GContext* ctx, Layer *cell_layer, MenuIndex *cell_index, void *data) {
  TodoListItem* item;
  const int index = cell_index->row;

  if ((item = get_todo_list_item_at_index(index)) == NULL) {
    return;
  }

  menu_cell_basic_draw(ctx, cell_layer, item->text, NULL, NULL);
  if (item->state == TodoListItemStateComplete) {
    draw_strikethrough_on_item(ctx, cell_layer, item);
  }
}

static uint16_t get_num_rows_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return s_active_item_count;
}

static void select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  const int index = cell_index->row;
  DictionaryIterator *iter;

  if (app_message_outbox_begin(&iter) != APP_MSG_OK) {
    return;
  }
  todo_list_toggle_state(index);
  if (dict_write_uint8(iter, TODO_KEY_TOGGLE_STATE, index) != DICT_OK) {
    return;
  }
  app_message_outbox_send();

  menu_layer_reload_data(menu_layer);
}

static void select_long_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  const int index = cell_index->row;
  DictionaryIterator *iter;

  if (app_message_outbox_begin(&iter) != APP_MSG_OK) {
    return;
  }
  todo_list_delete(index);
  if (dict_write_uint8(iter, TODO_KEY_DELETE, index) != DICT_OK) {
    return;
  }
  app_message_outbox_send();

  menu_layer_reload_data(menu_layer);
}

static void window_load(Window* window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_frame = layer_get_frame(window_layer);
  menu_layer = menu_layer_create(window_frame);
  menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks) {
    .get_cell_height = (MenuLayerGetCellHeightCallback) get_cell_height_callback,
    .draw_row = (MenuLayerDrawRowCallback) draw_row_callback,
    .get_num_rows = (MenuLayerGetNumberOfRowsInSectionsCallback) get_num_rows_callback,
    .select_click = (MenuLayerSelectCallback) select_callback,
    .select_long_click = (MenuLayerSelectCallback) select_long_callback
  });
  menu_layer_set_click_config_onto_window(menu_layer, window);
  layer_add_child(window_layer, menu_layer_get_layer(menu_layer));
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *append_tuple = dict_find(iter, TODO_KEY_APPEND);
  Tuple *delete_tuple = dict_find(iter, TODO_KEY_DELETE);
  Tuple *move_tuple = dict_find(iter, TODO_KEY_MOVE);
  Tuple *toggle_state_tuple = dict_find(iter, TODO_KEY_TOGGLE_STATE);

  if (append_tuple) {
    todo_list_append(append_tuple->value->cstring);
  }
  if (delete_tuple) {
    todo_list_delete(delete_tuple->value->uint8);
  }
  if (move_tuple) {
    todo_list_move(move_tuple->value->data[0], move_tuple->value->data[1]);
  }
  if (toggle_state_tuple) {
    todo_list_toggle_state(toggle_state_tuple->value->uint8);
  }

  menu_layer_reload_data(menu_layer);
}

static void app_message_init(void) {
  // Reduce the sniff interval for more responsive messaging at the expense of
  // increased energy consumption by the Bluetooth module
  // The sniff interval will be restored by the system after the app has been
  // unloaded
  app_comm_set_sniff_interval(SNIFF_INTERVAL_REDUCED);
  // Init buffers
  app_message_open(64, 16);
  // Register message handlers
  app_message_register_inbox_received(in_received_handler);
}

int main(void) {
  window = window_create();

  app_message_init();
  todo_list_init();

  // configure window
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
  });
  window_stack_push(window, true /* Animated */);

  // init done, start event loop
  app_event_loop();

  // delete objects
  window_destroy(window);
  menu_layer_destroy(menu_layer);
}
