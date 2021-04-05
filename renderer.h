#ifndef RENDERER_H
#define RENDERER_H

#include "device.h"
#include "game_data.h"
#include "engine.h"
#include "fmath.h"

///////////////////////////////////////////////////////////////////////////////
// Useful macros:
#define WAIT_UNITL_0(x) while(x != 0){}
#define WAIT_UNITL_1(x) while(x != 1){}


///////////////////////////////////////////////////////////////////////////////
// Functions:
void sort_sprites(int* order, double* dist, int amount);
void draw_sprite(uint32_t* src_p, uint16_t src_w, uint16_t src_h, uint16_t dst_x, uint16_t dst_y);
void renderer_init();
void renderer_render(camera_t* camera);


#endif