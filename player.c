#include "player.h"

camera_t player_camera;

void player_init() {
    player_camera.pos_x = FP32F(22.5), player_camera.pos_y = FP32F(12.5); // player x and y start position
    player_camera.dir_x = FP32(-1), player_camera.dir_y = FP32(0);       // initial player direction vector

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
    
    fp32_t move_speed = FP32F(5.0/60.0);
    fp32_t rotation_speed = FP32F(3.0/60.0);

    fp32_t player_width = FP32F(0.4);

    // Temp variables for better code readability
    fp32_t pos_x = player_camera.pos_x;
    fp32_t pos_y = player_camera.pos_y;
    fp32_t dir_x = player_camera.dir_x;
    fp32_t dir_y = player_camera.dir_y;
    fp32_t plane_x = player_camera.plane_x;
    fp32_t plane_y = player_camera.plane_y;

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

    // Temp variables for better code readability
    player_camera.pos_x = pos_x;
    player_camera.pos_y = pos_y;
    player_camera.dir_x = dir_x;
    player_camera.dir_y = dir_y;
    player_camera.plane_x = plane_x;
    player_camera.plane_y = plane_y;
}