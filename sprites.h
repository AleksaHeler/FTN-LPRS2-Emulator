#ifndef SPRITES_H
#define SPRITES_H

#include "device.h"
#include "game_data.h"
#include "engine.h"
#include "fmath.h"

///////////////////////////////////////////////////////////////////////////////
// Functions:
void draw_sprite(uint32_t*, uint16_t, uint16_t, uint16_t, uint16_t);
void swap(int*, double*, int*, double*);
int partition(int*, double*, int, int);
void quick_sort(int*, double*, int, int);
void sort_sprites(int*, double*, int);


#endif // SPRITES_H