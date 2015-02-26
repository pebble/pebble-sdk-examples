#include <pebble.h>
#include "score.h"
#include "entry.h"

#define NUM_HIGH_SCORES  8

typedef struct Score {
  char name[4];   // A null-terminated 3 initials, eg. "AAA\0".
  unsigned score;
} Score;

static Window *s_main_window;
static TextLayer *s_name_text[NUM_HIGH_SCORES];
static TextLayer *s_score_text[NUM_HIGH_SCORES];
static TextLayer *s_title_text;

static Score s_score_array[NUM_HIGH_SCORES];
static char s_score_buf[NUM_HIGH_SCORES][16];
static short s_new_place; // if > 0, this is the added place

// Returns the placing this player got in the high score list.
// -1 if they did not place.
static short get_place(unsigned score) {
  for (int i = 0; i < NUM_HIGH_SCORES; i++)
    if (score >= s_score_array[i].score)  {
      return i;
    }

  return -1;
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_title_text = text_layer_create(GRect(0, 0, bounds.size.w, 64));
  text_layer_set_text(s_title_text, "HIGH SCORES");
  text_layer_set_text_alignment(s_title_text, GTextAlignmentCenter);
  text_layer_set_font(s_title_text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(s_title_text));

  unsigned width = bounds.size.w / 2 - 10;
  
  // Buffers for stringification of s_score_array
  for (int i = 0; i < NUM_HIGH_SCORES; ++i) {
    s_name_text[i] = text_layer_create(GRect(10, i * 15 + 25, width, 15));
    text_layer_set_text(s_name_text[i], s_score_array[i].name);
    text_layer_set_text_alignment(s_name_text[i], GTextAlignmentLeft);
    layer_add_child(window_layer, text_layer_get_layer(s_name_text[i]));

    s_score_text[i] = text_layer_create(GRect(bounds.size.w - width - 10, i * 15 + 25, width, 15));
    snprintf(s_score_buf[i], sizeof(s_score_buf[i]), "%d", s_score_array[i].score);
    text_layer_set_text(s_score_text[i], s_score_buf[i]);
    text_layer_set_text_alignment(s_score_text[i], GTextAlignmentRight);
    layer_add_child(window_layer, text_layer_get_layer(s_score_text[i]));
  }
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_title_text);

  if (s_new_place > -1) {
    // Write all data to persistent storage
    for (int i = NUM_HIGH_SCORES; i >= s_new_place; i--) {
      persist_write_string(i * 2, s_score_array[i].name);
      persist_write_int(i * 2 + 1, s_score_array[i].score);
    }
  }

  for (int i = 0; i < NUM_HIGH_SCORES; ++i) {
    text_layer_destroy(s_name_text[i]);
    text_layer_destroy(s_score_text[i]);
  }
}

// If this score is a high score, pop up the entry window and enter the score.
bool high_score_add_score(unsigned score) {
  short place = get_place(score);
  s_new_place = place;
  if (place < 0) {
    return false;
  }

  // Shift everything below this high score down.
  for (int i = NUM_HIGH_SCORES-1; i > place; i--) {
    s_score_array[i].score = s_score_array[i-1].score;
    memcpy(s_score_array[i].name, s_score_array[i-1].name, 3);
    snprintf(s_score_buf[i], sizeof(s_score_buf[i]), "%d", s_score_array[i].score);
  }

  s_score_array[place].score = score;

  entry_get_name(s_score_array[place].name);

  snprintf(s_score_buf[place], sizeof(s_score_buf[place]), "%d", s_score_array[place].score);
  layer_mark_dirty(text_layer_get_layer(s_name_text[place]));
  layer_mark_dirty(text_layer_get_layer(s_score_text[place]));

  return true;
}

void high_score_show() {
  window_stack_push(s_main_window, true);
}

void high_score_init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });

  // Read in stored high score, or put in default values if non-existent.
  for (int i = 0; i < NUM_HIGH_SCORES; ++i) {
    if (persist_exists(2 * i)) {
      persist_read_string(2 * i, s_score_array[i].name, 4);
    } else {
      memcpy(s_score_array[i].name, "AAA\0", 4);
    }

    if (persist_exists(2 * i + 1)) {
      s_score_array[i].score = persist_read_int(2 * i + 1);
    } else {
      s_score_array[i].score = 2 * (NUM_HIGH_SCORES - i);
    }
  }
}

void high_score_deinit() {
  window_destroy(s_main_window);
}
