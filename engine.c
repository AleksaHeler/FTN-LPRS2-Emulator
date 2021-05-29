#include "engine.h"

#ifdef USE_DOUBLE_BUFFER
    uint32_t buffer[SCREEN_W * SCREEN_H];
#endif

// Time management
unsigned long long int frame_count = 0;

float time(){
    return (float)frame_count * SECONDS_PER_FRAME;
}