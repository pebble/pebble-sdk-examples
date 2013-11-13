#pragma once

void high_score_init(void);
void high_score_deinit(void);

// Add the given score if it is a high score.
// This will launch up an entry window to request the player's intiials.
// Returns true if this is a high score, false otherwise.
bool high_score_add_score(unsigned score);

void high_score_show(void);
