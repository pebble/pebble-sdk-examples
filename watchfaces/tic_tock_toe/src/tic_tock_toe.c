#include "pebble.h"

#define INVERT_COLORS

#ifndef INVERT_COLORS
#define COLOR_FOREGROUND GColorBlack
#define COLOR_BACKGROUND GColorWhite
#else
#define COLOR_FOREGROUND GColorWhite
#define COLOR_BACKGROUND GColorBlack
#endif

#define DRAW_WIN_LINE 10
#define DRAW_BLANK 11

//
// Board location offsets for the move sequence arrays:
//
//  0 | 1 | 2
//  3 | 4 | 5
//  6 | 7 | 8
//
// Sequences of moves needed to get the specified game result
// Each array position holds the location offset of the active player's move.
// Currently "Player 1" is "O" and "Player 2" is "X"
const unsigned short SEQUENCE_FIRST_PLAYER_WINS[] = {DRAW_BLANK, 0, 8, 2, 1, 6, 3, 4, DRAW_WIN_LINE, DRAW_BLANK};
const unsigned short SEQUENCE_SECOND_PLAYER_WINS[] = {2, 8, 5, 6, 7, 0, 4, 3, DRAW_WIN_LINE, DRAW_BLANK};
const unsigned short SEQUENCE_NEITHER_PLAYER_WINS[] = {4, 2, 7, 1, 0, 8, 5, 3, 6, DRAW_BLANK};

typedef struct {
  const unsigned short *SEQUENCE;
  unsigned short sequence_offset;
} GameState;

static Window *s_main_window; 
static TextLayer *s_time_layer;
static Layer *s_board_layer, *s_players_layer; 

void board_layer_update_callback(Layer *layer, GContext* ctx) {
  graphics_context_set_stroke_color(ctx, COLOR_FOREGROUND);

  // Vertical lines
  graphics_draw_line(ctx, GPoint(54, 19), GPoint(54, 123));
  graphics_draw_line(ctx, GPoint(89, 19), GPoint(89, 123));

  // Horizontal lines
  graphics_draw_line(ctx, GPoint(19, 54), GPoint(123, 54));
  graphics_draw_line(ctx, GPoint(19, 89), GPoint(123, 89));
}

void graphics_draw_line_wide(GContext *ctx, GPoint p0, GPoint p1) {
  for (int x_offset = -1; x_offset < 2; x_offset++) {
    graphics_draw_line(ctx, GPoint(p0.x + x_offset, p0.y), GPoint(p1.x + x_offset, p1.y));
  }
}

void draw_circle_player(GContext* ctx, GPoint center) {
  graphics_context_set_fill_color(ctx, COLOR_FOREGROUND);
  graphics_fill_circle(ctx, center, 15);
  graphics_context_set_fill_color(ctx, COLOR_BACKGROUND);
  graphics_fill_circle(ctx, center, 13);
}

void draw_cross_player(GContext* ctx, GPoint center) {
  graphics_context_set_stroke_color(ctx, COLOR_FOREGROUND);
  graphics_draw_line_wide(ctx, GPoint(center.x - 12, center.y - 12), GPoint(center.x+12, center.y + 12));
  graphics_draw_line_wide(ctx, GPoint(center.x - 12, center.y + 12), GPoint(center.x+12, center.y - 12));
}

GPoint get_cell_center(unsigned int cell_offset) {
  // Returns the centre coordinates of a cell location on the playing board.
  const unsigned short NUM_COLUMNS = 3;
  const unsigned short OFFSET_X = 37;
  const unsigned short OFFSET_Y = 37;
  const unsigned short CELL_SIZE = 35;

  return GPoint( ((cell_offset % NUM_COLUMNS) * CELL_SIZE) + OFFSET_X,
		 ((cell_offset / NUM_COLUMNS) * CELL_SIZE) + OFFSET_Y);
}

void retrieve_current_game_state(GameState *game_state) {
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);

  // The current location in the move sequence is synced to the current time
  // so that games end when the clock digits change.
  game_state->sequence_offset = (current_time->tm_sec + 8) % 10;

  // The game sequence we are displaying depends on which digit (i.e. hours or minutes)
  // will change next.
  if ((current_time->tm_sec >= 52) || (current_time->tm_sec <= 1)) {
    // Because sequences run over the start of minutes we need to check both
    // the current minute and the current second to know which is the correct
    // sequence to display.
    if (((current_time->tm_min == 59) && (current_time->tm_sec >= 52))
	   || ((current_time->tm_min == 0) && (current_time->tm_sec <= 1)) ) {
      // Because it will be a new hour at the end of this time period.
      game_state->SEQUENCE = SEQUENCE_FIRST_PLAYER_WINS;
    } else {
      // Because it will be a new minute at the end of this time period.
      game_state->SEQUENCE = SEQUENCE_SECOND_PLAYER_WINS;
    }
  } else {
    // Neither the minute nor hour digit will change at the end of this time period.
    game_state->SEQUENCE = SEQUENCE_NEITHER_PLAYER_WINS;
  }
}

void draw_win_line(GContext* ctx, GPoint left_most_point, GPoint right_most_point) {
  // Make the ends of the line extend past the edges of the board.
  if (left_most_point.x != right_most_point.x) {
    // It's not a vertical line so adjust the X axis.
    left_most_point.x -= 18;
    right_most_point.x += 18;
  }

  if (left_most_point.y != right_most_point.y) {
    // It's not a horizontal line so adjust the Y axis.
    if (left_most_point.y < right_most_point.y) {
      left_most_point.y -= 18;
      right_most_point.y += 18;
    } else {
      left_most_point.y += 18;
      right_most_point.y -= 18;
    }
  }

  graphics_draw_line_wide(ctx, left_most_point, right_most_point);
}

void draw_game_state(GContext* ctx, const GameState game_state) {
  // Note: Because the layer is blanked before we're called, we also need to
  //       redraw all the player moves preceeding the current move each time
  //       we're called.

  // If we've reached the final part of the sequence don't draw anything.
  if (game_state.SEQUENCE[game_state.sequence_offset] == DRAW_BLANK) {
    return;
  }

  unsigned short s_active_player = 0;

  for (unsigned short current_offset = 0; current_offset <= game_state.sequence_offset; current_offset++) {
    switch (game_state.SEQUENCE[current_offset]) {
      case DRAW_BLANK:
        // Do nothing as a DRAW_BLANK value in a non terminal position is just
        // padding so the sequence has the correct length.
      	break;
      case DRAW_WIN_LINE:
      	if (game_state.SEQUENCE == SEQUENCE_SECOND_PLAYER_WINS) {
      	  draw_win_line(ctx, get_cell_center(0), get_cell_center(6));
      	} else { 
          // Assumes first player wins
      	  draw_win_line(ctx, get_cell_center(6), get_cell_center(2));
      	}
      	break;
      default: {
      	// The value is a cell location offset.
      	// Draw the marker for the current player who is active in this turn.
        (s_active_player ? draw_cross_player : draw_circle_player)(ctx, get_cell_center(game_state.SEQUENCE[current_offset]));
        s_active_player = (s_active_player + 1) % 2;
      	break;
      }
    }
  }
}

void players_layer_update_callback(Layer *layer, GContext* ctx) {
#ifdef INVERT_COLORS
  graphics_context_set_compositing_mode(ctx, GCompOpAssignInverted);
#endif

  GameState game_state;
  retrieve_current_game_state(&game_state);
  draw_game_state(ctx, game_state);
}

void update_time_text() {
  time_t now = time(NULL);
  const struct tm *current_time = localtime(&now);
  
  char *time_format;
  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }

  static char s_time_text[] = "00:00";
  strftime(s_time_text, sizeof(s_time_text), time_format, current_time);

  // Handle lack of non-padded hour format string for twelve hour clock.
  if (!clock_is_24h_style() && (s_time_text[0] == '0')) {
    memmove(s_time_text, &s_time_text[1], sizeof(s_time_text) - 1);
  }
  text_layer_set_text(s_time_layer, s_time_text);

  layer_mark_dirty(s_players_layer);
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  update_time_text();
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  // Init the layer that shows the board
  s_board_layer = layer_create(bounds); 
  layer_set_update_proc(s_board_layer, board_layer_update_callback); 
  layer_add_child(window_layer, s_board_layer);

  // Init the layer that shows the player marks
  s_players_layer = layer_create(bounds);
  layer_set_update_proc(s_players_layer, players_layer_update_callback);
  layer_add_child(window_layer, s_players_layer);

  // Init the text layer used to show the time
  s_time_layer = text_layer_create(GRect(0, 126, 144, 42));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_text_color(s_time_layer, COLOR_FOREGROUND);
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_time_layer);

  layer_destroy(s_board_layer);
  layer_destroy(s_players_layer);
}

static void init() {
  s_main_window = window_create();
  window_set_background_color(s_main_window, COLOR_BACKGROUND);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);

  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);

  update_time_text();
}

static void deinit() {
  window_destroy(s_main_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}
