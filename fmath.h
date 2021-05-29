#ifndef FMATH_H
#define FMATH_H

#include <stdint.h>
#include "fixedpoint.h"

//#define USE_MATH_LIB	// Use math lib and floating point (or fixed point and our sin/cos)
#ifdef USE_MATH_LIB		// Use C math library
	#include <math.h>
	#define my_sin sin
	#define my_cos cos
#endif // USE_MATH_LIB

// Absolute value macro
#define ABS(N) ((N<0)?(-N):(N))

// Functions
void cordic(int angle, int* sin, int* cos);
fp32_t my_sin(fp32_t);
fp32_t my_cos(fp32_t);

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

#endif // FMATH_H