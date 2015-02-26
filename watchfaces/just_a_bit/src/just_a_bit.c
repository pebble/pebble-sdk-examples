#include <pebble.h>

#define CIRCLE_RADIUS 12
#define CIRCLE_LINE_THICKNESS 2
#define CIRCLE_PADDING 14 - CIRCLE_RADIUS
#define CELL_SIZE (2 * (CIRCLE_RADIUS + CIRCLE_PADDING))
#define SIDE_PADDING (144 - (4 * CELL_SIZE)) / 2

#define CELLS_PER_ROW 4
#define CELLS_PER_COLUMN 6

#define HOURS_FIRST_DIGIT_ROW 0
#define HOURS_SECOND_DIGIT_ROW 1
#define MINUTES_FIRST_DIGIT_ROW 2
#define MINUTES_SECOND_DIGIT_ROW 3
#define SECONDS_FIRST_DIGIT_ROW 4
#define SECONDS_SECOND_DIGIT_ROW 5

#define DEFAULT_MAX_COLS 4
#define HOURS_FIRST_DIGIT_MAX_COLS 2
#define MINUTES_FIRST_DIGIT_MAX_COLS 3
#define SECONDS_FIRST_DIGIT_MAX_COLS 3

static Window *s_main_window;
static Layer *s_display_layer;

static void draw_cell(GContext *ctx, GPoint center, bool filled) {
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, center, CIRCLE_RADIUS);

  if (!filled) {
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_circle(ctx, center, CIRCLE_RADIUS - CIRCLE_LINE_THICKNESS);
  }
}

static GPoint get_center_point_from_cell_location(unsigned short x, unsigned short y) {
  return GPoint(SIDE_PADDING + (CELL_SIZE / 2) + (CELL_SIZE * x), (CELL_SIZE / 2) + (CELL_SIZE * y));
}

static void draw_cell_row_for_digit(GContext *ctx, unsigned short digit, unsigned short max_columns_to_display, unsigned short cell_row) {
  // Converts the supplied decimal digit into Binary Coded Decimal form and
  // then draws a row of cells on screen--'1' binary values are filled, '0' binary values are not filled.
  // `max_columns_to_display` restricts how many binary digits are shown in the row.
  for (int i = 0; i < max_columns_to_display; i++) {
    draw_cell(ctx, get_center_point_from_cell_location(i, cell_row), (digit >> i) & 0x1);
  }
}

static unsigned short get_display_hour(unsigned short hour) {
  if (clock_is_24h_style()) {
    return hour;
  }

  unsigned short display_hour = hour % 12;

  return display_hour ? display_hour : 12;
}

static void display_layer_update_callback(Layer *layer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  unsigned short display_hour = get_display_hour(t->tm_hour);

  draw_cell_row_for_digit(ctx, display_hour / 10, HOURS_FIRST_DIGIT_MAX_COLS, HOURS_FIRST_DIGIT_ROW);
  draw_cell_row_for_digit(ctx, display_hour % 10, DEFAULT_MAX_COLS, HOURS_SECOND_DIGIT_ROW);

  draw_cell_row_for_digit(ctx, t->tm_min / 10, MINUTES_FIRST_DIGIT_MAX_COLS, MINUTES_FIRST_DIGIT_ROW);
  draw_cell_row_for_digit(ctx, t->tm_min % 10, DEFAULT_MAX_COLS, MINUTES_SECOND_DIGIT_ROW);

  draw_cell_row_for_digit(ctx, t->tm_sec / 10, SECONDS_FIRST_DIGIT_MAX_COLS, SECONDS_FIRST_DIGIT_ROW);
  draw_cell_row_for_digit(ctx, t->tm_sec % 10, DEFAULT_MAX_COLS, SECONDS_SECOND_DIGIT_ROW);
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(s_display_layer);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  s_display_layer = layer_create(bounds);
  layer_set_update_proc(s_display_layer, display_layer_update_callback);
  layer_add_child(window_layer, s_display_layer);

  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
}

static void main_window_unload(Window *window) {
  layer_destroy(s_display_layer);
}

static void init() {
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);
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
