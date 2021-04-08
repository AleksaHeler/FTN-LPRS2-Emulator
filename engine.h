#ifndef ENGINE_H
#define ENGINE_H

#ifdef DEBUG
	#include <stdio.h>	// For testing only: todo remove in final version
	#include <time.h>
	#define MAX_FPS 60	// Actual FPS may be lower, but not higher
#endif

typedef struct {
	double pos_x;
	double pos_y;
	double dir_x;
	double dir_y;
	double plane_x;
	double plane_y;

    #ifdef DEBUG
		double time;    // time of current frame
		double oldTime; // time of previous frame
	#endif
} camera_t;

typedef struct
{
  double x;
  double y;
  int texture;
} sprite_t;

#endif