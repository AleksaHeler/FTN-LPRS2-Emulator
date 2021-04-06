#include "player.h"

camera_t playerCamera;

void player_init() {
    playerCamera.posX = 22, playerCamera.posY = 12;  // player x and y start position
    playerCamera.dirX = -1, playerCamera.dirY = 0; // initial player direction vector

    // Camera plane is perpendicular to the direction, but we 
    //  can change the length of it. The ratio between the length
    //  of the direction and the camera plane determinates the FOV.
    //  FOV is 2 * atan(0.66/1.0)=66°, which is perfect for a first person shooter game
    playerCamera.planeX = 0, playerCamera.planeY = 0.66; // the 2d raycaster version of camera plane
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
    double moveSpeed = 5.0/60.0;
    double rotSpeed = 3.0/60.0;
    
    // Move forward if no wall in front of the player
    if(joypad.up) {
        if(worldMap[(int)(playerCamera.posX + playerCamera.dirX * moveSpeed)][(int)playerCamera.posY] == 0) 
            playerCamera.posX += playerCamera.dirX * moveSpeed;
        if(worldMap[(int)(playerCamera.posX)][(int)(playerCamera.posY + playerCamera.dirY * moveSpeed)] == 0) 
            playerCamera.posY += playerCamera.dirY * moveSpeed;
    }
    // Move backwards if no wall behind the player
    if(joypad.down) {
        if(worldMap[(int)(playerCamera.posX - playerCamera.dirX * moveSpeed)][(int)playerCamera.posY] == 0) playerCamera.posX -= playerCamera.dirX * moveSpeed;
        if(worldMap[(int)(playerCamera.posX)][(int)(playerCamera.posY - playerCamera.dirY * moveSpeed)] == 0) playerCamera.posY -= playerCamera.dirY * moveSpeed;
    }

    // Rotate to the right
    if(joypad.right) {
        // Both camera direction and camera plane must be rotated
        // Rotating vectors by multiplying it with rotation matrix:
        //   [ cos(a) -sin(a) ]
        //   [ sin(a)  cos(a) ]
        double oldDirX = playerCamera.dirX;
        playerCamera.dirX = playerCamera.dirX * my_cos(-rotSpeed) - playerCamera.dirY * my_sin(-rotSpeed);
        playerCamera.dirY = oldDirX * my_sin(-rotSpeed) + playerCamera.dirY * my_cos(-rotSpeed);
        double oldPlaneX = playerCamera.planeX;
        playerCamera.planeX = playerCamera.planeX * my_cos(-rotSpeed) - playerCamera.planeY * my_sin(-rotSpeed);
        playerCamera.planeY = oldPlaneX * my_sin(-rotSpeed) + playerCamera.planeY * my_cos(-rotSpeed);
    }
    // Rotate to the left
    if(joypad.left) {
        // Both camera direction and camera plane must be rotated
        double oldDirX = playerCamera.dirX;
        playerCamera.dirX = playerCamera.dirX * my_cos(rotSpeed) - playerCamera.dirY * my_sin(rotSpeed);
        playerCamera.dirY = oldDirX * my_sin(rotSpeed) + playerCamera.dirY * my_cos(rotSpeed);
        double oldPlaneX = playerCamera.planeX;
        playerCamera.planeX = playerCamera.planeX * my_cos(rotSpeed) - playerCamera.planeY * my_sin(rotSpeed);
        playerCamera.planeY = oldPlaneX * my_sin(rotSpeed) + playerCamera.planeY * my_cos(rotSpeed);
    }
}