#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <stdint.h>
#include "engine.h"
#include "sprites_data.h"

#define map_width 24         // Map size
#define map_height 24
#define tex_width 64         // Texture size
#define tex_height 64
#define num_sprites 19       // How many sprites in the map

extern int world_map[map_width][map_height];

extern uint8_t* textures[8];
extern uint8_t* sprites[3];

extern sprite_t sprites_data[num_sprites];

#endif