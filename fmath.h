#ifndef FMATH_H
#define FMATH_H

#include <stdint.h>

//#define USE_MATH_LIB	// Use math lib and floating point (or fixed point and our sin/cos)
#ifdef USE_MATH_LIB		// Use C math library
	#include <math.h>
	#define my_sin sin
	#define my_cos cos
#endif // USE_MATH_LIB

// CORDIC algorithm constants
#define cordic_1K 0x26DD3B6A
#define half_pi 0x6487ED51
#define MUL 1073741824.000000
#define CORDIC_NTAB 32
#define M_PI 3.1415926535897932384626

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

#ifndef USE_MATH_LIB
	void cordic(int, int*, int*, int);
	double my_sin(double);
	double my_cos(double);
#endif

#endif // FMATH_H