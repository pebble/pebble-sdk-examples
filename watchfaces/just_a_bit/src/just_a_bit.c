/*

  Just A Bit watch.

  A "binary" clock.

  See:

    <http://en.wikipedia.org/wiki/Binary_clock#Binary-coded_decimal_clocks>

 */

#include "pebble.h"

Window *window;

Layer *display_layer;


#define CIRCLE_RADIUS 12
#define CIRCLE_LINE_THICKNESS 2

void draw_cell(GContext* ctx, GPoint center, bool filled) {
  // Each "cell" represents a binary digit or 0 or 1.

  graphics_context_set_fill_color(ctx, GColorWhite);

  graphics_fill_circle(ctx, center, CIRCLE_RADIUS);

  if (!filled) {
    // This is our ghetto way of drawing circles with a line thickness
    // of more than a single pixel.
    graphics_context_set_fill_color(ctx, GColorBlack);

    graphics_fill_circle(ctx, center, CIRCLE_RADIUS - CIRCLE_LINE_THICKNESS);
  }

}


#define CIRCLE_PADDING 14 - CIRCLE_RADIUS // Number of padding pixels on each side
#define CELL_SIZE (2 * (CIRCLE_RADIUS + CIRCLE_PADDING)) // One "cell" is the square that contains the circle.
#define SIDE_PADDING (144 - (4 * CELL_SIZE))/2

#define CELLS_PER_ROW 4
#define CELLS_PER_COLUMN 6


GPoint get_center_point_from_cell_location(unsigned short x, unsigned short y) {
  // Cell location (0,0) is upper left, location (4, 6) is lower right.
  return GPoint(SIDE_PADDING + (CELL_SIZE/2) + (CELL_SIZE * x),
		(CELL_SIZE/2) + (CELL_SIZE * y));
}

void draw_cell_row_for_digit(GContext* ctx, unsigned short digit, unsigned short max_columns_to_display, unsigned short cell_row) {
  // Converts the supplied decimal digit into Binary Coded Decimal form and
  // then draws a row of cells on screen--'1' binary values are filled, '0' binary values are not filled.
  // `max_columns_to_display` restricts how many binary digits are shown in the row.
  for (int cell_column_index = 0; cell_column_index < max_columns_to_display; cell_column_index++) {
    draw_cell(ctx, get_center_point_from_cell_location(cell_column_index, cell_row), (digit >> cell_column_index) & 0x1);
  }
}


// The cell row offsets for each digit
#define HOURS_FIRST_DIGIT_ROW 0
#define HOURS_SECOND_DIGIT_ROW 1
#define MINUTES_FIRST_DIGIT_ROW 2
#define MINUTES_SECOND_DIGIT_ROW 3
#define SECONDS_FIRST_DIGIT_ROW 4
#define SECONDS_SECOND_DIGIT_ROW 5

// The maximum number of cell columns to display
// (Used so that if a binary digit can never be 1 then no un-filled
// placeholder is shown.)
#define DEFAULT_MAX_COLS 4
#define HOURS_FIRST_DIGIT_MAX_COLS 2
#define MINUTES_FIRST_DIGIT_MAX_COLS 3
#define SECONDS_FIRST_DIGIT_MAX_COLS 3


unsigned short get_display_hour(unsigned short hour) {

  if (clock_is_24h_style()) {
    return hour;
  }

  unsigned short display_hour = hour % 12;

  // Converts "0" to "12"
  return display_hour ? display_hour : 12;

}


void display_layer_update_callback(Layer *me, GContext* ctx) {

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

void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(display_layer);
}


static void do_init(void) {
  window = window_create();
  window_stack_push(window, true);

  window_set_background_color(window, GColorBlack);

  Layer *root_layer = window_get_root_layer(window);
  GRect frame = layer_get_frame(root_layer);

  // Init the layer for the display
  display_layer = layer_create(frame);
  layer_set_update_proc(display_layer, &display_layer_update_callback);
  layer_add_child(root_layer, display_layer);

  tick_timer_service_subscribe(SECOND_UNIT, &handle_second_tick);
}

static void do_deinit(void) {
  layer_destroy(display_layer);
  window_destroy(window);
}


int main(void) {
  do_init();
  app_event_loop();
  do_deinit();
}
