#ifndef ENGINE_H
#define ENGINE_H

#define USE_DOUBLE_BUFFER // Wether to use seperate buffer to store rendered data and only then display it
#define SECONDS_PER_FRAME FP32F(1.0/60.0)

// Double buffer: renderer draws to this buffer, and then on vSync copies it to the screen
// Although this may reduce framerate a bit, it can save us from displaying unfinished renders.
// So we can use 'buffer' in code, while actually having control over what exact buffer is used

#include <stdint.h>
#include "device.h"
#include "fixedpoint.h"

#ifdef USE_DOUBLE_BUFFER
	extern uint32_t buffer[SCREEN_W * SCREEN_H];
#else
    #define buffer unpack_idx4_p32
#endif

// Sprite objects in-game
typedef struct
{
	fp32_t x;
	fp32_t y;
	uint8_t visible; 
    unsigned anim_index;
	const uint8_t* textures;
	uint8_t raycast_transparent;
} sprite_t;

typedef struct {
	fp32_t pos_x;
	fp32_t pos_y;
	fp32_t dir_x;
	fp32_t dir_y;
	fp32_t plane_x;
	fp32_t plane_y;
	uint8_t hp;
	uint8_t damage;
	uint8_t ammo;
	uint32_t score;
	uint32_t shoot_frequency;
	unsigned long long int last_shot;
	sprite_t* target_sprite;
	uint8_t target_valid;
} player_t;

typedef struct
{
	sprite_t* sprite;			// Sprite info
	fp32_t dist_to_player;		// Distance to keep from player
	fp32_t max_shot_distance;	// Max distance from which to shoot
	fp32_t view_distance;		// When can this enemy see the player
	uint8_t hp;
	uint8_t damage;
	fp32_t speed;
	int shoot_interval;
} enemy_t;

// Time management
extern unsigned long long int frame_count;
float time();	// Returns current time in seconds since game started

#endif