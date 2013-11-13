#include <pebble.h>
#include "score.h"
#include "entry.h"

#define NUM_HIGH_SCORES  8

static struct HighScoreUi {
  Window *window;
  TextLayer *title_text;

  TextLayer *name_text[NUM_HIGH_SCORES];
  TextLayer *score_text[NUM_HIGH_SCORES];
  char score_buf[NUM_HIGH_SCORES][16];
} ui;

typedef struct Score {
  char name[4];   // A null-terminated 3 initials, eg. "AAA\0".
  unsigned score;
} Score;

static short new_place; // if > 0, this is the added place

static Score scores[NUM_HIGH_SCORES];

// Returns the placing this player got in the high score list.
// -1 if they did not place.
static short get_place(unsigned score) {
  int i;
  for (i = 0; i < NUM_HIGH_SCORES; i++)
    if (score >= scores[i].score) return i;

  return -1;
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(ui.window);
  GRect bounds = layer_get_bounds(window_layer);

  ui.title_text = text_layer_create((GRect) {
        .origin = { 0, 0 },
        .size = { bounds.size.w, 64 }
      });

  text_layer_set_text(ui.title_text, "HIGH SCORES");
  text_layer_set_text_alignment(ui.title_text, GTextAlignmentCenter);
  text_layer_set_font(ui.title_text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(ui.title_text));

  unsigned width = bounds.size.w/2 - 10;
  // Buffers for stringification of scores
  for (int i = 0; i < NUM_HIGH_SCORES; ++i) {
    ui.name_text[i] = text_layer_create((GRect) {
        .origin = { 10, i*15 + 25 },
        .size = { width, 15 }
      });
    text_layer_set_text(ui.name_text[i], scores[i].name);
    text_layer_set_text_alignment(ui.name_text[i], GTextAlignmentLeft);
    layer_add_child(window_layer, text_layer_get_layer(ui.name_text[i]));

    ui.score_text[i] = text_layer_create((GRect) {
        .origin = { bounds.size.w - width - 10, i*15 + 25 },
        .size = { width, 15 }
      });
    snprintf(ui.score_buf[i], 16, "%d", scores[i].score);
    text_layer_set_text(ui.score_text[i], ui.score_buf[i]);
    text_layer_set_text_alignment(ui.score_text[i], GTextAlignmentRight);
    layer_add_child(window_layer, text_layer_get_layer(ui.score_text[i]));
  }
}

static void window_unload(Window *window) {
  text_layer_destroy(ui.title_text);

  if (new_place > -1) {
    // Write all data to persistent storage
    for (int i = NUM_HIGH_SCORES; i >= new_place; i--) {
      persist_write_string(i*2, scores[i].name);
      persist_write_int(i*2 + 1, scores[i].score);
    }
  }

  for (int i = 0; i < NUM_HIGH_SCORES; ++i) {
    text_layer_destroy(ui.name_text[i]);
    text_layer_destroy(ui.score_text[i]);
  }
}

// If this score is a high score, pop up the entry window and enter the score.
bool high_score_add_score(unsigned score) {
  short place = get_place(score);
  new_place = place;
  if (place < 0)
    return false;

  // Shift everything below this high score down.
  for (int i = NUM_HIGH_SCORES-1; i > place; i--) {
    scores[i].score = scores[i-1].score;
    memcpy(scores[i].name, scores[i-1].name, 3);
    snprintf(ui.score_buf[i], 16, "%d", scores[i].score);
  }

  scores[place].score = score;

  entry_get_name(scores[place].name);

  snprintf(ui.score_buf[place], 16, "%d", scores[place].score);
  layer_mark_dirty(text_layer_get_layer(ui.name_text[place]));
  layer_mark_dirty(text_layer_get_layer(ui.score_text[place]));

  return true;
}

void high_score_show(void) {
  const bool animated = true;
  window_stack_push(ui.window, animated);
}

void high_score_init(void) {
  ui.window = window_create();
  window_set_window_handlers(ui.window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  // Read in stored high scores, or put in default values if non-existent.
  for (int i = 0; i < NUM_HIGH_SCORES; ++i) {
    if (persist_exists(2*i))
      persist_read_string(2*i, scores[i].name, 4);
    else
      memcpy(scores[i].name, "AAA\0", 4);

    if (persist_exists(2*i + 1))
      scores[i].score = persist_read_int(2*i + 1);
    else
      scores[i].score = 2*(NUM_HIGH_SCORES - i);
  }
}

void high_score_deinit(void) {
  window_destroy(ui.window);
}
