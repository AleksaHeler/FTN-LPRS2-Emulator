#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <stdint.h>
#include "my_sprites.h"

#define mapWidth 24         // Map size
#define mapHeight 24
#define texWidth 64         // Texture size
#define texHeight 64

extern int worldMap[mapWidth][mapHeight];

extern uint32_t* textures[8];

#endif