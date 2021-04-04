#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <stdint.h>
#include "my_sprites.h"

#define mapWidth 24
#define mapHeight 24

extern int worldMap[mapWidth][mapHeight];

extern uint32_t* images[8];

#endif