#include "fmath.h"

#ifndef USE_MATH_LIB

// Using FP32_DECIMAL_PLACES + 2 bits
//Cordic in 18 bit signed fixed point math
//Function is valid for arguments in range -pi/2 -- pi/2
//for values pi/2--pi: value = half_pi-(theta-half_pi) and similarly for values -pi---pi/2
//
// 1.0 = 65536
// 1/k = 0.6072529350088812561694
// pi = 3.1415926536897932384626
//Constants
#define cordic_1K 0x00009B74
#define half_pi 0x0001921F
#define CORDIC_NTAB 18
int cordic_ctab[] = {
    0x0000C90F,
    0x000076B1,
    0x00003EB6,
    0x00001FD5,
    0x00000FFA,
    0x000007FF,
    0x000003FF,
    0x000001FF,
    0x000000FF,
    0x0000007F,
    0x0000003F,
    0x0000001F,
    0x0000000F,
    0x00000007,
    0x00000003,
    0x00000001,
    0x00000000,
    0x00000000,
};

void _cordic(int theta, int *s, int *c, int n) {
    int k, d, tx, ty, tz;
    int x = cordic_1K, y = 0, z = theta;
    n = (n > CORDIC_NTAB) ? CORDIC_NTAB : n;
    for (k = 0; k < n; ++k) {
        d = (z >= 0) ? 0 : -1;
        tx = x - (((y >> k) ^ d) - d);
        ty = y + (((x >> k) ^ d) - d);
        tz = z - ((cordic_ctab[k] ^ d) - d);
        x = tx;
        y = ty;
        z = tz;
    }
    *c = x;
    *s = y;
}

void cordic(fp32_t a, fp32_t *s, fp32_t *c) {
    // Translate all input angles to range -pi/2 -- pi/2
    const fp32_t pi2 = fp32_pi * 2;
    while (a < -fp32_pi) a += pi2;
    while (a > fp32_pi) a -= pi2;
    
    if(a < -half_pi) { // in range -pi to -pi/2, then sin(a) = sin(pi-a)
        a = -fp32_pi - a;
    } else if (a > half_pi) {
        a = fp32_pi - a;
    }
    //use max iterations
    _cordic(a, s, c, CORDIC_NTAB);
    return;
}

fp32_t my_sin(fp32_t a) { // a is in radians
    int s, c;
    cordic(a, &s, &c);
    return s;
}

fp32_t my_cos(fp32_t a){
    int s, c;
    cordic(a, &s, &c);
    return c;
}

#endif
