#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID {0x8B, 0x29, 0x1D, 0x47, 0xE4, 0x4F, 0x44, 0xC4, 0xA8, 0x61, 0xCD, 0xCF, 0xB6, 0x07, 0x7C, 0x8A}
PBL_APP_INFO(MY_UUID,
	     "Drop Zone", "Pebble Technology",
	     0x02, 0x00 /* App major/minor version */,
	     RESOURCE_ID_IMAGE_MENU_ICON,
	     APP_INFO_WATCH_FACE);

Window window;

TextLayer text_time_layer;


// There are two of these because we have to schedule the next animation
// while the other is still in motion because otherwise we cancel the
// in-progress animation.
// (Actually, with the current implementation this is probably no longer
// necessary.)
PropertyAnimation up_animation;
PropertyAnimation down_animation;


// This works around the inability to use the current GRect macro
// for constants.
#define ConstantGRect(x, y, w, h) {{(x), (y)}, {(w), (h)}}

GRect UPPER_TO_RECT = ConstantGRect(0, 5, 144, 168-5);
GRect LOWER_TO_RECT = ConstantGRect(0, 112, 144, 168-112);


#define ANIMATION_DURATION_IN_MS 1500

void init_animations() {

  property_animation_init_layer_frame(&up_animation, &text_time_layer.layer, &LOWER_TO_RECT, &UPPER_TO_RECT);

  animation_set_duration(&up_animation.animation, ANIMATION_DURATION_IN_MS);

  animation_set_curve(&up_animation.animation, AnimationCurveEaseOut);


  property_animation_init_layer_frame(&down_animation, &text_time_layer.layer, &UPPER_TO_RECT, &LOWER_TO_RECT);

  animation_set_duration(&down_animation.animation, ANIMATION_DURATION_IN_MS);

  animation_set_curve(&down_animation.animation, AnimationCurveEaseOut);

}


void schedule_animation(PblTm *tick_time) {

  static bool use_upper_to_rect = false;

  int32_t animation_delay_required_in_ms = 60000 - (tick_time->tm_sec * 1000) - (ANIMATION_DURATION_IN_MS - 100);

  if (animation_delay_required_in_ms < 0) {
    // In the hopefully rare case that we don't have time to schedule the animation
    // before the value is due to change we just let it change in place.
    return;
  }

  if (use_upper_to_rect) {
    animation_set_delay(&up_animation.animation, animation_delay_required_in_ms);
    animation_schedule(&up_animation.animation);
  } else {
    animation_set_delay(&down_animation.animation, animation_delay_required_in_ms);
    animation_schedule(&down_animation.animation);
  }

  use_upper_to_rect = !use_upper_to_rect;

}


void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Drop Zone");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);

  resource_init_current_app(&APP_RESOURCES);

  text_layer_init(&text_time_layer, UPPER_TO_RECT);
  text_layer_set_text_color(&text_time_layer, GColorWhite);
  text_layer_set_background_color(&text_time_layer, GColorClear);
  text_layer_set_text_alignment(&text_time_layer, GTextAlignmentCenter);
  text_layer_set_font(&text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_CONDENSED_SUBSET_40)));
  layer_add_child(&window.layer, &text_time_layer.layer);

  init_animations();

  // TODO: Update display here to avoid blank display on launch?

}


void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {

  (void)ctx;

  // Need to be static because it's used by the system later.
  static char time_text[] = "00:00";

  string_format_time(time_text, sizeof(time_text), "%R", t->tick_time);
  text_layer_set_text(&text_time_layer, time_text);

  schedule_animation(t->tick_time);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,

    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }

  };
  app_event_loop(params, &handlers);
}
