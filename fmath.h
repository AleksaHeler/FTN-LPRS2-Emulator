#ifndef FMATH_H
#define FMATH_H

#include <stdint.h>

// Absolute value maco
#define ABS(N) ((N<0)?(-N):(N))

// Divide with rounding down
static inline uint32_t shift_div_with_round_down(uint32_t num, uint32_t shift){
	uint32_t d = num >> shift;
	return d;
}

// Divide with rounding up
static inline uint32_t shift_div_with_round_up(uint32_t num, uint32_t shift){
	uint32_t d = num >> shift;
	uint32_t mask = (1<<shift)-1;
	if((num & mask) != 0){
		d++;
	}
	return d;
}


#endif