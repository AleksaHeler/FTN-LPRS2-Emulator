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
{
  {8,8,8,8,8,8,8,8,8,8,8,4,4,6,4,4,6,4,6,4,4,4,6,4},
  {8,0,0,0,0,0,0,0,0,0,8,4,0,0,0,0,0,0,0,0,0,0,0,4},
  {8,0,3,3,0,0,0,0,0,8,8,4,0,0,0,0,0,0,0,0,0,0,0,6},
  {8,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
  {8,0,3,3,0,0,0,0,0,8,8,4,0,0,0,0,0,0,0,0,0,0,0,4},
  {8,0,0,0,0,0,0,0,0,0,8,4,0,0,0,0,0,6,6,6,0,6,4,6},
  {8,8,8,8,0,8,8,8,8,8,8,4,4,4,4,4,4,6,0,0,0,0,0,6},
  {7,7,7,7,0,7,7,7,7,0,8,0,8,0,8,0,8,4,0,4,0,6,0,6},
  {7,7,0,0,0,0,0,0,7,8,0,8,0,8,0,8,8,6,0,0,0,0,0,6},
  {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,6,0,0,0,0,0,4},
  {7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,6,0,6,0,6,0,6},
  {7,7,0,0,0,0,0,0,7,8,0,8,0,8,0,8,8,6,4,6,0,6,6,6},
  {7,7,7,7,0,7,7,7,7,8,8,4,0,6,8,4,8,3,3,3,0,3,3,3},
  {2,2,2,2,0,2,2,2,2,4,6,4,0,0,6,0,6,3,0,0,0,0,0,3},
  {2,2,0,0,0,0,0,2,2,4,0,0,0,0,0,0,4,3,0,0,0,0,0,3},
  {2,0,0,0,0,0,0,0,2,4,0,0,0,0,0,0,4,3,0,0,0,0,0,3},
  {1,0,0,0,0,0,0,0,1,4,4,4,4,4,6,0,6,3,3,0,0,0,3,3},
  {2,0,0,0,0,0,0,0,2,2,2,1,2,2,2,6,6,0,0,5,0,5,0,5},
  {2,2,0,0,0,0,0,2,2,2,0,0,0,2,2,0,5,0,5,0,0,0,5,5},
  {2,0,0,0,0,0,0,0,2,0,0,0,0,0,2,5,0,5,0,5,0,5,0,5},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5},
  {2,0,0,0,0,0,0,0,2,0,0,0,0,0,2,5,0,5,0,5,0,5,0,5},
  {2,2,0,0,0,0,0,2,2,2,0,0,0,2,2,0,5,0,5,0,0,0,5,5},
  {2,2,2,2,1,2,2,2,2,2,2,1,2,2,2,5,5,5,5,5,5,5,5,5}
};

// Wall, floor & ceiling textures
uint32_t* textures[8] = {
    bluestone__p, colorstone__p, eagle__p, greystone__p,
    mossy__p, purplestone__p, wood__p, redbrick__p
};

// Billboard sprites
uint32_t* sprites[3] = {
    barrel__p, greenlight__p, pillar__p
};

sprite_t sprites_data[num_sprites] =
{
  {FP32F(20.5), FP32F(11.5), 1}, //green light in front of playerstart
  //green lights in every room
  {FP32F(18.5), FP32F(4.5), 1},
  {FP32F(10.0), FP32F(4.5), 1},
  {FP32F(10.0), FP32F(12.5), 1},
  {FP32F(3.5), FP32F(6.5), 1},
  {FP32F(3.5), FP32F(20.5), 1},
  {FP32F(3.5), FP32F(14.5), 1},
  {FP32F(14.5), FP32F(20.5), 1},

  //row of pillars in front of wall: fisheye test
  {FP32F(18.5), FP32F(10.5), 2},
  {FP32F(18.5), FP32F(11.5), 2},
  {FP32F(18.5), FP32F(12.5), 2},

  //some barrels around the map
  {FP32F(21.5), FP32F(1.5), 0},
  {FP32F(15.5), FP32F(1.5), 0},
  {FP32F(16.0), FP32F(1.8), 0},
  {FP32F(16.2), FP32F(1.2), 0},
  {FP32F(3.5), FP32F(2.5), 0},
  {FP32F(9.5), FP32F(15.5), 0},
  {FP32F(10.0), FP32F(15.1), 0},
  {FP32F(10.5), FP32F(15.8), 0},
};
