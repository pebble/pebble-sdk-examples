#include <pebble.h>
#include "game.h"
#include "score.h"

// Time always in ms
#define MAX_TIME      5000
#define TIME_INTERVAL 10

static struct GameUi {
  Window *window;
  TextLayer *score_text;
  TextLayer *time_text;
} ui;

static struct GameState {
  unsigned score;
  unsigned time; // Elapsed time in ms
  uint16_t prev_ms;
  AppTimer *timer;
} state;

static void finish(void) {
  high_score_show();
  high_score_add_score(state.score);
}

static void update_time_text(unsigned seconds, unsigned fraction) {
  static char buf[16];
  snprintf(buf, 16, "%u.%02u", seconds, fraction);
  text_layer_set_text(ui.time_text, buf);
}

static void timer_callback(void *data) {
  uint16_t c_ms = time_ms(NULL, NULL);
  uint16_t elapsed;
  if (c_ms < state.prev_ms)
    elapsed = (c_ms+1000) - state.prev_ms;
  else
    elapsed = c_ms - state.prev_ms;
  state.prev_ms = c_ms;

  state.time += elapsed;
  unsigned seconds_remaining = (MAX_TIME - state.time) / 1000;
  unsigned fraction_remaining = ((MAX_TIME - state.time) % 1000) / 10;

  update_time_text(seconds_remaining, fraction_remaining);
  if (state.time < MAX_TIME)
    state.timer = app_timer_register(TIME_INTERVAL - (elapsed % TIME_INTERVAL),
                                     timer_callback, NULL);
  else
    finish();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (!state.timer) {
    state.prev_ms = time_ms(NULL, NULL);
    state.timer = app_timer_register(TIME_INTERVAL, timer_callback, NULL);
  }

  static char buf[32];
  ++state.score;
  snprintf(buf, 32, "Score: %u", state.score);
  text_layer_set_text(ui.score_text, buf);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(ui.window);
  GRect bounds = layer_get_bounds(window_layer);

  ui.score_text = text_layer_create((GRect) {
        .origin = { 0, 72 },
        .size = { bounds.size.w, 20 }
      });
  text_layer_set_text_alignment(ui.score_text, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(ui.score_text));

  ui.time_text = text_layer_create((GRect) {
        .origin = { 0, 0 },
        .size = { bounds.size.w, 64 }
      });
  text_layer_set_text_alignment(ui.time_text, GTextAlignmentCenter);
  text_layer_set_font(ui.time_text,
                      fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(ui.time_text));
}

static void window_appear(Window *window) {
  // When the game window appears, reset the game
  state.score = 0;
  state.time = 0;
  state.timer = NULL;

  update_time_text(MAX_TIME/1000, (MAX_TIME % 1000) / 10);
  text_layer_set_text(ui.score_text, "Press Select to Start");
}

static void window_unload(Window *window) {
  text_layer_destroy(ui.time_text);
  text_layer_destroy(ui.score_text);
}

void game_init(void) {
  ui.window = window_create();

  window_set_click_config_provider(ui.window, click_config_provider);
  window_set_window_handlers(ui.window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
    .appear = window_appear
  });
  const bool animated = true;
  window_stack_push(ui.window, animated);
}

void game_deinit(void) {
  window_destroy(ui.window);
}
