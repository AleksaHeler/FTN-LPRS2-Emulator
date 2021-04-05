#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <stdint.h>
#include "engine.h"
#include "my_sprites.h"

#define mapWidth 24         // Map size
#define mapHeight 24
#define texWidth 64         // Texture size
#define texHeight 64
#define numSprites 19       // How many sprites in the map

extern int worldMap[mapWidth][mapHeight];

extern uint32_t* textures[8];
extern uint32_t* sprites[3];

extern sprite_t sprites_data[numSprites];

#endif