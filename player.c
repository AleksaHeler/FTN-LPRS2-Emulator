#include "player.h"
#include "stdio.h" // TODO: remove when done

player_t player;

void player_init() {
    player.pos_x = FP32F(22.5), player.pos_y = FP32F(12.5); // player x and y start position
    player.dir_x = FP32(-1), player.dir_y = FP32(0);       // initial player direction vector

    // Camera plane is perpendicular to the direction, but we 
    //  can change the length of it. The ratio between the length
    //  of the direction and the camera plane determinates the FOV.
    //  FOV is 2 * atan(0.66/1.0)=66°, which is perfect for a first person shooter game
    player.plane_x = FP32F(0), player.plane_y = FP32F(0.66); // the 2d raycaster version of camera plane

    player.hp = FP32(100);
    player.damage = FP32(50);
}

// Registering inputs in main menu
int player_menu() {
    if(joypad.start)    return 1;
    if(joypad.a)        return 1;
    if(joypad.b)        return -1;
    if(joypad.z)        return -1;
    return 0;
}

void player_hit(int damage){
    player.hp -= damage;
    printf("Player hit! [HP=%f]\n", fp32_to_float(player.hp));
}

int player_update() {
    /////////////////////////////////////
    // Poll controls:
    // The speed modifiers use frameTime, and a constant value, to 
    // determinate the speed of the moving and rotating of the input 
    // keys. Thanks to using the frameTime, we can make sure that the 
    // moving and rotating speed is independent of the processor speed
    //double moveSpeed = frameTime * 5.0; //the constant value is in squares/second
    //double rotSpeed = frameTime * 3.0;  //the constant value is in radians/second

    // Check if player is dead
    if(player.hp <= FP32(0))
        return -1;
    
    fp32_t move_speed = fp32_mul(SECONDS_PER_FRAME, FP32F(5.0));
    fp32_t rotation_speed = fp32_mul(SECONDS_PER_FRAME, FP32F(3.0));

    fp32_t player_width = FP32F(0.4);

    // Temp variables for better code readability
    fp32_t pos_x = player.pos_x;
    fp32_t pos_y = player.pos_y;
    fp32_t dir_x = player.dir_x;
    fp32_t dir_y = player.dir_y;
    fp32_t plane_x = player.plane_x;
    fp32_t plane_y = player.plane_y;

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
    player.pos_x = pos_x;
    player.pos_y = pos_y;
    player.dir_x = dir_x;
    player.dir_y = dir_y;
    player.plane_x = plane_x;
    player.plane_y = plane_y;

    return 0;
}