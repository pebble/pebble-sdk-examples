#include <pebble.h>



static const uint32_t ANIMAL_LOG_TAGS[3] = { 0x5, 0xb, 0xd }; // sealion, pelican, dolphin

static const int RESOURCE_IDS[3] = {
  RESOURCE_ID_IMAGE_ACTION_ICON_SEALION,
  RESOURCE_ID_IMAGE_ACTION_ICON_PELICAN,
  RESOURCE_ID_IMAGE_ACTION_ICON_DOLPHIN
};

static const GRect NUMBER_POSITIONS[3] =  {
  {{/* x: */ 100, /* y: */ 12 }, {/* width: */ 28, /* height: */ 28}},
  {{/* x: */ 100, /* y: */ 60 }, {/* width: */ 28, /* height: */ 28}},
  {{/* x: */ 100, /* y: */ 107 }, {/* width: */ 28, /* height: */ 28}}
};

typedef struct {
  uint32_t tag;
  TextLayer *text_layer;
  char text[20];
  DataLoggingSessionRef logging_session;
  int count;
  GBitmap *bitmap;
} AnimalData;

static Window *window;
static ActionBarLayer *action_bar_layer;
static AnimalData s_animal_datas[3]; // 0 = sealion, 1 = dolphin, 2 = pelican
static TextLayer *intro_layer;


static void count_animal(AnimalData *animal_data) {
  animal_data->count++;
  time_t now = time(NULL);
  data_logging_log(animal_data->logging_session, (uint8_t *)&now, 1);
  snprintf(animal_data->text, 20, "%d", animal_data->count);
  text_layer_set_text(animal_data->text_layer, animal_data->text);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  // Ma, it's a dolphin!
  AnimalData *animal_data = &s_animal_datas[0];
  count_animal(animal_data);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  // A wonderful bird is the pelican
  // His bill will hold more than his belican
  // He can take in his beak
  // Food enough for a week,
  // But I'm damned if I see how the helican
  AnimalData *animal_data = &s_animal_datas[1];
  count_animal(animal_data);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  // Oh, look daddy! A sea lion!
  AnimalData *animal_data = &s_animal_datas[2];
  count_animal(animal_data);
}

static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  for (int i = 0; i < 3; i++) {
    AnimalData *animal_data = &s_animal_datas[i];
    data_logging_finish(animal_data->logging_session);
    animal_data->count = 0;
    snprintf(animal_data->text, 20, "%d", animal_data->count);
    text_layer_set_text(animal_data->text_layer, animal_data->text);
    animal_data->logging_session =
        data_logging_create(ANIMAL_LOG_TAGS[i], DATA_LOGGING_UINT, 4, false);
  }
}

static void config_provider(void *ctx) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 0, select_long_click_handler, NULL);
}

static void init_animal_datas(Window *window) {
  for (int i = 0; i < 3; i++) {
    AnimalData *animal_data = &s_animal_datas[i];
    animal_data->text_layer = text_layer_create(NUMBER_POSITIONS[i]);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(animal_data->text_layer));
    animal_data->logging_session =
        data_logging_create(ANIMAL_LOG_TAGS[i], DATA_LOGGING_UINT, 4, false);
    animal_data->bitmap = gbitmap_create_with_resource(RESOURCE_IDS[i]);
    snprintf(animal_data->text, 20, "%d", animal_data->count);
    text_layer_set_text(animal_data->text_layer, animal_data->text);
    text_layer_set_font(animal_data->text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  }
}

static void action_bar_init(Window *window) {
  action_bar_layer = action_bar_layer_create();
  action_bar_layer_add_to_window(action_bar_layer, window);
  action_bar_layer_set_click_config_provider(action_bar_layer, config_provider);
  action_bar_layer_set_icon(action_bar_layer, BUTTON_ID_UP, s_animal_datas[0].bitmap);
  action_bar_layer_set_icon(action_bar_layer, BUTTON_ID_SELECT, s_animal_datas[1].bitmap);
  action_bar_layer_set_icon(action_bar_layer, BUTTON_ID_DOWN, s_animal_datas[2].bitmap);
}

static void deinit_animal_datas(void) {
  for (int i = 0; i < 3; i++) {
    AnimalData *animal_data = &s_animal_datas[i];
    data_logging_finish(animal_data->logging_session);
    text_layer_destroy(animal_data->text_layer);
    gbitmap_destroy(animal_data->bitmap);
  }
}

void on_window_load(Window *window) {
  init_animal_datas(window);
  action_bar_init(window);
  intro_layer = text_layer_create(GRect(7, 50, 90, 93));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(intro_layer));
  text_layer_set_text(intro_layer, "track animals\n???\nprofit");
  text_layer_set_text_alignment(intro_layer, GTextAlignmentCenter);
}

void on_window_unload(Window *window) {
  deinit_animal_datas();
  action_bar_layer_destroy(action_bar_layer);
  text_layer_destroy(intro_layer);
}

int main(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = on_window_load,
    .unload = on_window_unload,
  });
  window_stack_push(window, true /* Animated */);
  app_event_loop();
  window_destroy(window);
}
