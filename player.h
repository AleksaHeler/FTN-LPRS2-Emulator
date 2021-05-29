#ifndef PLAYER_H
#define PLAYER_H

#include "engine.h"
#include "game_data.h"
#include "fmath.h"
#include "raycast_renderer.h"

extern camera_t player_camera;

void player_init();
int player_menu();
void player_update();

#endif