#ifndef SPRITES_RENDERER_H
#define SPRITES_RENDERER_H

#include "game_data.h"
#include "engine.h"
#include "fmath.h"

///////////////////////////////////////////////////////////////////////////////
// Functions:

// Drawing sprite:
void draw_sprite(uint32_t*, uint16_t, uint16_t, uint16_t, uint16_t);
void draw_sprite_transparent(uint32_t*, uint16_t, uint16_t, uint16_t, uint16_t);
void draw_sprite_offset_transparent(uint32_t*, uint32_t, uint32_t, uint32_t, uint32_t, uint16_t, uint16_t);

// Sorting sprites:
void swap(int*, fp32_t*, int*, fp32_t*);
int partition(int*, fp32_t*, int, int);
void quick_sort(int*, fp32_t*, int, int);
void sort_sprites(int*, fp32_t*, player_t*, int);


#endif // SPRITES_H