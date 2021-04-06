/* This file is 'renderer.c' in project files */

#include "renderer.h"

void draw_sprite(uint32_t* src_p, uint16_t src_w, uint16_t src_h, uint16_t dst_x, uint16_t dst_y) {
    // Prodje kroz region na ekranu na kom treba biti iscrtano, i za svaki piksel nadje vrednost
    for(uint16_t x = 0; x < src_w; x++){ 
        for(uint16_t y = 0; y < src_h; y++){
            uint32_t dst_idx = (dst_y+y)*SCREEN_W + dst_x+x;
            uint32_t src_idx = (y)*(src_w/8) + x/8;
			
            uint32_t pixel = src_p[src_idx] >> (x%8)*4;
        
            unpack_idx4_p32[dst_idx] = pixel;
		}
	}
}


void renderer_init() {
    // Setup
	gpu_p32[0] = INDEX_MODE;
	gpu_p32[1] = USE_PACKED;

	// Setting colors
    // Copy colors from 'sprites_data.c'
    for(int i = 0; i < 16; i++){
        palette_p32[i] = palette[i];
    }
	gpu_p32[0x800] = 0x00ff00ff;    // Magenta for HUD.
}

void renderer_render(camera_t* camera) {
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
        double rayDirX = camera->dirX + camera->planeX * cameraX;
        double rayDirY = camera->dirY + camera->planeY * cameraX;

        // Which box of the map we're in (just rounding to int), basically the index in map matrix
        int mapX = (int)camera->posX;
        int mapY = (int)camera->posY;

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
            sideDistX = (camera->posX - mapX) * deltaDistX;
        } else { // rayDirX > 0
            stepX = 1;
            sideDistX = (mapX + 1.0 - camera->posX) * deltaDistX;
        }
        if(rayDirY < 0) {
            stepY = -1;
            sideDistY = (camera->posY - mapY) * deltaDistY;
        } else { // rayDirY > 0
            stepY = 1;
            sideDistY = (mapY + 1.0 - camera->posY) * deltaDistY;
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
        if(side == 0) perpWallDist = (mapX - camera->posX + (1 - stepX) / 2) / rayDirX;
        else          perpWallDist = (mapY - camera->posY + (1 - stepY) / 2) / rayDirY;


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
        
        // Choose wall texture from map
        // Number 1 on map corresponds with texture 0 and so on
        int texNum = worldMap[mapX][mapY] - 1;

        // Calculate value of wallX 
        double wallX; //where exactly the wall was hit
        if(side == 0) wallX = camera->posY + perpWallDist * rayDirY;
        else          wallX = camera->posX + perpWallDist * rayDirX;
        wallX -= floor((wallX));

        // X coordinate on the texture 
        int texX = (int)(wallX * ((double)texWidth)); // pos on the wall texture
        if(side == 0 && rayDirX > 0) texX = texWidth - texX - 1;
        if(side == 1 && rayDirY < 0) texX = texWidth - texX - 1;

        // TODO: an integer-only bresenham or DDA like algorithm could make the texture coordinate stepping faster
        // How much to increase the texture coordinate per screen pixel
        double step = 1.0 * texHeight / lineHeight;
        // Starting texture coordinate
        double texPos = (drawStart - SCREEN_H / 2 + lineHeight / 2) * step;

        // Draw the texture vertical stripe
        for(int y = drawStart; y < drawEnd; y++) {
            // Cast the texture coordinate to integer, and mask with (texHeight - 1) in case of overflow
            int texY = (int)texPos & (texHeight - 1);
            texPos += step;

            // Get the indices of source texture pixel and destination in buffer
            uint32_t dst_idx = y*SCREEN_W + x;
            uint32_t src_idx = texY*(texWidth/8) + texX/8;
            // Get the pixel color and write to buffer
            uint32_t color = textures[texNum][src_idx] >> (texX%8)*4;
            unpack_idx4_p32[dst_idx] = color;
        }
        
    } /// End of raycaster for loop ///
}
