#ifndef RENDERER_H
#define RENDERER_H

// TODO:
//   move sprite functions to sprites.h
//   extract all functionality from renderer_render() to other functions
//   make all those functions use a double of screen buffer
//   function render will only copy to screen buffer

#include "game_data.h"
#include "engine.h"
#include "fmath.h"
#include "sprites_renderer.h"

///////////////////////////////////////////////////////////////////////////////
// Useful macros:
#define WAIT_UNITL_0(x) while(x != 0){}
#define WAIT_UNITL_1(x) while(x != 1){}


///////////////////////////////////////////////////////////////////////////////
// Functions:
void renderer_init();
void renderer_render(camera_t* camera);

void wait_for_vsync();      // Wait for the vSync pulse signal
void cls();                 // Clear screen to color with index 0
void floor_raycaster();     // Render floors and ceiling
void wall_raycaster();      // Render walls
void dda();                 // DDA algorithm implementation
void sprite_raycaster();    // Render sprites

void transfer_buffer();     // Copy from double buffer to original one for displaying

#endif // RENDERER_H