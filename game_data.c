#include "game_data.h"

// The map of the world is a 2D array, where each value represents a square 
// If the value is 0, that square represents an empty, walkthroughable square, and if 
// the value is higher than 0, it represents a wall with  a certain color or texture.
// The map declared here is versprites_idx4y small, only 24 by 24 squares, and is defined directly 
// in the code. For a real game, like Wolfenstein 3D, you use a bigger map and load it 
// from a file instead. All the zero's in the grid are empty space, so basicly you see 
// a very big room, with a wall around it (the values 1), a small room inside it (the 
// values 2), a few pilars (the values 3), and a corridor with a room (the values 4).
int world_map[map_width][map_height]=
{  //0 1 2 3 4 5 6 7 8 9 10  12  14  16  18  20    23  <- Y-axis
    {8,8,8,8,8,8,8,8,8,8,8,4,4,6,4,4,6,4,6,4,4,4,6,4}, // 1  <- X-axis
    {8,0,0,0,0,0,0,0,0,0,8,4,0,0,0,0,0,0,0,0,0,0,0,4}, // 2
    {8,0,3,3,0,0,0,0,0,8,8,4,0,0,0,0,0,0,0,0,0,0,0,6}, // 3
    {8,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6}, // 4
    {8,0,3,3,0,0,0,0,0,8,8,4,0,0,0,0,0,0,0,0,0,0,0,4}, // 5
    {8,0,0,0,0,0,0,0,0,0,8,4,0,0,0,0,0,6,6,6,0,6,4,6}, // 6
    {8,8,8,8,0,8,8,8,8,8,8,4,4,4,4,4,4,6,0,0,0,0,0,6}, // 7
    {7,7,7,7,0,7,7,7,7,0,8,0,8,0,8,0,8,4,0,4,0,6,0,6}, // 8
    {7,7,0,0,0,0,0,0,7,8,0,8,0,8,0,8,8,6,0,0,0,0,0,6}, // 9
    {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,6,0,0,0,0,0,4}, // 10
    {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,6,0,6,0,6,0,6}, // 11
    {7,7,0,0,0,0,0,0,7,8,0,8,0,8,0,8,8,6,4,6,0,6,6,6}, // 12
    {7,7,7,7,0,7,7,7,7,8,8,4,0,6,8,4,8,3,3,3,0,3,3,3}, // 13
    {2,2,2,2,0,2,2,2,2,4,6,4,0,0,6,0,6,3,0,0,0,0,0,3}, // 14
    {2,2,0,0,0,0,0,2,2,4,0,0,0,0,0,0,4,3,0,0,0,0,0,3}, // 15
    {2,0,0,0,0,0,0,0,2,4,0,0,0,0,0,0,4,3,0,0,0,0,0,3}, // 16
    {1,0,0,0,0,0,0,0,1,4,4,4,4,4,6,0,6,3,3,0,0,0,3,3}, // 17
    {2,0,0,0,0,0,0,0,2,2,2,1,2,2,2,6,6,0,0,5,0,5,0,5}, // 18
    {2,2,0,0,0,0,0,2,2,2,0,0,0,2,2,0,5,0,5,0,0,0,5,5}, // 19
    {2,0,0,0,0,0,0,0,2,0,0,0,0,0,2,5,0,5,0,5,0,5,0,5}, // 20
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5}, // 21
    {2,0,0,0,0,0,0,0,2,0,0,0,0,0,2,5,0,5,0,5,0,5,0,5}, // 22
    {2,2,0,0,0,0,0,2,2,2,0,0,0,2,2,0,5,0,5,0,0,0,5,5}, // 23
    {2,2,2,2,1,2,2,2,2,2,2,1,2,2,2,5,5,5,5,5,5,5,5,5}  // 24
};

// Wall, floor & ceiling textures
uint32_t* textures[8] = {
    bluestone__p, colorstone__p, eagle__p, greystone__p,
    mossy__p, purplestone__p, wood__p, redbrick__p
};

// Sprite textures
uint32_t* sprite_textures[5] = {
    barrel__p, greenlight__p, pillar__p, monster_doom__p, monster_afrit__p
};

// Compiler demands this
const uint8_t sprite_greenlight[] = { 1 };
const uint8_t sprite_pillar[] = { 2 };
const uint8_t sprite_barrel[] = { 0 };
const uint8_t sprite_monster_doom[] = { 3 };
const uint8_t sprite_monster_afrit[] = { 4 };

sprite_t sprites_data[SPRITES_MAX_NUM] =
{
    // Green lights in every room
    {FP32F(20.5), FP32F(11.5), 1, 0, sprite_greenlight},
    {FP32F(18.5), FP32F(4.5), 1, 0, sprite_greenlight},
    {FP32F(10.0), FP32F(4.5), 1, 0, sprite_greenlight},
    {FP32F(10.0), FP32F(12.5), 1, 0, sprite_greenlight},
    {FP32F(3.5), FP32F(6.5), 1, 0, sprite_greenlight},
    {FP32F(3.5), FP32F(20.5), 1, 0, sprite_greenlight},
    {FP32F(3.5), FP32F(14.5), 1, 0, sprite_greenlight},
    {FP32F(14.5), FP32F(20.5), 1, 0, sprite_greenlight},

    // Row of pillars in front of wall: fisheye test
    {FP32F(18.5), FP32F(10.5), 1, 0, sprite_pillar},
    {FP32F(18.5), FP32F(11.5), 1, 0, sprite_pillar},
    {FP32F(18.5), FP32F(12.5), 1, 0, sprite_pillar},

    // Some barrels around the map
    {FP32F(21.5), FP32F(1.5), 1, 0, sprite_barrel},
    {FP32F(15.5), FP32F(1.5), 1, 0, sprite_barrel},
    {FP32F(16.0), FP32F(1.8), 1, 0, sprite_barrel},
    {FP32F(16.2), FP32F(1.2), 1, 0, sprite_barrel},
    {FP32F(3.5), FP32F(2.5), 1, 0, sprite_barrel},
    {FP32F(9.5), FP32F(15.5), 1, 0, sprite_barrel},
    {FP32F(10.0), FP32F(15.1), 1, 0, sprite_barrel},
    {FP32F(10.5), FP32F(15.8), 1, 0, sprite_barrel},

    // Monsters in rooms
    {FP32F(21.5), FP32F(4.5), 1, 0, sprite_monster_doom},
    {FP32F(17.5), FP32F(4.5), 1, 0, sprite_monster_afrit},
    {FP32F(3.5), FP32F(15.5), 1, 0, sprite_monster_doom},
    {FP32F(3.5), FP32F(19.5), 1, 0, sprite_monster_afrit},
    {FP32F(15.5), FP32F(20.5), 1, 0, sprite_monster_doom}
};

enemy_t enemies_data[num_enemies] =
{
    {&sprites_data[19]},
    {&sprites_data[20]},
    {&sprites_data[21]},
    {&sprites_data[22]},
    {&sprites_data[23]}
};
