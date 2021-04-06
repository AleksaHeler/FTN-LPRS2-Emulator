#include "renderer.h"


double z_buffer[SCREEN_W];               // 1D 'depth/distance' z_buffer
int sprite_order[numSprites];            // Arrays used to sort the sprites
double sprite_distance[numSprites];


///////////////////////////////////////////////////////////////////////////////
// Initialization and setup of the renderer data. Copies colors from palette in 'sprites_data.c'
void renderer_init() {
	gpu_p32[0] = INDEX_MODE;
	gpu_p32[1] = USE_PACKED;

    for(int i = 0; i < 16; i++)         // Copy colors from 'sprites_data.c'
        palette_p32[i] = palette[i];

	gpu_p32[0x800] = 0x0000FF00;        // Green for HUD
}

///////////////////////////////////////////////////////////////////////////////
// Rendering: floor&ceiling, walls, sprites
void renderer_render(camera_t* camera) {
    wait_for_vsync();

    cls();  // Clear background to color with index 0 in palette
    floor_raycaster(camera);
    wall_raycaster(camera);
    sort_sprites(sprite_order, sprite_distance, camera, numSprites);
    sprite_raycaster(camera);
}

///////////////////////////////////////////////////////////////////////////////
// Helper functions for renderer

// Detecting VSync pulse
void wait_for_vsync(){
    WAIT_UNITL_0(gpu_p32[2]);   // Wait for rising edge
    WAIT_UNITL_1(gpu_p32[2]);   // After falling edge return from function
}

// Clear background to color with index 0 in palette
void cls(){
    // Go trough the whole screen buffer and set to 0 
    uint32_t end = SCREEN_H * SCREEN_W;
    for(uint32_t i = 0; i < end; i++){
        unpack_idx4_p32[i] = 0;
    }
}

// Draw floor and ceiling in screen buffer
void floor_raycaster(camera_t* camera){
    // For every horizontal line from middle to the bottom of the screen
    for(int y = SCREEN_H / 2 + 1; y < SCREEN_H; ++y) {
        // rayDir for leftmost ray (x = 0) and rightmost ray (x = w)
        float rayDirX0 = camera->dirX - camera->planeX;
        float rayDirY0 = camera->dirY - camera->planeY;
        float rayDirX1 = camera->dirX + camera->planeX;
        float rayDirY1 = camera->dirY + camera->planeY;

        // Current y position compared to the center of the screen (the horizon)
        int p = y - SCREEN_H / 2;

        // Vertical position of the camera
        float posZ = 0.5 * SCREEN_H;

        // Horizontal distance from the camera to the floor for the current row
        float rowDistance = posZ / p;

        // Calculate the real world step vector we have to add for each x (parallel to camera plane)
        // adding step by step avoids multiplications with a weight in the inner loop
        float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / SCREEN_W;
        float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / SCREEN_W;

        // real world coordinates of the leftmost column. This will be updated as we step to the right.
        float floorX = camera->posX + rowDistance * rayDirX0;
        float floorY = camera->posY + rowDistance * rayDirY0;

        for(int x = 0; x < SCREEN_W; ++x){
            // the cell coord is simply derived from the integer parts of floorX and floorY
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
}

void wall_raycaster(camera_t* camera){
    // For every vertical line on the screen
    for(int x = 0; x < SCREEN_W; x++) {
        //                  \     
        //       __________camX_____________________ camera plane
        //       -1           \   |                1
        //                  ray\  |dir
        //                      \ |
        //                       \|
        //                      player
        double cameraX = 2 * x / (double)SCREEN_W - 1; //x-coordinate in camera space
        double rayDirX = camera->dirX + camera->planeX * cameraX;
        double rayDirY = camera->dirY + camera->planeY * cameraX;

        // Which box of the map we're in (just rounding to int), basically the index in map matrix
        int mapX = (int)camera->posX;
        int mapY = (int)camera->posY;

        // Length of ray from current position to next x or y-side
        double sideDistX;
        double sideDistY;

        // The distance the ray has to travel to go from 1 x-side 
        // to the next x-side, or from 1 y-side to the next y-side.
        double deltaDistX = ABS(1 / rayDirX);
        double deltaDistY = ABS(1 / rayDirY);

        // Will be used later to calculate the length of the ray
        double perpWallDist;

        int stepX;
        int stepY;

        // Used to determinate whether or not the coming loop may be ended (was there a wall hit?)
        int hit = 0;
        // Will contain if an x-side or a y-side of a wall was hit
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

        dda(&hit, &mapX, &mapY, &stepX, &stepY, &sideDistX, &sideDistY, &deltaDistX, &deltaDistY, &side);

        // Calculate distance projected on camera direction. We don't use the 
        // Euclidean distance to the point representing player, but instead 
        // the distance to the camera plane (or, the distance of the point 
        // projected on the camera direction to the player), to avoid the fisheye effect
        if(side == 0) perpWallDist = (mapX - camera->posX + (1 - stepX) / 2) / rayDirX;
        else          perpWallDist = (mapY - camera->posY + (1 - stepY) / 2) / rayDirY;


        // Calculate the height of the line that has to be drawn on screen
        int lineHeight = (int)(SCREEN_H / perpWallDist);
        
        // TODO: add draw_wall() function
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

        // Set the Z buffer (depth) for sprite casting
        z_buffer[x] = perpWallDist; //perpendicular distance is used
    }
}

void dda(int* hit, int* mapX, int* mapY, int* stepX, int* stepY, double* sideDistX, double* sideDistY, double* deltaDistX, double* deltaDistY, int* side){
    // A loop that increments the ray with one square every time, until a wall is hit 
    while (*hit == 0) { // Perform DDA
        // Either jumps a square in the x-direction (with stepX) or a square in the 
        // y-direction (with stepY), it always jumps 1 square at once.
        if(*sideDistX < *sideDistY) {
            *sideDistX += *deltaDistX;
            *mapX += *stepX;
            *side = 0;
        }
        else {
            *sideDistY += *deltaDistY;
            *mapY += *stepY;
            *side = 1;
        }
        // Check if the ray has hit a wall (current map square is not empty)
        if(worldMap[*mapX][*mapY] > 0) *hit = 1;
    }
}

void sprite_raycaster(camera_t* camera){
    for(int i = 0; i < numSprites; i++) {
        // Translate sprite position to relative to camera
        double spriteX = sprites_data[sprite_order[i]].x - camera->posX;
        double spriteY = sprites_data[sprite_order[i]].y - camera->posY;

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
            //4) z_buffer, with perpendicular distance
            if(transformY > 0 && stripe > 0 && stripe < SCREEN_W && transformY < z_buffer[stripe])
            for(int y = drawStartY; y < drawEndY; y++){ //for every pixel of the current stripe  
                int d = (y) * 256 - SCREEN_H * 128 + spriteHeight * 128; //256 and 128 factors to avoid floats
                int texY = ((d * texHeight) / spriteHeight) / 256;
                uint32_t src_idx = texY*(texWidth/8) + texX/8;
                uint32_t dst_idx = y*SCREEN_W + stripe;
                uint32_t color = sprites[sprites_data[sprite_order[i]].texture][src_idx] >> (texX%8)*4 & 0xF; //get current color from the texture
                if(color != 0xd) 
                    unpack_idx4_p32[dst_idx] = color; //paint pixel if it isn't white, white is the invisible color
            }
        }
    }
}
