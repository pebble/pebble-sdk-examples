/* Inspired by peapod's progress bar layer: https://github.com/Kathatrine/peapod */
#include <pebble.h>

typedef Layer ProgressBarLayer;

#define SPEED_MAX 5000
#define SPEED_MIN 50
#define DEFAULT_SPEED 750

static Window *window;
static TextLayer *done_text;
static TextLayer *instruction_text;
static AppTimer *progress_timer;
static ProgressBarLayer *progress_bar;
static unsigned int s_current_speed = DEFAULT_SPEED;

typedef struct {
  unsigned int progress; // how full the progress bar is
} ProgressData;

static void progress_timer_callback(void *data);

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  ProgressData *data = layer_get_data(progress_bar);
  data->progress = 0;
  s_current_speed = DEFAULT_SPEED;
  if (progress_timer) {
    app_timer_cancel(progress_timer);
  }
  progress_timer = app_timer_register(s_current_speed /* milliseconds */, progress_timer_callback, NULL);
  text_layer_set_text(done_text, "");
  layer_mark_dirty(progress_bar);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_current_speed > SPEED_MIN) {
    s_current_speed -= 50;
  }
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_current_speed < SPEED_MAX) {
    s_current_speed += 50;
  }
}

static void config_provider(Window *window) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void progress_timer_callback(void *data) {
  ProgressData *d = layer_get_data(progress_bar);
  if (d->progress > 129) {
    progress_timer = NULL;
    text_layer_set_text(done_text, "Done!");
  } else {
    progress_timer = app_timer_register(s_current_speed /* milliseconds */, progress_timer_callback, NULL);
    layer_mark_dirty(progress_bar);
  }
}

static void progress_bar_layer_update(ProgressBarLayer *bar, GContext *ctx) {
  ProgressData *data = (ProgressData *)layer_get_data(bar);

  // Outline the progress bar
  graphics_context_set_stroke_color(ctx, GColorBlack);
  GRect bounds = layer_get_bounds(bar);
  graphics_draw_round_rect(ctx, GRect(0, 0, bounds.size.w, bounds.size.h), 4);

  // Fill the progress bar
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(0, 0, data->progress, bounds.size.h), 4, GCornersAll);
  ++data->progress;
}

static ProgressBarLayer * progress_bar_layer_create(void) {
  ProgressBarLayer *progress_bar_layer = layer_create_with_data(GRect(6, 120, 130, 8), sizeof(ProgressData));
  layer_set_update_proc(progress_bar_layer, progress_bar_layer_update);
  layer_mark_dirty(progress_bar_layer);

  return progress_bar_layer;
}

static void progress_bar_destroy(ProgressBarLayer *progress_bar_layer) {
  layer_destroy(progress_bar_layer);
}

void init(void) {
  window = window_create();
  window_set_click_config_provider(window, (ClickConfigProvider) config_provider);
  window_stack_push(window, true /* Animated */);

  instruction_text = text_layer_create(GRect(0, 0, 144, 73));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(instruction_text));
  text_layer_set_text(instruction_text, "[UP] increase speed\n[SEL] reset\n[DN] decrease speed");

  progress_bar = progress_bar_layer_create();
  layer_add_child(window_get_root_layer(window), progress_bar);

  done_text = text_layer_create(GRect(50, 74, 94, 20));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(done_text));

  // Start the progress timer
  progress_timer = app_timer_register(s_current_speed /* milliseconds */, progress_timer_callback, NULL);
}

void deinit(void) {
  text_layer_destroy(done_text);
  progress_bar_destroy(progress_bar);
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
