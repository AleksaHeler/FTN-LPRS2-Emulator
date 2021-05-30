#ifndef ENEMY_H
#define ENEMY_H

#include "game_data.h"
#include "raycast_renderer.h"
#include "player.h"

// Update enemy -> move, AI logic
void enemy_update(player_t*);

#endif // ENEMY_H