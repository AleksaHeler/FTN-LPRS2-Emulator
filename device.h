#ifndef DEVICE_H
#define DEVICE_H

#include <stdint.h>
#include "system.h"

///////////////////////////////////////////////////////////////////////////////
// HW stuff:
#define SCREEN_IDX1_W 640		// Screen resolutions for different kinds of indexing types
#define SCREEN_IDX1_H 480
#define SCREEN_IDX4_W 320
#define SCREEN_IDX4_H 240
#define SCREEN_RGB333_W 160
#define SCREEN_RGB333_H 120

#define SCREEN_IDX4_W8 (SCREEN_IDX4_W/8) // constant: screen width / 8

#define gpu_p32 ((volatile uint32_t*)LPRS2_GPU_BASE)						// Indexing bases
#define palette_p32 ((volatile uint32_t*)(LPRS2_GPU_BASE+0x1000))
#define unpack_idx1_p32 ((volatile uint32_t*)(LPRS2_GPU_BASE+0x400000))
#define pack_idx1_p32 ((volatile uint32_t*)(LPRS2_GPU_BASE+0x600000))
#define unpack_idx4_p32 ((volatile uint32_t*)(LPRS2_GPU_BASE+0x800000))
#define pack_idx4_p32 ((volatile uint32_t*)(LPRS2_GPU_BASE+0xa00000))
#define unpack_rgb333_p32 ((volatile uint32_t*)(LPRS2_GPU_BASE+0xc00000))
#define joypad_p32 ((volatile uint32_t*)LPRS2_JOYPAD_BASE)


///////////////////////////////////////////////////////////////////////////////
// Game config:
// TODO: parametrize stuff
#define INDEX_MODE 2 				// IDX4 - 4b color
#define SCREEN_W SCREEN_IDX4_W		// Currently used screen size
#define SCREEN_H SCREEN_IDX4_H
#define USE_PACKED 0 				// Use unpacked indexing
#define USE_DOUBLE_BUFFER			// Wether to use seperate buffer to store rendered data and only then display it
//#define DEBUG						// Enable some debuging features

// Double buffer: renderer draws to this buffer, and then on vSync copies it to the screen
// Although this may reduce framerate a bit, it can save us from displaying unfinished renders.
// So we can use 'buffer' in code, while actually having control over what exact buffer is used
#ifdef USE_DOUBLE_BUFFER
	volatile uint32_t buffer[SCREEN_W*SCREEN_H];
#else
	#define buffer unpack_idx4_p32		
#endif

///////////////////////////////////////////////////////////////////////////////
// Indexing structs:
typedef struct {
	unsigned a      : 1;	// Keyboard: A
	unsigned b      : 1;	// Keyboard: B
	unsigned z      : 1;	// Keyboard: Z
	unsigned start  : 1;	// Keyboard: S
	unsigned up     : 1;	// Up arrow
	unsigned down   : 1;	// Down arrow
	unsigned left   : 1;	// Left arrow
	unsigned right  : 1;	// Right arrow
} bf_joypad;
#define joypad (*((volatile bf_joypad*)LPRS2_JOYPAD_BASE))

typedef struct {
	uint32_t m[SCREEN_H][SCREEN_W];
} bf_unpack_idx1;
#define unpack_idx1 (*((volatile bf_unpack_idx1*)unpack_idx1_p32))

#endif // DEVICE_H