#include "raycast_renderer.h"

sprite_t *renderer_sprites;
unsigned sprites_num;

fp32_t z_buffer[SCREEN_W];               // 1D 'depth/distance' z_buffer
int standing_sprite_order[SPRITES_MAX_NUM];            // Arrays used to sort the sprites
fp32_t standing_sprite_distance[SPRITES_MAX_NUM];
int enemies_sprite_order[SPRITES_MAX_NUM];            // Arrays used to sort the sprites
fp32_t enemies_sprite_distance[SPRITES_MAX_NUM];


///////////////////////////////////////////////////////////////////////////////
// Initialization and setup of the renderer data. Copies colors from palette in 'sprites_data.c'
void renderer_init(sprite_t *sprites, unsigned sprite_n) {
    gpu_p32[0] = INDEX_MODE;
	gpu_p32[1] = USE_PACKED;

    for(int i = 0; i < 16; i++)         // Copy colors from 'sprites_data.c'
        palette_p32[i] = palette[i];

	gpu_p32[0x800] = 0x0000FF00;        // Green for HUD

    renderer_sprites = sprites;
    sprites_num = sprite_n;
}

void renderer_menu(){
    #ifdef USE_DOUBLE_BUFFER
        wait_for_vsync();                                               // usually: 24-28ms
        transfer_buffer(); // Copy buffer from prev frame to screen     // usually: 0.4 - 4ms
    #else
        wait_for_vsync();
    #endif

    cls();  // Clear background to color with index 0 in palette        // usually: 0.27ms
    // Draw main menu background
    draw_sprite(background__p, background__w, background__h, 0, 0);     // Background
    draw_sprite_transparent(play__p, play__w, play__h, 55, 55);         // Play text
    draw_sprite_transparent(quit__p, quit__w, quit__h, 55, 95);         // Quit text
}

///////////////////////////////////////////////////////////////////////////////
// Rendering: floor&ceiling, walls, sprites
void renderer_render(camera_t* camera) {                                // Time taken on an average pc:
    
    #ifdef USE_DOUBLE_BUFFER
        wait_for_vsync();                                               // usually: 24-28ms
        transfer_buffer(); // Copy buffer from prev frame to screen     // usually: 0.4 - 4ms
    #else
        wait_for_vsync();
    #endif

    cls();  // Clear background to color with index 0 in palette        // usually: 0.27ms
    floor_raycaster(camera);                                            // usually: 1.5 - 5ms
    wall_raycaster(camera);                                             // usually: 1 - 1.4ms
    // TODO: still some work on sorting all sprites at once to remove sorting bugs
    sort_sprites(standing_sprite_order, standing_sprite_distance, camera, SPRITES_MAX_NUM); // usually: 0.006ms
    sort_sprites(enemies_sprite_order, enemies_sprite_distance, camera, num_enemies);
    sprite_raycaster(camera);                                           // usually: 0.65ms
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
        buffer[i] = 0;
    }
}

// Draw floor and ceiling in screen buffer
void floor_raycaster(camera_t* camera){
    // For every horizontal line from middle to the bottom of the screen
    for(int y = SCREEN_H / 2 + 1; y < SCREEN_H; ++y) {
        // rayDir for leftmost ray (x = 0) and rightmost ray (x = w)
        fp32_t ray_dir_x0 = camera->dir_x - camera->plane_x;
        fp32_t ray_dir_y0 = camera->dir_y - camera->plane_y;
        fp32_t ray_dir_x1 = camera->dir_x + camera->plane_x;
        fp32_t ray_dir_y1 = camera->dir_y + camera->plane_y;

        // Current y position compared to the center of the screen (the horizon)
        int p = y - SCREEN_H / 2;

        // Vertical position of the camera (SCREEN_H / 2)
        fp32_t pos_z = FP32(SCREEN_H) >> 1;

        // Horizontal distance from the camera to the floor for the current row
        fp32_t row_distance = pos_z / p;

        // Calculate the real world step vector we have to add for each x (parallel to camera plane)
        // adding step by step avoids multiplications with a weight in the inner loop
        fp32_t floor_step_x = fp32_mul(row_distance, (ray_dir_x1 - ray_dir_x0) / SCREEN_W);
        fp32_t floor_step_y = fp32_mul(row_distance, (ray_dir_y1 - ray_dir_y0) / SCREEN_W);

        // real world coordinates of the leftmost column. This will be updated as we step to the right.
        fp32_t floor_y = camera->pos_y + fp32_mul(row_distance, ray_dir_y0);
        fp32_t floor_x = camera->pos_x + fp32_mul(row_distance, ray_dir_x0);

        for(int x = 0; x < SCREEN_W; ++x){
            // the cell coord is simply derived from the integer parts of floor_x and floor_y
            int cell_x = fp32_to_int(floor_x);
            int cell_y = fp32_to_int(floor_y);

            // get the texture coordinate from the fractional part
            int tx = fp32_frac(floor_x) >> (FP32_DECIMAL_PLACES - tex_width_shift);
            int ty = fp32_frac(floor_y) >> (FP32_DECIMAL_PLACES - tex_height_shift);

            floor_x += floor_step_x;
            floor_y += floor_step_y;

            // choose texture and draw the pixel
            int checker_board_pattern = ((int)(cell_x + cell_y)) & 1;
            int floor_texture;
            if(checker_board_pattern == 0) floor_texture = 3;
            else floor_texture = 4;
            int ceiling_texture = 6;
            uint32_t color;

            // floor
            uint32_t dst_idx = y*SCREEN_W + x;
            uint32_t src_idx = tex_width/8 * ty + tx/8;
            color = textures[floor_texture][src_idx] >> (tx%8)*4;
            buffer[dst_idx] = color;

            //ceiling (symmetrical, at screenHeight - y - 1 instead of y)
            dst_idx = (SCREEN_H-y-1)*SCREEN_W + x;
            src_idx = (tex_width/8) * ty + tx/8;
            color = textures[ceiling_texture][src_idx] >> (tx%8)*4;
            buffer[dst_idx] = color;
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
        fp32_t camera_x = 2 * fp32_from_int(x) / SCREEN_W - FP32(1); //x-coordinate in camera space
        fp32_t ray_dir_x = camera->dir_x + fp32_mul(camera->plane_x, camera_x);
        fp32_t ray_dir_y = camera->dir_y + fp32_mul(camera->plane_y, camera_x);

        // Which box of the map we're in (just rounding to int), basically the index in map matrix
        int map_x = fp32_to_int(camera->pos_x);
        int map_y = fp32_to_int(camera->pos_y);

        // Length of ray from current position to next x or y-side
        fp32_t side_dist_x;
        fp32_t side_dist_y;

        // The distance the ray has to travel to go from 1 x-side 
        // to the next x-side, or from 1 y-side to the next y-side.
        fp32_t delta_dist_x = fp32_abs(fp32_div(FP32(1), ray_dir_x));
        fp32_t delta_dist_y = fp32_abs(fp32_div(FP32(1), ray_dir_y));

        // Will be used later to calculate the length of the ray
        fp32_t perpendicular_wall_distance;

        int step_x;
        int step_y;

        // Used to determinate whether or not the coming loop may be ended (was there a wall hit?)
        int hit = 0;
        // Will contain if an x-side or a y-side of a wall was hit
        int side;

        // Now, before the actual DDA can start, first step_x, step_y, and the 
        // initial side_dist_x and side_dist_y still have to be calculated.
        if(ray_dir_x < 0) {
            step_x = -1;
            side_dist_x = fp32_mul(camera->pos_x - fp32_from_int(map_x), delta_dist_x);
        } else { // ray_dir_x > 0
            step_x = 1;
            side_dist_x = fp32_mul(fp32_from_int(map_x + 1) - camera->pos_x, delta_dist_x);
        }
        if(ray_dir_y < 0) {
            step_y = -1;
            side_dist_y = fp32_mul(camera->pos_y - fp32_from_int(map_y), delta_dist_y);
        } else { // ray_dir_y > 0
            step_y = 1;
            side_dist_y = fp32_mul(fp32_from_int(map_y + 1) - camera->pos_y, delta_dist_y);
        }

        fp32_t distance; // Not used here, but function needs the parameter
        dda(&hit, &map_x, &map_y, &step_x, &step_y, &side_dist_x, &side_dist_y, &delta_dist_x, &delta_dist_y, &side, &distance);

        // Calculate distance projected on camera direction. We don't use the 
        // Euclidean distance to the point representing player, but instead 
        // the distance to the camera plane (or, the distance of the point 
        // projected on the camera direction to the player), to avoid the fisheye effect
        if(side == 0) perpendicular_wall_distance = fp32_div(fp32_from_int(map_x) - camera->pos_x + fp32_from_int(1 - step_x) / 2, ray_dir_x);
        else          perpendicular_wall_distance = fp32_div(fp32_from_int(map_y) - camera->pos_y + fp32_from_int(1 - step_y) / 2, ray_dir_y);


        // Calculate the height of the line that has to be drawn on screen
        fp32_t line_height = fp32_div(FP32(SCREEN_H), perpendicular_wall_distance);
        
        // TODO: add draw_wall() function
        // Calculate lowest and highest pixel to fill in current stripe. 
        // The center of the wall should be at the center of the screen, and 
        // if these points lie outside the screen, they're capped to 0 or h-1
        int draw_start = -fp32_to_int(line_height) / 2 + SCREEN_H / 2;
        if(draw_start < 0) draw_start = 0;
        int draw_end = fp32_to_int(line_height) / 2 + SCREEN_H / 2;
        if(draw_end >= SCREEN_H) draw_end = SCREEN_H;
        
        // Choose wall texture from map
        // Number 1 on map corresponds with texture 0 and so on
        int tex_num = world_map[map_x][map_y] - 1;

        // Calculate value of wall_x 
        fp32_t wall_x; //where exactly the wall was hit
        if(side == 0) wall_x = camera->pos_y + fp32_mul(perpendicular_wall_distance, ray_dir_y);
        else          wall_x = camera->pos_x + fp32_mul(perpendicular_wall_distance, ray_dir_x);
        wall_x = fp32_frac(wall_x);

        // X coordinate on the texture 
        int tex_x = fp32_to_int(wall_x << tex_width_shift); // pos on the wall texture
        if(side == 0 && ray_dir_x > 0) tex_x = tex_width - tex_x - 1;
        if(side == 1 && ray_dir_y < 0) tex_x = tex_width - tex_x - 1;

        // TODO: an integer-only bresenham or DDA like algorithm could make the texture coordinate stepping faster
        // How much to increase the texture coordinate per screen pixel
        fp32_t step = fp32_div(FP32(tex_height), line_height);
        // Starting texture coordinate
        fp32_t tex_pos = fp32_mul(fp32_from_int(draw_start) - FP32(SCREEN_H / 2) + line_height / 2, step);

        // Draw the texture vertical stripe
        for(int y = draw_start; y < draw_end; y++) {
            // Cast the texture coordinate to integer, and mask with (tex_height - 1) in case of overflow
            int tex_y = fp32_to_int(tex_pos) & (tex_height - 1);
            tex_pos += step;

            // Get the indices of source texture pixel and destination in buffer
            uint32_t dst_idx = y*SCREEN_W + x;
            // shift_div_with_round_down
            uint32_t src_idx = tex_y*(shift_div_with_round_up(tex_width, 3)) + shift_div_with_round_down(tex_x, 3);
            if(y == SCREEN_H/2) // Fix bug where lines on horizon were fuzzy
                src_idx = (tex_height/2)*shift_div_with_round_down(tex_width,3) + shift_div_with_round_down(tex_x,3);
            // Get the pixel color and write to buffer
            uint32_t color = textures[tex_num][src_idx] >> (tex_x%8)*4;
            buffer[dst_idx] = color;
        }

        // Set the Z buffer (depth) for sprite casting
        z_buffer[x] = perpendicular_wall_distance; //perpendicular distance is used
    }
}

void dda(int* hit, int* map_x, int* map_y, int* step_x, int* step_y, fp32_t* side_dist_x, fp32_t* side_dist_y, fp32_t* delta_dist_x, fp32_t* delta_dist_y, int* side, fp32_t* dist){
    // A loop that increments the ray with one square every time, until a wall is hit 
    while (*hit == 0) { // Perform DDA
        // Either jumps a square in the x-direction (with step_x) or a square in the 
        // y-direction (with step_y), it always jumps 1 square at once.
        if(*side_dist_x < *side_dist_y) {
            *side_dist_x += *delta_dist_x;
            *map_x += *step_x;
            *side = 0;
            *dist += *delta_dist_x;
        }
        else {
            *side_dist_y += *delta_dist_y;
            *map_y += *step_y;
            *side = 1;
        }
        // Check if the ray has hit a wall (current map square is not empty)
        if(world_map[*map_x][*map_y] > 0) *hit = 1;
    }
}

void sprite_raycaster(camera_t* camera){
    // Draw standing sprites
    for(int i = 0; i < sprites_num; i++) {
        // Translate sprite position to relative to camera
        // TODO convert sprite data to fp32_t
        sprite_t* sprite = &sprites_data[standing_sprite_order[i]];
        fp32_t sprite_x = sprite->x - camera->pos_x;
        fp32_t sprite_y = sprite->y - camera->pos_y;

        //transform sprite with the inverse camera matrix
        // [ planeX   dirX ] -1                                       [ dirY      -dirX ]
        // [               ]       =  1/(planeX*dirY-dirX*planeY) *   [                 ]
        // [ planeY   dirY ]                                          [ -planeY  planeX ]
        fp32_t inv_det = fp32_div(FP32(1), (fp32_mul(camera->plane_x, camera->dir_y) - fp32_mul(camera->dir_x, camera->plane_y))); //required for correct matrix multiplication

        fp32_t transform_x = fp32_mul(inv_det, (fp32_mul(camera->dir_y, sprite_x) - fp32_mul(camera->dir_x, sprite_y)));
        fp32_t transform_y = fp32_mul(inv_det, (-fp32_mul(camera->plane_y, sprite_x) + fp32_mul(camera->plane_x, sprite_y))); //this is actually the depth inside the screen, that what Z is in 3D

        int sprite_screen_x = fp32_to_int(fp32_mul(FP32(SCREEN_W) / 2, FP32(1) + fp32_div(transform_x, transform_y)));

        // TODO overflow happens when calculating sprite_screen_x
        // Don't render sprites close to camera
        if (transform_y < SPRITE_MIN_RENDER_DISTANCE) continue;

        //calculate height of the sprite on screen
        int sprite_height = fp32_to_int(fp32_abs(fp32_div(FP32(SCREEN_H), transform_y))); //using 'transform_y' instead of the real distance prevents fisheye
        //calculate lowest and highest pixel to fill in current stripe
        int draw_start_y = -sprite_height / 2 + SCREEN_H / 2;
        if(draw_start_y < 0) draw_start_y = 0;
        int draw_end_y = sprite_height / 2 + SCREEN_H / 2;
        if(draw_end_y >= SCREEN_H) draw_end_y = SCREEN_H;

        //calculate width of the sprite
        int sprite_width = fp32_to_int(fp32_abs(fp32_div(FP32(SCREEN_H), transform_y)));
        int draw_start_x = -sprite_width / 2 + sprite_screen_x;
        if(draw_start_x < 0) draw_start_x = 0;
        int draw_end_x = sprite_width / 2 + sprite_screen_x;
        if(draw_end_x >= SCREEN_W) draw_end_x = SCREEN_W;

        //loop through every vertical stripe of the sprite on screen
        for(int stripe = draw_start_x; stripe < draw_end_x; stripe++) {
            int tex_x = (int)(256 * (stripe - (-sprite_width / 2 + sprite_screen_x)) * tex_width / sprite_width) / 256;
            //the conditions in the if are:
            //1) it's in front of camera plane so you don't see things behind you
            //2) it's on the screen (left)
            //3) it's on the screen (right)
            //4) z_buffer, with perpendicular distance
            if(transform_y > 0 && stripe >= 0 && stripe < SCREEN_W && transform_y < z_buffer[stripe])
            for(int y = draw_start_y; y < draw_end_y; y++){ //for every pixel of the current stripe  
                int d = (y) * 256 - SCREEN_H * 128 + sprite_height * 128; //256 and 128 factors to avoid floats
                int tex_y = ((d * tex_height) / sprite_height) / 256;
                uint32_t src_idx = tex_y*(tex_width/8) + tex_x/8;
                uint32_t dst_idx = y*SCREEN_W + stripe;
                uint32_t color = sprite_textures[sprite->textures[sprite->anim_index]][src_idx] >> (tex_x%8)*4 & 0xF; //get current color from the texture
                if(color != 0xd) 
                    buffer[dst_idx] = color; //paint pixel if it isn't white, white is the invisible color
            }
        }
    }

    // Draw enemies
    for(int i = 0; i < num_enemies; i++) {
        // Translate sprite position to relative to camera
        // TODO convert sprite data to fp32_t
        fp32_t sprite_x = enemies_data[enemies_sprite_order[i]].x - camera->pos_x;
        fp32_t sprite_y = enemies_data[enemies_sprite_order[i]].y - camera->pos_y;

        //transform sprite with the inverse camera matrix
        // [ planeX   dirX ] -1                                       [ dirY      -dirX ]
        // [               ]       =  1/(planeX*dirY-dirX*planeY) *   [                 ]
        // [ planeY   dirY ]                                          [ -planeY  planeX ]
        fp32_t inv_det = fp32_div(FP32(1), (fp32_mul(camera->plane_x, camera->dir_y) - fp32_mul(camera->dir_x, camera->plane_y))); //required for correct matrix multiplication

        fp32_t transform_x = fp32_mul(inv_det, (fp32_mul(camera->dir_y, sprite_x) - fp32_mul(camera->dir_x, sprite_y)));
        fp32_t transform_y = fp32_mul(inv_det, (-fp32_mul(camera->plane_y, sprite_x) + fp32_mul(camera->plane_x, sprite_y))); //this is actually the depth inside the screen, that what Z is in 3D

        int sprite_screen_x = fp32_to_int(fp32_mul(FP32(SCREEN_W) / 2, FP32(1) + fp32_div(transform_x, transform_y)));

        // TODO overflow happens when calculating sprite_screen_x
        // Don't render sprites close to camera
        if (transform_y < SPRITE_MIN_RENDER_DISTANCE) continue;

        //calculate height of the sprite on screen
        int sprite_height = fp32_to_int(fp32_abs(fp32_div(FP32(SCREEN_H), transform_y))); //using 'transform_y' instead of the real distance prevents fisheye
        //calculate lowest and highest pixel to fill in current stripe
        int draw_start_y = -sprite_height / 2 + SCREEN_H / 2;
        if(draw_start_y < 0) draw_start_y = 0;
        int draw_end_y = sprite_height / 2 + SCREEN_H / 2;
        if(draw_end_y >= SCREEN_H) draw_end_y = SCREEN_H;

        //calculate width of the sprite
        int sprite_width = fp32_to_int(fp32_abs(fp32_div(FP32(SCREEN_H), transform_y)));
        int draw_start_x = -sprite_width / 2 + sprite_screen_x;
        if(draw_start_x < 0) draw_start_x = 0;
        int draw_end_x = sprite_width / 2 + sprite_screen_x;
        if(draw_end_x >= SCREEN_W) draw_end_x = SCREEN_W;

        //loop through every vertical stripe of the sprite on screen
        for(int stripe = draw_start_x; stripe < draw_end_x; stripe++) {
            int tex_x = (int)(256 * (stripe - (-sprite_width / 2 + sprite_screen_x)) * tex_width / sprite_width) / 256;
            //the conditions in the if are:
            //1) it's in front of camera plane so you don't see things behind you
            //2) it's on the screen (left)
            //3) it's on the screen (right)
            //4) z_buffer, with perpendicular distance
            if(transform_y > 0 && stripe >= 0 && stripe < SCREEN_W && transform_y < z_buffer[stripe])
            for(int y = draw_start_y; y < draw_end_y; y++){ //for every pixel of the current stripe  
                int d = (y) * 256 - SCREEN_H * 128 + sprite_height * 128; //256 and 128 factors to avoid floats
                int tex_y = ((d * tex_height) / sprite_height) / 256;
                uint32_t src_idx = tex_y*(tex_width/8) + tex_x/8;
                uint32_t dst_idx = y*SCREEN_W + stripe;
                uint32_t color = sprite_textures[enemies_data[enemies_sprite_order[i]].texture][src_idx] >> (tex_x%8)*4 & 0xF; //get current color from the texture
                if(color != 0xd) 
                    buffer[dst_idx] = color; //paint pixel if it isn't white, white is the invisible color
            }
        }
    }
}

// Currently only works for IDX4 in unpacked mode
void transfer_buffer(){
    uint32_t end = SCREEN_W * SCREEN_H;
    for(uint32_t i = 0; i < end; i++){
        unpack_idx4_p32[i] = buffer[i];
    }
}
