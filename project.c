
///////////////////////////////////////////////////////////////////////////////
// Convert images to src files:
// python img_to_src.py -v -f IDX4 -o bluestone.c images/bluestone.png 
//    -p 72a6e4,506fb8,393f74,32283f,35289e,443be5,2b92fb,62e7ff,dc663,457332,3f3d19,81674f,d2bfaf,ffffff,f4e82c,d18404


///////////////////////////////////////////////////////////////////////////////
// Headers:
#include <stdint.h>
#include "system.h"
#include <stdio.h>
#include <math.h> // TODO: remove this library and use our own sin() and cos() functions
// Images
#include "my_sprites.h"

///////////////////////////////////////////////////////////////////////////////
// Useful macros:
#define ABS(N) ((N<0)?(-N):(N))
#define WAIT_UNITL_0(x) while(x != 0){}
#define WAIT_UNITL_1(x) while(x != 1){}


///////////////////////////////////////////////////////////////////////////////
// HW stuff:
#define SCREEN_IDX1_W 640
#define SCREEN_IDX1_H 480
#define SCREEN_IDX4_W 320
#define SCREEN_IDX4_H 240
#define SCREEN_RGB333_W 160
#define SCREEN_RGB333_H 120

#define SCREEN_IDX4_W8 (SCREEN_IDX4_W/8)

#define gpu_p32 ((volatile uint32_t*)LPRS2_GPU_BASE)
#define palette_p32 ((volatile uint32_t*)(LPRS2_GPU_BASE+0x1000))
#define unpack_idx1_p32 ((volatile uint32_t*)(LPRS2_GPU_BASE+0x400000))
#define pack_idx1_p32 ((volatile uint32_t*)(LPRS2_GPU_BASE+0x600000))
#define unpack_idx4_p32 ((volatile uint32_t*)(LPRS2_GPU_BASE+0x800000))
#define pack_idx4_p32 ((volatile uint32_t*)(LPRS2_GPU_BASE+0xa00000))
#define unpack_rgb333_p32 ((volatile uint32_t*)(LPRS2_GPU_BASE+0xc00000))
#define joypad_p32 ((volatile uint32_t*)LPRS2_JOYPAD_BASE)


///////////////////////////////////////////////////////////////////////////////
// Game config:
// TODO: parametrize stuff
#define INDEX_MODE 2 // IDX4 - 4b color
#define SCREEN_W SCREEN_IDX4_W
#define SCREEN_H SCREEN_IDX4_H
#define USE_PACKED 0 // Use unpacked indexing


///////////////////////////////////////////////////////////////////////////////
// Indexing structs:
typedef struct {
	unsigned a      : 1;
	unsigned b      : 1;
	unsigned z      : 1;
	unsigned start  : 1;
	unsigned up     : 1;
	unsigned down   : 1;
	unsigned left   : 1;
	unsigned right  : 1;
} bf_joypad;
#define joypad (*((volatile bf_joypad*)LPRS2_JOYPAD_BASE))

typedef struct {
	uint32_t m[SCREEN_H][SCREEN_W];
} bf_unpack_idx1;
#define unpack_idx1 (*((volatile bf_unpack_idx1*)unpack_idx1_p32))

uint32_t* images[8] = {
	bluestone__p, colorstone__p, eagle__p, greystone__p,
	mossy__p, purplestone__p, redbrick__p, wood__p,
};


///////////////////////////////////////////////////////////////////////////////
// Game data:
#define mapWidth 24
#define mapHeight 24

// The map of the world is a 2D array, where each value represents a square 
// If the value is 0, that square represents an empty, walkthroughable square, and if 
// the value is higher than 0, it represents a wall with  a certain color or texture.
// The map declared here is versprites_idx4y small, only 24 by 24 squares, and is defined directly 
// in the code. For a real game, like Wolfenstein 3D, you use a bigger map and load it 
// from a file instead. All the zero's in the grid are empty space, so basicly you see 
// a very big room, with a wall around it (the values 1), a small room inside it (the 
// values 2), a few pilars (the values 3), and a corridor with a room (the values 4).
int worldMap[mapWidth][mapHeight]=
{
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,3,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};


///////////////////////////////////////////////////////////////////////////////
// Helper functions:
static inline uint32_t shift_div_with_round_down(uint32_t num, uint32_t shift){
	uint32_t d = num >> shift;
	return d;
}

static inline uint32_t shift_div_with_round_up(uint32_t num, uint32_t shift){
	uint32_t d = num >> shift;
	uint32_t mask = (1<<shift)-1;
	if((num & mask) != 0){
		d++;
	}
	return d;
}

static void draw_sprite(uint32_t* src_p, uint16_t src_w, uint16_t src_h, uint16_t dst_x, uint16_t dst_y) {
	// Using unpacked idx4 mode
    uint16_t dst_x8 = shift_div_with_round_down(dst_x, 3);
	uint16_t src_w8 = shift_div_with_round_up(src_w, 3);
	
    // TODO: MAKE THIS WORK
    /*for(uint16_t y = 0; y < src_h; y++){
		for(uint16_t x8 = 0; x8 < src_w8; x8++){
			uint32_t src_idx = y*src_w8 + x8;
			uint32_t pixels = src_p[src_idx];
			uint32_t dst_idx =
				(dst_y+y)*SCREEN_IDX4_W8 +
				(dst_x8+x8);
			pack_idx4_p32[dst_idx] = pixels;
		}
	}*/
}


///////////////////////////////////////////////////////////////////////////////
// Game code:
int main(void) {
	
	// Setup
	gpu_p32[0] = INDEX_MODE;
	gpu_p32[1] = USE_PACKED;

	// Setting colors
	palette_p32[0] = 0x72a6e4;      //
	palette_p32[1] = 0x506fb8;      //
	palette_p32[2] = 0x393f74;      //
	palette_p32[3] = 0x32283f;      //
	palette_p32[4] = 0x35289e;      //
	palette_p32[5] = 0x443be5;      //
	palette_p32[6] = 0x2b92fb;      //
	palette_p32[7] = 0x62e7ff;      //
	palette_p32[8] = 0xdc663;      //
	palette_p32[9] = 0x457332;      //
	palette_p32[10] = 0x3f3d19;     //
	palette_p32[11] = 0x81674f;     //
	palette_p32[12] = 0xd2bfaf;     //
	palette_p32[13] = 0xffffff;     //
	palette_p32[14] = 0xf4e82c;     //
	palette_p32[15] = 0xd18404;     //
	gpu_p32[0x800] = 0x00ff00ff;    // Magenta for HUD.

	// Player & camera data
    double posX = 22, posY = 12;  // player x and y start position
    double dirX = -1, dirY = 0; // initial player direction vector

    // Camera plane is perpendicular to the direction, but we 
    //  can change the length of it. The ratio between the length
    //  of the direction and the camera plane determinates the FOV.
    //  FOV is 2 * atan(0.66/1.0)=66°, which is perfect for a first person shooter game
    double planeX = 0, planeY = 0.66; // the 2d raycaster version of camera plane
	
	
	while(1){
		
		/////////////////////////////////////
		// Poll controls:
		// The speed modifiers use frameTime, and a constant value, to 
        // determinate the speed of the moving and rotating of the input 
        // keys. Thanks to using the frameTime, we can make sure that the 
        // moving and rotating speed is independent of the processor speed
        //double moveSpeed = frameTime * 5.0; //the constant value is in squares/second
        //double rotSpeed = frameTime * 3.0;  //the constant value is in radians/second
        double moveSpeed = 5.0/60.0;
        double rotSpeed = 3.0/60.0;
		
		// Move forward if no wall in front of the player
        if(joypad.up) {
            if(worldMap[(int)(posX + dirX * moveSpeed)][(int)posY] == 0) 
                posX += dirX * moveSpeed;
            if(worldMap[(int)(posX)][(int)(posY + dirY * moveSpeed)] == 0) 
                posY += dirY * moveSpeed;
        }
        // Move backwards if no wall behind the player
        if(joypad.down) {
            if(worldMap[(int)(posX - dirX * moveSpeed)][(int)posY] == 0) posX -= dirX * moveSpeed;
            if(worldMap[(int)(posX)][(int)(posY - dirY * moveSpeed)] == 0) posY -= dirY * moveSpeed;
        }

        // Rotate to the right
        if(joypad.right) {
            // Both camera direction and camera plane must be rotated
            // Rotating vectors by multiplying it with rotation matrix:
            //   [ cos(a) -sin(a) ]
            //   [ sin(a)  cos(a) ]
            double oldDirX = dirX;
            dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
            dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
            double oldPlaneX = planeX;
            planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
            planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
        }
        // Rotate to the left
        if(joypad.left) {
            // Both camera direction and camera plane must be rotated
            double oldDirX = dirX;
            dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
            dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
            double oldPlaneX = planeX;
            planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
            planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
        }
		
		
		/////////////////////////////////////
		// Gameplay:
		// TODO
		
		
		/////////////////////////////////////
		// Drawing:
		WAIT_UNITL_0(gpu_p32[2]);   // Detecting rising edge of VSync
		WAIT_UNITL_1(gpu_p32[2]);   // Draw in buffer while it is in VSync
		
		
		// Clear background to base color
        for(uint16_t r = 0; r < SCREEN_H; r++){
			for(uint16_t c = 0; c < SCREEN_W; c++){
				uint32_t idx = r*SCREEN_W + c;
				unpack_idx4_p32[idx] = 0;
			}
		}
		
		/////////////////////////////////////
		// Raycaster: for every vertical line on the screen
		for(int x = 0; x < SCREEN_W; x++) {
		
			// 'cameraX' is the x-coordinate on the camera plane that the current 
            // x-coordinate of the screen represents, done this way so that the 
            // right side of the screen will get coordinate 1, the center of the 
            // screen gets coordinate 0, and the left side of the screen gets 
            // coordinate -1. 
            //                  \     
            //       ____________x______________________ camera plane
            //       -1           \   |                1
        	//                  ray\  |dir
            //                      \ |
            //                       \|
            //                      player
            // Out of this, the direction of the ray can be calculated
            // as the sum of the direction vector, and a part of the plane 
            // vector. This has to be done both for the x and y coordinate of the 
            // vector (since adding two vectors is adding their x-coordinates, and 
            // adding their y-coordinates). 
            double cameraX = 2 * x / (double)SCREEN_W - 1; //x-coordinate in camera space
            double rayDirX = dirX + planeX * cameraX;
            double rayDirY = dirY + planeY * cameraX;

            // Which box of the map we're in (just rounding to int), basically the index in map matrix
            int mapX = (int)posX;
            int mapY = (int)posY;

            // Length of ray from current position to next x or y-side.
            // In this engine when we raycast we dont look down the ray by stepping some amount,
            // but by going from one dividing line of the map squares to the next, so we dont miss
            // the wall in edge case scenarios 
            double sideDistX;
            double sideDistY;

			// The distance the ray has to travel to go from 1 x-side 
            // to the next x-side, or from 1 y-side to the next y-side.
            // When deriving deltaDistX geometrically you get, with Pythagoras, the formulas:
            //   deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX))
            //   deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY))
            // But this can be simplified to:
            //   deltaDistX = abs(|v| / rayDirX)
            //   deltaDistY = abs(|v| / rayDirY)
            // Where |v| is the length of the vector rayDirX, rayDirY (that is 
            // sqrt(rayDirX * rayDirX + rayDirY * rayDirY)). However, we can 
            // use 1 instead of |v|, because only the *ratio* between deltaDistX 
            // and deltaDistY matters for the DDA code that follows later below, so we get:
            //   deltaDistX = abs(1 / rayDirX)
            //   deltaDistY = abs(1 / rayDirY) 
            double deltaDistX = ABS(1 / rayDirX);
            double deltaDistY = ABS(1 / rayDirY);

            // TODO: Make this work
            // Alternative code for deltaDist in case division through zero is not supported.
            // the following will make the DDA loop also work correctly by instead setting the finite one to 0.
            //double deltaDistX = (rayDirY == 0) ? 0 : ((rayDirX == 0) ? 1 : abs(1 / rayDirX));
            //double deltaDistY = (rayDirX == 0) ? 0 : ((rayDirY == 0) ? 1 : abs(1 / rayDirY));

            // Will be used later to calculate the length of the ray
            double perpWallDist;

            // The DDA algorithm will always jump exactly one square each loop, either 
            // a square in the x-direction, or a square in the y-direction. If it has to 
            // go in the negative or positive x-direction, and the negative or positive 
            // y-direction will depend on the direction of the ray, and this fact will 
        	// be stored in stepX and stepY.
            int stepX;
            int stepY;

            // Used to determinate whether or not the coming loop may be ended (was there a wall hit?)
            int hit = 0;
            // Will contain if an x-side or a y-side of a wall was hit. If an x-side was hit, 
            // side is set to 0, if an y-side was hit, side will be 1. By x-side and y-side, 
            // I mean the lines of the grid that are the borders between two squares.
            int side;

			// Now, before the actual DDA can start, first stepX, stepY, and the 
            // initial sideDistX and sideDistY still have to be calculated.
            if(rayDirX < 0) {
                stepX = -1;
                sideDistX = (posX - mapX) * deltaDistX;
            } else { // rayDirX > 0
                stepX = 1;
                sideDistX = (mapX + 1.0 - posX) * deltaDistX;
            }
            if(rayDirY < 0) {
                stepY = -1;
                sideDistY = (posY - mapY) * deltaDistY;
            } else { // rayDirY > 0
                stepY = 1;
                sideDistY = (mapY + 1.0 - posY) * deltaDistY;
            }

            // --- Perform DDA ---
            // A loop that increments the ray with one 
            // square every time, until a wall is hit 
            while (hit == 0)
            {
                // Either jumps a square in the x-direction (with stepX) or a square 
                // in the y-direction (with stepY), it always jumps 1 square at once.
                if(sideDistX < sideDistY) {
                    // sideDistX and sideDistY get incremented with deltaDistX with 
                    // every jump in their direction, and mapX and mapY get incremented 
                    // with stepX and stepY respectively.
                    sideDistX += deltaDistX;
                    mapX += stepX;
                    side = 0;
                }
                else {
                    sideDistY += deltaDistY;
                    mapY += stepY;
                    side = 1;
                }

                // Check if the ray has hit a wall (current map square is not empty)
                if(worldMap[mapX][mapY] > 0) hit = 1;
            }

			// We won't know exactly where the wall was hit however, but that's 
            // not needed in this case because we won't use textured walls for now

            // Calculate distance projected on camera direction. We don't use the 
            // Euclidean distance to the point representing player, but instead 
            // the distance to the camera plane (or, the distance of the point 
            // projected on the camera direction to the player), to avoid the 
            // fisheye effect. The fisheye effect is an effect you see if you 
            // use the real distance, where all the walls become rounded, and 
            // can make you sick if you rotate 
            // The distance is then calculated as follows: if an x-side is hit, 
            // mapX - posX + (1-stepX)/2) is the number of squares the ray has 
            // crossed in X direction (this is not necessarily a whole number). 
            // If the ray is perpendicular to the X side, this is the correct 
            // value already, but because the direction of the ray is different 
            // most of the times, its real perpendicular distance will be larger, 
            // so we divide it through the X coordinate of the rayDir vector 
            if(side == 0) perpWallDist = (mapX - posX + (1 - stepX) / 2) / rayDirX;
            else          perpWallDist = (mapY - posY + (1 - stepY) / 2) / rayDirY;


            // Now that we have the calculated distance (perpWallDist), we can 
            // calculate the height of the line that has to be drawn on screen: 
            // this is the inverse of perpWallDist, and then multiplied by h, the 
            // height in pixels of the screen, to bring it to pixel coordinates.
            int lineHeight = (int)(SCREEN_H / perpWallDist);

            // Calculate lowest and highest pixel to fill in current stripe. 
            // The center of the wall should be at the center of the screen, and 
            // if these points lie outside the screen, they're capped to 0 or h-1
            int drawStart = -lineHeight / 2 + SCREEN_H / 2;
            if(drawStart < 0)drawStart = 0;
            int drawEnd = lineHeight / 2 + SCREEN_H / 2;
            if(drawEnd >= SCREEN_H)drawEnd = SCREEN_H - 1;


			// Choose wall color
            int color = 0;
            switch(worldMap[mapX][mapY])
            {
                case 1:  color = 1; break;
                case 2:  color = 3; break;
                case 3:  color = 5; break;
                case 4:  color = 7; break;
                default: color = 9; break;
            }

            // If an y-side was hit, the color is made darker, this gives a nicer effect
            if(side == 1) { color++; } // Colors palette is in pairs of light and then dark version of the color

            // Draw the calculated verical line in right color
			for(int r = drawStart; r < drawEnd; r++){ // 
                unpack_idx4_p32[r*SCREEN_W + x] = color;
			}
            
		} /// End of raycaster for loop ///
        
        // Draw an example image over everything just to test how it works
        draw_sprite(images[0], 64, 64, 30, 30);
        draw_sprite(images[1], 64, 64, 100, 30);

	} /// End of main while loop ///

	return 0;
} /// End of main ///

///////////////////////////////////////////////////////////////////////////////
