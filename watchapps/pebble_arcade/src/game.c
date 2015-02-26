#include <pebble.h>
#include "game.h"
#include "score.h"

// Time always in ms
#define MAX_TIME      5000
#define TIME_INTERVAL 10

static Window *s_main_window;
static TextLayer *s_score_text, *s_time_text;
static AppTimer *s_timer;

static unsigned s_score, s_time;
static uint16_t s_prev_ms;

static void finish() {
  high_score_show();
  high_score_add_score(s_score);
}

static void update_time_text(unsigned seconds, unsigned fraction) {
  static char s_buf[16];
  snprintf(s_buf, sizeof(s_buf), "%u.%02u", seconds, fraction);
  text_layer_set_text(s_time_text, s_buf);
}

static void timer_callback(void *data) {
  uint16_t c_ms = time_ms(NULL, NULL);
  uint16_t elapsed;
  if (c_ms < s_prev_ms) {
    elapsed = (c_ms + 1000) - s_prev_ms;
  } else {
    elapsed = c_ms - s_prev_ms;
  }
  s_prev_ms = c_ms;

  s_time += elapsed;
  unsigned seconds_remaining = (MAX_TIME - s_time) / 1000;
  unsigned fraction_remaining = ((MAX_TIME - s_time) % 1000) / 10;

  update_time_text(seconds_remaining, fraction_remaining);
  if (s_time < MAX_TIME) {
    s_timer = app_timer_register(TIME_INTERVAL - (elapsed % TIME_INTERVAL), timer_callback, NULL);
  } else {
    finish();
  }
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (!s_timer) {
    s_prev_ms = time_ms(NULL, NULL);
    s_timer = app_timer_register(TIME_INTERVAL, timer_callback, NULL);
  }

  static char s_buf[32];
  ++s_score;
  snprintf(s_buf, sizeof(s_buf), "Score: %u", s_score);
  text_layer_set_text(s_score_text, s_buf);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_score_text = text_layer_create(GRect(0, 72, bounds.size.w, 20));
  text_layer_set_text_alignment(s_score_text, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_score_text));

  s_time_text = text_layer_create(GRect(0, 0, bounds.size.w, 64));
  text_layer_set_text_alignment(s_time_text, GTextAlignmentCenter);
  text_layer_set_font(s_time_text, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(s_time_text));
}

static void main_window_appear(Window *window) {
  // When the game window appears, reset the game
  s_score = 0;
  s_time = 0;
  s_timer = NULL;

  update_time_text(MAX_TIME / 1000, (MAX_TIME % 1000) / 10);
  text_layer_set_text(s_score_text, "Press Select to Start");
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_time_text);
  text_layer_destroy(s_score_text);
}

void game_init() {
  s_main_window = window_create();
  window_set_click_config_provider(s_main_window, click_config_provider);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
    .appear = main_window_appear
  });
  window_stack_push(s_main_window, true);
}

void game_deinit() {
  window_destroy(s_main_window);
}
