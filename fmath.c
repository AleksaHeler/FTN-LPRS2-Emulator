#include "fmath.h"

#ifndef USE_MATH_LIB



// CORDIC algorithm constants
//Cordic in 32 bit signed fixed point math
//Function is valid for arguments in range -pi/2 -- pi/2
//for values pi/2--pi: value = half_pi-(theta-half_pi) and similarly for values -pi---pi/2
void cordic(int theta, int *s, int *c, int n)
{
    int cordic_ctab [] = {0x3243F6A8, 0x1DAC6705, 0x0FADBAFC, 0x07F56EA6, 0x03FEAB76, 0x01FFD55B, 0x00FFFAAA, 0x007FFF55, 0x003FFFEA, 0x001FFFFD, 0x000FFFFF, 0x0007FFFF, 0x0003FFFF, 0x0001FFFF, 0x0000FFFF, 0x00007FFF, 0x00003FFF, 0x00001FFF, 0x00000FFF, 0x000007FF, 0x000003FF, 0x000001FF, 0x000000FF, 0x0000007F, 0x0000003F, 0x0000001F, 0x0000000F, 0x00000008, 0x00000004, 0x00000002, 0x00000001, 0x00000000, };
    int k, d, tx, ty, tz;
    int x=cordic_1K,y=0,z=theta;
    n = (n>CORDIC_NTAB) ? CORDIC_NTAB : n;
    for (k=0; k<n; ++k)
    {
        d = z>>31;
        //get sign. for other architectures, you might want to use the more portable version
        //d = z>=0 ? 0 : -1;
        tx = x - (((y>>k) ^ d) - d);
        ty = y + (((x>>k) ^ d) - d);
        tz = z - ((cordic_ctab[k] ^ d) - d);
        x = tx; y = ty; z = tz;
    }  
    *c = x; *s = y;
}

double my_sin(double a) { // a is in radians
    // Translate all input angles to range -pi/2 -- pi/2
    if(a < -M_PI/2){
        a = a - ((int)(a/M_PI)) * M_PI; // if less than -PI it will clip it
        if(a < -M_PI/2){ // if still less than, which means in range -pi to -pi/2, then sin(a) = sin(pi-a)
            a = -M_PI - a;
        }
    }
    else if (a > M_PI/2){
        a = a - ((int)(a/M_PI)) * M_PI; // if greater than PI it will clip it below
        if(a > M_PI/2){ // if still greater, which means in range pi/2 to pi, then sin(a) = sin(pi-a)
            a = M_PI - a;
        }
    }
    int s,c;
    //use 32 iterations
    cordic((a*MUL), &s, &c, 32);
    return s / MUL;
}

double my_cos(double a){
    // cos(2*b) = 1 - 2sin^2(b)
    double b = a/2.0;
    double s = my_sin(b);
    return (1 - 2*s*s);
}

double floor(double x){
    int i0,i1,j0;
    unsigned i,j;
    i0 =  __HI(x);
    i1 =  __LO(x);
    j0 = ((i0>>20)&0x7ff)-0x3ff;
    if(j0<20) {
        if(j0<0) { 	/* raise inexact if x != 0 */
        if(huge+x>0.0) {/* return 0*sign(x) if |x|<1 */
            if(i0>=0) {i0=i1=0;} 
            else if(((i0&0x7fffffff)|i1)!=0)
            { i0=0xbff00000;i1=0;}
        }
        } else {
        i = (0x000fffff)>>j0;
        if(((i0&i)|i1)==0) return x; /* x is integral */
        if(huge+x>0.0) {	/* raise inexact flag */
            if(i0<0) i0 += (0x00100000)>>j0;
            i0 &= (~i); i1=0;
        }
        }
    } else if (j0>51) {
        if(j0==0x400) return x+x;	/* inf or NaN */
        else return x;		/* x is integral */
    } else {
        i = ((unsigned)(0xffffffff))>>(j0-20);
        if((i1&i)==0) return x;	/* x is integral */
        if(huge+x>0.0) { 		/* raise inexact flag */
        if(i0<0) {
            if(j0==20) i0+=1; 
            else {
            j = i1+(1<<(52-j0));
            if(j<i1) i0 +=1 ; 	/* got a carry */
            i1=j;
            }
        }
        i1 &= (~i);
        }
    }
    __HI(x) = i0;
    __LO(x) = i1;
    return x;
}

#endif
