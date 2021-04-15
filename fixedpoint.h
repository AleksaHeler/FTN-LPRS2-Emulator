#ifndef FIXEDPOINT_H
#define FIXEDPOINT_H

#include <stdint.h>

#define FP32_DECIMAL_PLACES 16

typedef int32_t fp32_t;

static const fp32_t fp32_one = 1 << FP32_DECIMAL_PLACES;
static const fp32_t fp32_maximum = 0x7FFFFFFF;
static const fp32_t fp32_minimum = 0x80000000;
//static const fp32_t fp32_pi = 205887;
//static const fp32_t fp32_e = 178145;

// Only use for constants, works with 16 dec
#define FP32F(x) ((fp32_t)(((x) >= 0) ? ((x)*65536.0 + 0.5) : ((x)*65536.0 - 0.5)))
#define FP32(x) ((fp32_t)(x << FP32_DECIMAL_PLACES))

// Use following functions for runtime conversion
static inline fp32_t fp32_from_int(int a) { return a << FP32_DECIMAL_PLACES; }
static inline int fp32_to_int(fp32_t a) { return a >> FP32_DECIMAL_PLACES; }
static inline int fp32_to_int_round(fp32_t a) {
    if (a >= 0) return (a + (fp32_one >> 1)) >> FP32_DECIMAL_PLACES;
    return (a - (fp32_one >> 1)) >> FP32_DECIMAL_PLACES;
}

static inline fp32_t fp32_from_float(float a) { return a * fp32_one; }
static inline fp32_t fp32_from_float_round(float a) {
    float temp = a * fp32_one;
    temp += (temp >= 0) ? 0.5f : -0.5f;
    return (fp32_t)temp;
}
static inline float fp32_to_float(fp32_t a) { return (float)a / fp32_one; }

// Useful functions
static inline fp32_t fp32_frac(fp32_t x) { return x & (fp32_one - 1); } // Return only the fraction part
static inline fp32_t fp32_abs(fp32_t x) { return (x < 0 ? -x : x); }
static inline fp32_t fp32_floor(fp32_t x) { return x & 0xFFFF0000; }
static inline fp32_t fp32_ceil(fp32_t x) { return (x & 0xFFFF0000) + (x & 0x0000FFFF ? fp32_one : 0); }
static inline fp32_t fp32_min(fp32_t x, fp32_t y) { return (x < y ? x : y); }
static inline fp32_t fp32_max(fp32_t x, fp32_t y) { return (x > y ? x : y); }
static inline fp32_t fp32_clamp(fp32_t x, fp32_t lo, fp32_t hi) { return fp32_min(fp32_max(x, lo), hi); }

// Arithmetic operations
static inline fp32_t fp32_add(fp32_t x, fp32_t y) { return (x + y); }
static inline fp32_t fp32_subtract(fp32_t x, fp32_t y) { return (x - y); }
// TODO: optimize these further
static inline fp32_t fp32_mul(fp32_t x, fp32_t y) { return ((int64_t)x * y) >> FP32_DECIMAL_PLACES; }
static inline fp32_t fp32_div(fp32_t x, fp32_t y) { return ((int64_t)x << FP32_DECIMAL_PLACES) / y; }

#endif