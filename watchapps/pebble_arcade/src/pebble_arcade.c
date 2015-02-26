#include <pebble.h>
#include "game.h"
#include "score.h"
#include "entry.h"

static void init() {
  game_init();
  high_score_init();
  entry_init();
}

static void deinit() {
  game_deinit();
  high_score_deinit();
  entry_deinit();
}

int main() {
  init();
  app_event_loop();
  deinit();
}
