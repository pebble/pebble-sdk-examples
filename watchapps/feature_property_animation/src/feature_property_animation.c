#include "pebble.h"

#include <stdlib.h>

static Window *window;

static TextLayer *text_layer;

static PropertyAnimation *prop_animation;

static int toggle;

static void animation_started(Animation *animation, void *data) {
  text_layer_set_text(text_layer, "Started.");
}

static void animation_stopped(Animation *animation, bool finished, void *data) {
  text_layer_set_text(text_layer, finished ? "Hi, I'm a TextLayer!" : "Just Stopped.");
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

static void click_handler(ClickRecognizerRef recognizer, Window *window) {
  Layer *layer = text_layer_get_layer(text_layer);

  GRect to_rect;
  if (toggle) {
    to_rect = GRect(4, 4, 120, 60);
  } else {
    to_rect = GRect(84, 92, 60, 60);
  }
  toggle = !toggle;

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Toggle is %u, %p", toggle, &toggle);

  destroy_property_animation(&prop_animation);

  prop_animation = property_animation_create_layer_frame(layer, NULL, &to_rect);
  animation_set_duration((Animation*) prop_animation, 400);
  switch (click_recognizer_get_button_id(recognizer)) {
    case BUTTON_ID_UP:
      animation_set_curve((Animation*) prop_animation, AnimationCurveEaseOut);
      break;

    case BUTTON_ID_DOWN:
      animation_set_curve((Animation*) prop_animation, AnimationCurveEaseIn);
      break;

    default:
    case BUTTON_ID_SELECT:
      animation_set_curve((Animation*) prop_animation, AnimationCurveEaseInOut);
      break;
  }

  /*
   // Exmple animation parameters:

   // Duration defaults to 250 ms
   animation_set_duration(&prop_animation->animation, 1000);

   // Curve defaults to ease-in-out
   animation_set_curve(&prop_animation->animation, AnimationCurveEaseOut);

   // Delay defaults to 0 ms
   animation_set_delay(&prop_animation->animation, 1000);
   */

  animation_set_handlers((Animation*) prop_animation, (AnimationHandlers) {
    .started = (AnimationStartedHandler) animation_started,
    .stopped = (AnimationStoppedHandler) animation_stopped,
  }, NULL /* callback data */);
  animation_schedule((Animation*) prop_animation);
}

static void config_provider(Window *window) {
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) click_handler);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, (ClickConfigProvider) config_provider);
  window_stack_push(window, false);

  text_layer = text_layer_create(GRect(0, 0, 60, 60));

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Toggle is %u, %p", toggle, &toggle);

  text_layer_set_text(text_layer, "Started!");
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layer));

  GRect to_rect = GRect(84, 92, 60, 60);

  prop_animation = property_animation_create_layer_frame(text_layer_get_layer(text_layer), NULL, &to_rect);

  animation_schedule((Animation*) prop_animation);
}

static void deinit(void) {
  destroy_property_animation(&prop_animation);

  window_stack_remove(window, false);
  window_destroy(window);
  text_layer_destroy(text_layer);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

