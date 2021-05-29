#include "player.h"

camera_t player_camera;

void player_init() {
    player_camera.pos_x = FP32F(22.5), player_camera.pos_y = FP32F(12.5); // player x and y start position
    player_camera.dir_x = FP32(-1), player_camera.dir_y = FP32(0);       // initial player direction vector

#ifdef DEBUG
        player_camera.time = 0;
        player_camera.oldTime = 0;
    #endif

    // Camera plane is perpendicular to the direction, but we 
    //  can change the length of it. The ratio between the length
    //  of the direction and the camera plane determinates the FOV.
    //  FOV is 2 * atan(0.66/1.0)=66°, which is perfect for a first person shooter game
    player_camera.plane_x = FP32F(0), player_camera.plane_y = FP32F(0.66); // the 2d raycaster version of camera plane
}

// Registering inputs in main menu
int player_menu() {
    if(joypad.a) return 1;
    if(joypad.b) return -1;
    return 0;
}

void player_update() {
    /////////////////////////////////////
    // Poll controls:
    // The speed modifiers use frameTime, and a constant value, to 
    // determinate the speed of the moving and rotating of the input 
    // keys. Thanks to using the frameTime, we can make sure that the 
    // moving and rotating speed is independent of the processor speed
    //double moveSpeed = frameTime * 5.0; //the constant value is in squares/second
    //double rotSpeed = frameTime * 3.0;  //the constant value is in radians/second
    // TODO: make speeds relate to time and not FPS
    
    #ifdef DEBUG
        player_camera.oldTime = player_camera.time;
        player_camera.time = clock();

        double frameTime = (player_camera.time - player_camera.oldTime) / 1000.0; //frametime is the time this frame has taken, in milliseconds

        /* Speed modifiers */
        double move_speed = frameTime * 2 * 0.001; //the constant value is in squares/second
        double rotation_speed = frameTime * 1.2 * 0.001; //the constant value is in radians/second
    #else
        fp32_t move_speed = FP32F(5.0/60.0);
        fp32_t rotation_speed = FP32F(3.0/60.0);
    #endif

    fp32_t player_width = FP32F(0.4);

    // TODO temp
    fp32_t pos_x = player_camera.pos_x;
    fp32_t pos_y = player_camera.pos_y;
    fp32_t dir_x = player_camera.dir_x;
    fp32_t dir_y = player_camera.dir_y;
    fp32_t plane_x = player_camera.plane_x;
    fp32_t plane_y = player_camera.plane_y;

    // TODO Optimize these pls

    // Move forward if no wall in front of the player
    if(joypad.up) {
        if(world_map[fp32_to_int(pos_x + fp32_mul(dir_x, move_speed) + fp32_mul(dir_x, player_width))][fp32_to_int(pos_y)] == 0) 
            pos_x += fp32_mul(dir_x, move_speed);
        if(world_map[fp32_to_int(pos_x)][fp32_to_int(pos_y + fp32_mul(dir_y, move_speed) + fp32_mul(dir_y, player_width))] == 0) 
            pos_y += fp32_mul(dir_y, move_speed);
    }
    // Move backwards if no wall behind the player
    if(joypad.down) {
        if(world_map[fp32_to_int(pos_x - fp32_mul(dir_x, move_speed) - fp32_mul(dir_x, player_width))][fp32_to_int(pos_y)] == 0) 
            pos_x -= fp32_mul(dir_x, move_speed);
        if(world_map[fp32_to_int(pos_x)][fp32_to_int(pos_y - fp32_mul(dir_y, move_speed) - fp32_mul(dir_y, player_width))] == 0) 
            pos_y -= fp32_mul(dir_y, move_speed);
    }

    // Rotate to the right
    if(joypad.right) {
        // Both camera direction and camera plane must be rotated
        // Rotating vectors by multiplying it with rotation matrix:
        //   [ cos(a) -sin(a) ]
        //   [ sin(a)  cos(a) ]
        fp32_t old_dir_x = dir_x;
        dir_x = fp32_mul(dir_x, my_cos(-rotation_speed)) - fp32_mul(dir_y, my_sin(-rotation_speed));
        dir_y = fp32_mul(old_dir_x, my_sin(-rotation_speed)) + fp32_mul(dir_y, my_cos(-rotation_speed));
        fp32_t old_plane_x = plane_x;
        plane_x = fp32_mul(plane_x, my_cos(-rotation_speed)) - fp32_mul(plane_y, my_sin(-rotation_speed));
        plane_y = fp32_mul(old_plane_x, my_sin(-rotation_speed)) + fp32_mul(plane_y, my_cos(-rotation_speed));
    }
    // Rotate to the left
    if(joypad.left) {
        // Both camera direction and camera plane must be rotated
        fp32_t old_dir_x = dir_x;
        dir_x = fp32_mul(dir_x, my_cos(rotation_speed)) - fp32_mul(dir_y, my_sin(rotation_speed));
        dir_y = fp32_mul(old_dir_x, my_sin(rotation_speed)) + fp32_mul(dir_y, my_cos(rotation_speed));
        fp32_t old_plane_x = plane_x;
        plane_x = fp32_mul(plane_x, my_cos(rotation_speed)) - fp32_mul(plane_y, my_sin(rotation_speed));
        plane_y = fp32_mul(old_plane_x, my_sin(rotation_speed)) + fp32_mul(plane_y, my_cos(rotation_speed));
    }

    // Animate enemies (maybe move to another place?)
    for(int i = 0; i < num_enemies; i++){
        // enemies_data[i].x, enemies_data[i].x
        // Calculate distance to player
        // Check if there is a wall in front
        // If there is no wall
        //   if distance is in some range (a to b) -> move towards player until distance is 'a' 
        //   if distance is too close (less than 'a') -> move backwards if there is no wall there
        //   if distance to player is in shooting range (a to b) -> shoot at player on regular interval
        
        // Example of moving in diagonal line each frame: 
        //enemies_data[i].x += FP32F(0.005);
        //enemies_data[i].y += FP32F(0.005);
    }

    // TODO temp
    player_camera.pos_x = pos_x;
    player_camera.pos_y = pos_y;
    player_camera.dir_x = dir_x;
    player_camera.dir_y = dir_y;
    player_camera.plane_x = plane_x;
    player_camera.plane_y = plane_y;
}