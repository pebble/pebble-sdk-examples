#include <pebble.h>

static Window *s_main_window;
static GSize s_window_size;

/*
 * Gray values for the animated gradient.
 */
static int s_gray_top;
static int s_gray_bottom;

/*
 * Helper value for draw_method_feedback().
 */
static int s_time_feedback_y;

/*
 * Bayer matrix for ordered dithering.
 *
 * Have a look at http://en.wikipedia.org/wiki/Ordered_dithering
 */
static const uint8_t ditherMatrix[8][8] = {
  {  0, 128,  32, 160,   8, 136,  40, 168},
  {192,  64, 224,  96, 200,  72, 232, 104},
  { 48, 176,  16, 144,  56, 184,  24, 152},
  {240, 112, 208,  80, 248, 120, 216,  88},
  { 12, 140,  44, 172,   4, 132,  36, 164},
  {204,  76, 236, 108, 196,  68, 228, 100},
  { 60, 188,  28, 156,  52, 180,  20, 148},
  {252, 124, 220,  92, 244, 116, 212,  84}
};

/*
 * Converts a gray value to CGColor by using ordered dithering.
 *
 * As ordered dithering uses a stable pattern relative to the output image
 * you need to specific the target coordinate of the resulting pixel.
 *
 */
static GColor color_for_gray(int16_t x, int16_t y, uint8_t gray) {
  // dithering performs a quantizing step to map many colors to a reduced set of colors.
  // it does so by comparing the actual color (here: gray) against a threshold to ultimately
  // find the closet matching color of the given palette (here: black or white)
  // please read http://en.wikipedia.org/wiki/Ordered_dithering for details

  // here's a simple implementation that does this with an ordered dithering matrix and
  // only two possible output colors

  // find the threshold value for a given pixel coordinate by "looping through" the dither matrix
  const uint8_t threshold_value = ditherMatrix[y % 8][x % 8];

  // with only two colors (black and white) finding the closest matching color is trivial
  return gray > threshold_value ? GColorWhite : GColorBlack;
}

/*
 * Calculates GColor values for 8 adjacent pixels at once and returns them as a single byte.
 */
#ifndef PBL_COLOR
static uint8_t dither_pattern_8(int16_t y, uint8_t gray) {
  return color_for_gray(0, y, gray) << 7 |
         color_for_gray(1, y, gray) << 6 |
         color_for_gray(2, y, gray) << 5 |
         color_for_gray(3, y, gray) << 4 |
         color_for_gray(4, y, gray) << 3 |
         color_for_gray(5, y, gray) << 2 |
         color_for_gray(6, y, gray) << 1 |
         color_for_gray(7, y, gray) << 0;
}
#endif

/*
 * Draws textual feedback about the currently used update procedure.
 *
 * Y-coordinate is determined by s_time_feedback_y which is updated by update_animation().
 *
 */
static void draw_method_feedback(GContext *ctx, char *method_text) {
  GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  graphics_context_set_fill_color(ctx, GColorBlack);

  struct GRect text_bounds = {.size = s_window_size};
  text_bounds.size.h = (int16_t)(graphics_text_layout_get_content_size(method_text, font, text_bounds,
      GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter).h + 5); // space for descender
  text_bounds.origin.y = (int16_t)(s_time_feedback_y % (s_window_size.h - text_bounds.size.h));

  graphics_fill_rect(ctx, text_bounds, 0, GCornerNone);
  graphics_draw_text(ctx, method_text, font, text_bounds,
      GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
}

/*
 * Returns interpolated gray value between s_gray_top and s_gray_bottom for a given row.
 */
uint8_t gray_for_row(int16_t y) {
  const int16_t h = s_window_size.h;

  // linear interpolation without floats, equivalent to
  //   float f = (float)y / (float)h;
  //   return = (uint8_t)(s_gray_top * (1-f) + f * s_gray_bottom);
  return (uint8_t)((s_gray_top * (h - y) + y * s_gray_bottom) / h);
}

/*
 * Draws linear gradient using graphics_draw_pixel() + draws info about used method.
 */
static void update_proc_draw_pixel(Layer *layer, GContext *ctx) {
  // determines gray value per row,
  // calculates dithered pixel color per pixel, and
  // uses graphics_draw_pixel for each pixel to draw the actual color

  for (int16_t y = 0; y < s_window_size.h; y++) {
    const uint8_t row_gray = gray_for_row(y);
    for (int16_t x = 0; x < s_window_size.w; x++) {
      graphics_context_set_stroke_color(ctx, color_for_gray(x, y, row_gray));
      graphics_draw_pixel(ctx, GPoint(x, y));
    }
  }
  draw_method_feedback(ctx, "draw_pixel");
}

/*
 * Draws linear gradient using direct frame buffer access.
 *
 * This function isn't being used directly by any layer. Instead, update_proc_frame_buffer_1()
 * and update_proc_frame_buffer_20() will call it before they draw additional information.
 */
static void update_proc_frame_buffer(Layer *layer, GContext *ctx) {
  // obtains direct access to the frame buffer,
  // determines gray_ values per row
  // calculates dithering pattern for the row, and
  // sets all pixels of the row at once

  // obtain frame buffer (must be released again using graphics_release_frame_buffer!)
  GBitmap *fb = graphics_capture_frame_buffer(ctx);

  // during the loop, row always points to the byte of the first 8 pixels of the current row
#ifdef PBL_PLATFORM_BASALT
  GRect fb_bounds = gbitmap_get_bounds(fb);
  uint8_t *row = gbitmap_get_data(fb);
  uint16_t row_bytes = gbitmap_get_bytes_per_row(fb);
#else
  GRect fb_bounds = fb->bounds;
  uint8_t *row = fb->addr;
  uint16_t row_bytes = fb->row_size_bytes;
#endif
  row += fb_bounds.origin.y * row_bytes;

  // For all rows
  for (int16_t y = fb_bounds.origin.y; y < fb_bounds.size.h; y++) {
  const uint8_t row_gray = gray_for_row(y);
#ifdef PBL_PLATFORM_BASALT
    // Each pixel is one whole byte
    for(int x = fb_bounds.origin.x; x < fb_bounds.size.w; x++) {
      memset(&row[(y * fb_bounds.size.w) + x], color_for_gray(x, y, row_gray).argb, 1);
    }
#else
    // as our dither pattern repeats itself after 8 pixels (8x8 dither matrix), we can store the
    // whole pattern of this row in a single byte and set all pixels with a single call to memset
    uint8_t row_gray_dither_pattern = dither_pattern_8(y, row_gray);
    memset(row, row_gray_dither_pattern, row_bytes);
    row += row_bytes;
#endif
  }

  // do not forget to release the frame buffer after usage. Other graphics_* functions are blocked
  // while it is capture.
  graphics_release_frame_buffer(ctx, fb);
}

/*
 * Draws linear gradient once using direct frame buffer access + draws info about used method.
 */
static void update_proc_frame_buffer_1(Layer *layer, GContext *ctx) {
  update_proc_frame_buffer(layer, ctx);
  draw_method_feedback(ctx, "frame_buffer");
}

/*
 * Draws linear gradient 20 times using direct frame buffer access + draws info about used method.
 */
static void update_proc_frame_buffer_20(Layer *layer, GContext *ctx) {
  for (int i = 0; i < 20; i++) {
    update_proc_frame_buffer(layer, ctx);
  }
  draw_method_feedback(ctx, "20 x frame_buffer");
}

/*
 * Updates global helper variables and marks root layer dirty for the next animation step.
 *
 * This function calls app_timer_register() for itself to enter an infinite update loop.
 */
static void update_animation(void *data) {
  // input_top/_bottom are angles and change with each update so s_gray_top/_bottom will update
  // smoothly thanks to sin_lookup
  // The magic numbers you see had been determined via trial and error
  static int input_top = TRIG_MAX_ANGLE * 1 / 4;
  static int input_bottom = TRIG_MAX_ANGLE * 3 / 4;
  input_top += TRIG_MAX_ANGLE * 7 / 360;
  input_bottom += TRIG_MAX_ANGLE * 3 / 360;

  // gray values used for the actual drawing
  s_gray_top = (127 + sin_lookup(input_top) * 127 / TRIG_MAX_RATIO);
  s_gray_bottom = (127 + sin_lookup(input_bottom) * 127 / TRIG_MAX_RATIO);

  // y-coordinate for draw_method_feedback to give additional visual feedback about the speed
  s_time_feedback_y += 5;

  // trigger the actual redraw
  layer_mark_dirty(window_get_root_layer(s_main_window));

  // enter the infinite update loop
  app_timer_register(1000 / 30, update_animation, NULL);
}

static LayerUpdateProc update_procs[] = {
    update_proc_frame_buffer_1,
    update_proc_draw_pixel,
    update_proc_frame_buffer_20,
};

static int s_current_update_proc = 0;

static void toggle_update_proc_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_current_update_proc = (s_current_update_proc + 1) % ARRAY_LENGTH(update_procs);
  layer_set_update_proc(window_get_root_layer(s_main_window), update_procs[s_current_update_proc]);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, toggle_update_proc_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, toggle_update_proc_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, toggle_update_proc_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  layer_set_update_proc(window_layer, update_procs[0]);
  s_window_size = layer_get_bounds(window_layer).size;

  // start update loop
  update_animation(NULL);
}

static void init(void) {
  s_main_window = window_create();
  window_set_click_config_provider(s_main_window, click_config_provider);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = window_load,
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
