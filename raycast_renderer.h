#ifndef RENDERER_H
#define RENDERER_H

#include "game_data.h"
#include "engine.h"
#include "fmath.h"
#include "sprites_renderer.h"

///////////////////////////////////////////////////////////////////////////////
// Settings / parameters:
#define SPRITE_MIN_RENDER_DISTANCE FP32F(0.2)

///////////////////////////////////////////////////////////////////////////////
// Useful macros:
#define WAIT_UNITL_0(x) while(x != 0){}
#define WAIT_UNITL_1(x) while(x != 1){}

///////////////////////////////////////////////////////////////////////////////
// Sprite list for drawing:
extern sprite_t* renderer_sprites;
extern unsigned sprites_num;

///////////////////////////////////////////////////////////////////////////////
// Functions:
void renderer_init(sprite_t* s, unsigned n);
void renderer_render(player_t* camera);

void renderer_menu();
void renderer_game_over(int);

void wait_for_vsync();      // Wait for the vSync pulse signal
void cls();                 // Clear screen to color with index 0
void floor_raycaster();     // Render floors and ceiling
void wall_raycaster();      // Render walls
void dda();                 // DDA algorithm implementation
void sprite_raycaster();    // Render sprites

void transfer_buffer();     // Copy from double buffer to original one for displaying

#endif // RENDERER_H