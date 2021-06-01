#ifndef ENEMY_H
#define ENEMY_H

#include "game_data.h"
#include "raycast_renderer.h"
#include "player.h"

// Update enemy -> move, AI logic
void enemy_update(player_t*);
void enemy_take_damage(enemy_t*, uint8_t);

#endif // ENEMY_H