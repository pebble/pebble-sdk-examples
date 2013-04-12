/*

   Tic Tock Toe watch for Pebble

 */

// Standard includes
#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID {0xAE, 0x4F, 0x3A, 0x10, 0xF0, 0x1C, 0x43, 0xE8, 0xB6, 0x94, 0x66, 0xD2, 0x6D, 0x30, 0xC7, 0x7D}
PBL_APP_INFO(MY_UUID, "Tic Tock Toe", "Pebble Technology", 4, 0, RESOURCE_ID_IMAGE_MENU_ICON, APP_INFO_WATCH_FACE);

//
// My understanding of graphics is that there are two approaches but only
// one is public currently:
//
//  * Create a "drawing" at initialisation time with GPath etc (with a
//    PathLayer--which isn't public yet?).
//
//  * Draw at screen update time in a layer update function using
//    `graphics_*` functions.
//
// So, for the moment we have to use the latter--hopefully this will change. :)
//

#define INVERT_COLORS

#ifndef INVERT_COLORS
#define COLOR_FOREGROUND GColorBlack
#define COLOR_BACKGROUND GColorWhite
#else
#define COLOR_FOREGROUND GColorWhite
#define COLOR_BACKGROUND GColorBlack
#endif

// App-specific data
Window window; // All apps must have at least one window

Layer boardLayer; // The board/grid
Layer playersLayer; // The player markers
TextLayer timeLayer; // The clock/"scoreboard"


// This is called whenever the `boardLayer` layer needs to be redrawn.
void boardLayer_update_callback(Layer *me, GContext* ctx) {
  (void)me; // Prevents "unused" warnings.

  graphics_context_set_stroke_color(ctx, COLOR_FOREGROUND);

  // Draw playing board grid
  // TODO: Change this to allow for a nicely centred circle in each square.

  // Vertical lines
  graphics_draw_line(ctx, GPoint(54, 19), GPoint(54, 123));
  graphics_draw_line(ctx, GPoint(89, 19), GPoint(89, 123));

  // Horizontal lines
  graphics_draw_line(ctx, GPoint(19, 54), GPoint(123, 54));
  graphics_draw_line(ctx, GPoint(19, 89), GPoint(123, 89));

}


void graphics_draw_line_wide(GContext* ctx, GPoint p0, GPoint p1) {
  // TODO: Replace this when lines can have width
  // NOTE: This fails for horizontal lines as it only "widens" along the x axis.

  for (int xOffset = -1; xOffset < 2; xOffset++) {
    graphics_draw_line(ctx, GPoint(p0.x+xOffset, p0.y), GPoint(p1.x+xOffset, p1.y));
  }

}


void drawCirclePlayer(GContext* ctx, GPoint center) {

  graphics_context_set_fill_color(ctx, COLOR_FOREGROUND);
  graphics_fill_circle(ctx, center, 15);
  graphics_context_set_fill_color(ctx, COLOR_BACKGROUND);
  graphics_fill_circle(ctx, center, 13);
}


void drawCrossPlayer(GContext* ctx, GPoint center) {

  graphics_context_set_stroke_color(ctx, COLOR_FOREGROUND);

  graphics_draw_line_wide(ctx, GPoint(center.x-12, center.y-12), GPoint(center.x+12, center.y+12));
  graphics_draw_line_wide(ctx, GPoint(center.x-12, center.y+12), GPoint(center.x+12, center.y-12));

}


GPoint getCellCenter(unsigned int cellOffset) {
  // Returns the centre coordinates of a cell location on the playing board.
  const unsigned short NUM_COLUMNS = 3;
  const unsigned short OFFSET_X = 37;
  const unsigned short OFFSET_Y = 37;
  const unsigned short CELL_WIDTH = 35;
  const unsigned short CELL_HEIGHT = 35;

  return GPoint( ((cellOffset % NUM_COLUMNS) * CELL_WIDTH) + OFFSET_X,
		 ((cellOffset / NUM_COLUMNS) * CELL_HEIGHT) + OFFSET_Y);
}


//
// Board location offsets for the move sequence arrays:
//
//  0 | 1 | 2
//  3 | 4 | 5
//  6 | 7 | 8
//

#define DRAW_WIN_LINE 10
#define DRAW_BLANK 11

// Sequences of moves needed to get the specified game result
// Each array position holds the location offset of the active player's move.
// Currently "Player 1" is "O" and "Player 2" is "X"
// TODO: Add more variations
// TODO: Swap who starts each time (or whatever the rule is).
const unsigned short SEQUENCE_FIRST_PLAYER_WINS[] = {DRAW_BLANK, 0, 8, 2, 1, 6, 3, 4, DRAW_WIN_LINE, DRAW_BLANK};
const unsigned short SEQUENCE_SECOND_PLAYER_WINS[] = {2, 8, 5, 6, 7, 0, 4, 3, DRAW_WIN_LINE, DRAW_BLANK};
const unsigned short SEQUENCE_NEITHER_PLAYER_WINS[] = {4, 2, 7, 1, 0, 8, 5, 3, 6, DRAW_BLANK};

typedef struct {
  const unsigned short *SEQUENCE;
  unsigned short sequenceOffset;
} DemoGameState;


void retrieveCurrentGameState(DemoGameState *gameState) {

  PblTm currentTime;

  get_time(&currentTime);

  // The current location in the move sequence is synced to the current time
  // so that games end when the clock digits change.
  gameState->sequenceOffset = (currentTime.tm_sec + 8) % 10;

  // The game sequence we are displaying depends on which digit (i.e. hours or minutes)
  // will change next.
  if ((currentTime.tm_sec >= 52) || (currentTime.tm_sec <= 1)) {

    // Because sequences run over the start of minutes we need to check both
    // the current minute and the current second to know which is the correct
    // sequence to display.
    if (((currentTime.tm_min == 59) && (currentTime.tm_sec >= 52))
	|| ((currentTime.tm_min == 0) && (currentTime.tm_sec <= 1)) ) {

      // Because it will be a new hour at the end of this time period.
      gameState->SEQUENCE = SEQUENCE_FIRST_PLAYER_WINS;

    } else {

      // Because it will be a new minute at the end of this time period.
      gameState->SEQUENCE = SEQUENCE_SECOND_PLAYER_WINS;

    }

  } else {

    // Neither the minute nor hour digit will change at the end of this time period.
    gameState->SEQUENCE = SEQUENCE_NEITHER_PLAYER_WINS;

  }

}


void drawWinLine(GContext* ctx, GPoint leftmostPoint, GPoint rightmostPoint) {

  // Note: The point names are there for a reason. :)
  // TODO: Make it work either way.

  // Make the ends of the line extend past the edges of the board.

  if (leftmostPoint.x != rightmostPoint.x) {
    // It's not a vertical line so adjust the X axis.
    leftmostPoint.x -= 18;
    rightmostPoint.x += 18;
  }

  if (leftmostPoint.y != rightmostPoint.y) {
    // It's not a horizontal line so adjust the Y axis.
    if (leftmostPoint.y < rightmostPoint.y) {
      leftmostPoint.y -= 18;
      rightmostPoint.y += 18;
    } else {
      leftmostPoint.y += 18;
      rightmostPoint.y -= 18;
    }
  }

  graphics_draw_line_wide(ctx, leftmostPoint, rightmostPoint);
}


void drawGameState(GContext* ctx, const DemoGameState gameState) {

  // Note: Because the layer is blanked before we're called, we also need to
  //       redraw all the player moves preceeding the current move each time
  //       we're called.

  // If we've reached the final part of the sequence don't draw anything.
  if (gameState.SEQUENCE[gameState.sequenceOffset] == DRAW_BLANK) {
    return;
  }

  unsigned short activePlayer = 0;

  for (unsigned short currentOffset = 0; currentOffset <= gameState.sequenceOffset; currentOffset++) {

    switch (gameState.SEQUENCE[currentOffset]) {

      case DRAW_BLANK:
	// Do nothing as a DRAW_BLANK value in a non terminal position
	// is just padding so the sequence has the correct length.
	break;


      case DRAW_WIN_LINE:
	// TODO: Don't hard code this--calculate the winner from the board.
	if (gameState.SEQUENCE == SEQUENCE_SECOND_PLAYER_WINS) {
	  drawWinLine(ctx, getCellCenter(0), getCellCenter(6));
	} else { // Assumes first player wins
	  drawWinLine(ctx, getCellCenter(6), getCellCenter(2));
	}
	break;


      default:
	// The value is a cell location offset.
	// Draw the marker for the current player who is active in this turn.
	(activePlayer ? drawCrossPlayer : drawCirclePlayer)(ctx, getCellCenter(gameState.SEQUENCE[currentOffset]));

	activePlayer = (activePlayer + 1) % 2;
	break;
    }
  }
}


// This is called whenever the `playersLayer` layer needs to be redrawn.
// TODO: Can we get this called without the layer being cleared?
void playersLayer_update_callback(Layer *me, GContext* ctx) {
  (void)me; // Prevents "unused" warnings.

  DemoGameState gameState;


#ifdef INVERT_COLORS
  graphics_context_set_compositing_mode(ctx, GCompOpAssignInverted);
#endif

  retrieveCurrentGameState(&gameState);

  drawGameState(ctx, gameState);

}


void update_time_display() {
  static char timeText[] = "00:00";

  char *timeFormat;

  PblTm currentTime;

  GContext* ctx = app_get_current_graphics_context();
#ifdef INVERT_COLORS
  graphics_context_set_compositing_mode(ctx, GCompOpAssignInverted);
#endif

  if (clock_is_24h_style()) {
    timeFormat = "%R";
  } else {
    timeFormat = "%I:%M";
  }

  get_time(&currentTime);

  string_format_time(timeText, sizeof(timeText), timeFormat, &currentTime);

  // Kludge to handle lack of non-padded hour format string
  // for twelve hour clock.
  if (!clock_is_24h_style() && (timeText[0] == '0')) {
    memmove(timeText, &timeText[1], sizeof(timeText) - 1);
  }

  text_layer_set_text(&timeLayer, timeText);
}


// Handle the start-up of the app
void handle_init_app(AppContextRef app_ctx) {
  (void) app_ctx;

  // Create our app's base window
  window_init(&window, "Tic Tock Toe watch");
  window_stack_push(&window, true);

  window_set_background_color(&window, COLOR_BACKGROUND);

  // Init the layer that shows the board
  layer_init(&boardLayer, window.layer.frame); // Associate with layer object and set dimensions
  boardLayer.update_proc = &boardLayer_update_callback; // Set the drawing callback function for the layer.
  layer_add_child(&window.layer, &boardLayer); // Add the child to the app's base window

  // Init the layer that shows the player marks
  // TODO: Wrap this boilerplate in a function?
  layer_init(&playersLayer, window.layer.frame);
  playersLayer.update_proc = &playersLayer_update_callback;
  layer_add_child(&window.layer, &playersLayer);

  // Init the text layer used to show the time
  // TODO: Determine best practice for text. e.g. TextLayer with standard update proc vs Layer with custom update proc
  // TODO: Wrap this boilerplate in a function?
  text_layer_init(&timeLayer, GRect(0, 168-42, 144 /* width */, 42 /* height */));
  text_layer_set_text_alignment(&timeLayer, GTextAlignmentCenter);
  text_layer_set_text_color(&timeLayer, COLOR_FOREGROUND);
  text_layer_set_background_color(&timeLayer, GColorClear);
  text_layer_set_font(&timeLayer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));

  update_time_display();

  layer_add_child(&window.layer, &timeLayer.layer);
}


// Called once per second
static void handle_second_tick(AppContextRef ctx, PebbleTickEvent *t) {
  (void)ctx;
  (void)t;

  // Causes a redraw of the layer (via the
  // associated layer update callback)
  // Note: This will cause the entire layer to be cleared first so it needs
  //       to be redrawn in its entirety--if you want to preserve drawn
  //       content you must have it on a different layer. e.g. board vs player layers.
  layer_mark_dirty(&playersLayer);

  // TODO: Find out when the redraw actually occurs. Does it make any difference about order of these calls?
  update_time_display();
}


// The main event/run loop for our app
void pbl_main(void *params) {
  PebbleAppHandlers handlers = {

    // Handle app start
    .init_handler = &handle_init_app,

    // Handle time updates
    .tick_info = {
      .tick_handler = &handle_second_tick,
      .tick_units = SECOND_UNIT
    }

  };
  app_event_loop(params, &handlers);
}
