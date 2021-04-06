#ifndef RENDERER_H
#define RENDERER_H

// TODO:
//   move sprite functions to sprites.h
//   extract all functionality from renderer_render() to other functions
//   make all those functions use a double of screen buffer
//   function render will only copy to screen buffer

#include "device.h"
#include "game_data.h"
#include "engine.h"
#include "fmath.h"
#include "sprites.h"

///////////////////////////////////////////////////////////////////////////////
// Useful macros:
#define WAIT_UNITL_0(x) while(x != 0){}
#define WAIT_UNITL_1(x) while(x != 1){}


///////////////////////////////////////////////////////////////////////////////
// Functions:
void renderer_init();
void renderer_render(camera_t* camera);


#endif // RENDERER_H