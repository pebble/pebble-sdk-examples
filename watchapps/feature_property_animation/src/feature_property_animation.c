#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_text_layer;
static PropertyAnimation *s_prop_animation;

static int s_toggle;

static void animation_started(Animation *animation, void *data) {
  text_layer_set_text(s_text_layer, "Started.");
}

static void animation_stopped(Animation *animation, bool finished, void *data) {
  text_layer_set_text(s_text_layer, finished ? "Hi, I'm a TextLayer!" : "Just Stopped.");
}

static void destroy_property_animation(PropertyAnimation **prop_animation) {
  if (*prop_animation == NULL) {
    return;
  }

  if (animation_is_scheduled((Animation*) *prop_animation)) {
    animation_unschedule((Animation*) *prop_animation);
  }

  property_animation_destroy(*prop_animation);
  *prop_animation = NULL;
}

static void click_handler(ClickRecognizerRef recognizer, void *context) {
  Layer *layer = text_layer_get_layer(s_text_layer);

  GRect to_rect = (s_toggle) ? GRect(4, 4, 120, 60) : GRect(84, 92, 60, 60);

  s_toggle = !s_toggle;

  destroy_property_animation(&s_prop_animation);

  s_prop_animation = property_animation_create_layer_frame(layer, NULL, &to_rect);
  animation_set_duration((Animation*) s_prop_animation, 400);
  switch (click_recognizer_get_button_id(recognizer)) {
    case BUTTON_ID_UP:
      animation_set_curve((Animation*) s_prop_animation, AnimationCurveEaseOut);
      break;

    case BUTTON_ID_DOWN:
      animation_set_curve((Animation*) s_prop_animation, AnimationCurveEaseIn);
      break;

    default:
    case BUTTON_ID_SELECT:
      animation_set_curve((Animation*) s_prop_animation, AnimationCurveEaseInOut);
      break;
  }

  /*
   // Example animation parameters:

   // Duration defaults to 250 ms
   animation_set_duration(&s_prop_animation->animation, 1000);

   // Curve defaults to ease-in-out
   animation_set_curve(&s_prop_animation->animation, AnimationCurveEaseOut);

   // Delay defaults to 0 ms
   animation_set_delay(&s_prop_animation->animation, 1000);
   */

  animation_set_handlers((Animation*) s_prop_animation, (AnimationHandlers) {
    .started = (AnimationStartedHandler) animation_started,
    .stopped = (AnimationStoppedHandler) animation_stopped,
  }, NULL /* callback data */);
  animation_schedule((Animation*) s_prop_animation);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, click_handler);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(s_main_window);
  GRect bounds = layer_get_frame(window_layer);

  s_text_layer = text_layer_create(GRect(0, 0, 60, 60));
  text_layer_set_text(s_text_layer, "Started!");
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));

  GRect to_rect = GRect(84, 92, 60, 60);

  s_prop_animation = property_animation_create_layer_frame(text_layer_get_layer(s_text_layer), NULL, &to_rect);

  animation_schedule((Animation*) s_prop_animation);
}

static void main_window_unload(Window *window) {
  destroy_property_animation(&s_prop_animation);
  text_layer_destroy(s_text_layer);
}

static void init() {
  s_main_window = window_create();
  window_set_click_config_provider(s_main_window, click_config_provider);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
}

static void deinit(void) {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
