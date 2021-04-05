#include "renderer.h"

double ZBuffer[SCREEN_W];               // 1D 'depth/distance' Zbuffer
int spriteOrder[numSprites];            // Arrays used to sort the sprites
double spriteDistance[numSprites];

///////////////////////////////////////////////////////////////////////////////
// Draws whole sprite at the given location:
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

//sort algorithm
//sort the sprites based on distance
//The sortSprites sorts the sprites from farthest away to closest by distance. 
//It uses the standard std::sort function provided by C++. But since we need to 
//sort two arrays using the same order here (order and dist), most of the code 
//is spent moving the data into and out of a vector of pairs.
// sortSprites(spriteOrder, spriteDistance, numSprites);
/// TODO: implement a C variant of this function
/*void sortSprites(int* order, double* dist, int amount)
{
    std::vector<std::pair<double, int>> spr(amount);
    for(int i = 0; i < amount; i++) {
        spr[i].first = dist[i];
        spr[i].second = order[i];
    }
    std::sort(spr.begin(), spr.end());

    // restore in reverse order to go from farthest to nearest
    for(int i = 0; i < amount; i++) {
        dist[i] = spr[amount - i - 1].first;
        order[i] = spr[amount - i - 1].second;
    }
}*/
void sortSprites(int* order, double* dist, int amount){}

///////////////////////////////////////////////////////////////////////////////
// Initialization and setup of the renderer data
void renderer_init() {
    // Setup
	gpu_p32[0] = INDEX_MODE;
	gpu_p32[1] = USE_PACKED;

	// Setting colors
    // Copy colors from 'my_sprites.c'
    for(int i = 0; i < 16; i++){
        palette_p32[i] = palette[i];
    }
    // Green for HUD
	gpu_p32[0x800] = 0x0000FF00;
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
    // Floor raycaster: for every horizontal line from middle to the bottom of the screen
    for(int y = SCREEN_H / 2 + 1; y < SCREEN_H; ++y)
    {
      // rayDir for leftmost ray (x = 0) and rightmost ray (x = w)
      float rayDirX0 = camera->dirX - camera->planeX;
      float rayDirY0 = camera->dirY - camera->planeY;
      float rayDirX1 = camera->dirX + camera->planeX;
      float rayDirY1 = camera->dirY + camera->planeY;

      // Current y position compared to the center of the screen (the horizon)
      int p = y - SCREEN_H / 2;

      // Vertical position of the camera.
      // NOTE: with 0.5, it's exactly in the center between floor and ceiling,
      // matching also how the walls are being raycasted. For different values
      // than 0.5, a separate loop must be done for ceiling and floor since
      // they're no longer symmetrical.
      float posZ = 0.5 * SCREEN_H;

      // Horizontal distance from the camera to the floor for the current row.
      // 0.5 is the z position exactly in the middle between floor and ceiling.
      // NOTE: this is affine texture mapping, which is not perspective correct
      // except for perfectly horizontal and vertical surfaces like the floor.
      // NOTE: this formula is explained as follows: The camera ray goes through
      // the following two points: the camera itself, which is at a certain
      // height (posZ), and a point in front of the camera (through an imagined
      // vertical plane containing the screen pixels) with horizontal distance
      // 1 from the camera, and vertical position p lower than posZ (posZ - p). When going
      // through that point, the line has vertically traveled by p units and
      // horizontally by 1 unit. To hit the floor, it instead needs to travel by
      // posZ units. It will travel the same ratio horizontally. The ratio was
      // 1 / p for going through the camera plane, so to go posZ times farther
      // to reach the floor, we get that the total horizontal distance is posZ / p.
      float rowDistance = posZ / p;

      // calculate the real world step vector we have to add for each x (parallel to camera plane)
      // adding step by step avoids multiplications with a weight in the inner loop
      float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / SCREEN_W;
      float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / SCREEN_W;

      // real world coordinates of the leftmost column. This will be updated as we step to the right.
      float floorX = camera->posX + rowDistance * rayDirX0;
      float floorY = camera->posY + rowDistance * rayDirY0;

      for(int x = 0; x < SCREEN_W; ++x)
      {
        // the cell coord is simply got from the integer parts of floorX and floorY
        int cellX = (int)(floorX);
        int cellY = (int)(floorY);

        // get the texture coordinate from the fractional part
        int tx = (int)(texWidth * (floorX - cellX)) & (texWidth - 1);
        int ty = (int)(texHeight * (floorY - cellY)) & (texHeight - 1);

        floorX += floorStepX;
        floorY += floorStepY;

        // choose texture and draw the pixel
        int checkerBoardPattern = ((int)(cellX + cellY)) & 1;
        int floorTexture;
        if(checkerBoardPattern == 0) floorTexture = 3;
        else floorTexture = 4;
        int ceilingTexture = 6;
        uint32_t color;

        // floor
        uint32_t dst_idx = y*SCREEN_W + x;
        uint32_t src_idx = texWidth/8 * ty + tx/8;
        color = textures[floorTexture][src_idx] >> (tx%8)*4;
        unpack_idx4_p32[dst_idx] = color;

        //ceiling (symmetrical, at screenHeight - y - 1 instead of y)
        dst_idx = (SCREEN_H-y-1)*SCREEN_W + x;
        src_idx = (texWidth/8) * ty + tx/8;
        color = textures[ceilingTexture][src_idx] >> (tx%8)*4;
        unpack_idx4_p32[dst_idx] = color;
      }
    }
    
    /////////////////////////////////////
    // Wall raycaster: for every vertical line on the screen
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
        wallX -= (int)(wallX);

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

        // Set the Z buffer (depth) for sprite casting
        ZBuffer[x] = perpWallDist; //perpendicular distance is used
        
    } /// End of raycaster for loop ///

    /*
    To bring the sprite's coordinates to camera space, first subtract the player's position 
    from the sprite's position, then you have the position of the sprite relative to the player. 
    Then it has to be rotated so that the direction is relative to the player. The camera 
    can also be skewed and has a certain size, so it isn't really a rotation, but a transformation. 
    The transformation is done by multiplying the relative position of the sprite with the inverse 
    of the camera matrix. The camera matrix is in our case

    [planeX   dirX]
    [planeY   dirY]

    And the inverse of a 2x2 matrix is very easy to calculate

    ____________1___________    [dirY      -dirX]
    (planeX*dirY-dirX*planeY) * [-planeY  planeX]


    Then you get the X and Y coordinate of the sprite in camera space, where Y is the depth inside 
    the screen (in a true 3D engine, Z is the depth). To project it on screen, divide X through the 
    depth, and then translate and scale it so that it's in pixel coordinates.
    */
    
    /////////////////////////////////////
    // Sprite casting:
    for(int i = 0; i < numSprites; i++) { //sort all sprites from far to close
        spriteOrder[i] = i;
        spriteDistance[i] = ((camera->posX - sprites_data[i].x) * (camera->posX - sprites_data[i].x) + (camera->posY - sprites_data[i].y) * (camera->posY - sprites_data[i].y)); //sqrt not taken, unneeded
    }
    sortSprites(spriteOrder, spriteDistance, numSprites);

    // After sorting the sprites, do the projection and draw them
    for(int i = 0; i < numSprites; i++) {
        // Translate sprite position to relative to camera
        double spriteX = sprites_data[spriteOrder[i]].x - camera->posX;
        double spriteY = sprites_data[spriteOrder[i]].y - camera->posY;

        //transform sprite with the inverse camera matrix
        // [ planeX   dirX ] -1                                       [ dirY      -dirX ]
        // [               ]       =  1/(planeX*dirY-dirX*planeY) *   [                 ]
        // [ planeY   dirY ]                                          [ -planeY  planeX ]

        double invDet = 1.0 / (camera->planeX * camera->dirY - camera->dirX * camera->planeY); //required for correct matrix multiplication

        double transformX = invDet * (camera->dirY * spriteX - camera->dirX * spriteY);
        double transformY = invDet * (-camera->planeY * spriteX + camera->planeX * spriteY); //this is actually the depth inside the screen, that what Z is in 3D

        int spriteScreenX = (int)((SCREEN_W / 2) * (1 + transformX / transformY));

        //calculate height of the sprite on screen
        int spriteHeight = ABS((int)(SCREEN_H / (transformY))); //using 'transformY' instead of the real distance prevents fisheye
        //calculate lowest and highest pixel to fill in current stripe
        int drawStartY = -spriteHeight / 2 + SCREEN_H / 2;
        if(drawStartY < 0) drawStartY = 0;
        int drawEndY = spriteHeight / 2 + SCREEN_H / 2;
        if(drawEndY >= SCREEN_H) drawEndY = SCREEN_H - 1;

        //calculate width of the sprite
        int spriteWidth = ABS((int)(SCREEN_H / (transformY)));
        int drawStartX = -spriteWidth / 2 + spriteScreenX;
        if(drawStartX < 0) drawStartX = 0;
        int drawEndX = spriteWidth / 2 + spriteScreenX;
        if(drawEndX >= SCREEN_W) drawEndX = SCREEN_W - 1;

        //loop through every vertical stripe of the sprite on screen
        for(int stripe = drawStartX; stripe < drawEndX; stripe++) {
            int texX = (int)(256 * (stripe - (-spriteWidth / 2 + spriteScreenX)) * texWidth / spriteWidth) / 256;
            //the conditions in the if are:
            //1) it's in front of camera plane so you don't see things behind you
            //2) it's on the screen (left)
            //3) it's on the screen (right)
            //4) ZBuffer, with perpendicular distance
            if(transformY > 0 && stripe > 0 && stripe < SCREEN_W && transformY < ZBuffer[stripe])
            for(int y = drawStartY; y < drawEndY; y++){ //for every pixel of the current stripe  
                int d = (y) * 256 - SCREEN_H * 128 + spriteHeight * 128; //256 and 128 factors to avoid floats
                int texY = ((d * texHeight) / spriteHeight) / 256;
                uint32_t src_idx = texY*(texWidth/8) + texX/8;
                uint32_t dst_idx = y*SCREEN_W + stripe;
                uint32_t color = sprites[sprites_data[spriteOrder[i]].texture][src_idx] >> (texX%8)*4 & 0xF; //get current color from the texture
                if(color != 0xd) 
                    unpack_idx4_p32[dst_idx] = color; //paint pixel if it isn't white, white is the invisible color
            }
        }
    }
}