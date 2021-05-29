#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <stdint.h>
#include "engine.h"
#include "sprites_data.h"

#define map_width 24            // Map size
#define map_height 24
#define tex_width 64            // Texture size
#define tex_height 64
#define tex_width_shift 6       // 2^6 = 64
#define tex_height_shift 6
#define SPRITES_MAX_NUM 24      // How many sprites in the map (3 pillars + 8 lights + 8 barrels)
#define num_enemies 5           // How many enemies in the map

extern int world_map[map_width][map_height];

extern uint32_t* textures[8];
extern uint32_t* sprite_textures[5];

extern sprite_t sprites_data[SPRITES_MAX_NUM];
extern enemy_t enemies_data[num_enemies];

#endif