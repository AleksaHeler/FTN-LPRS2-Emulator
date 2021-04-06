#ifndef ENGINE_H
#define ENGINE_H

typedef struct {
	double pos_x;
	double pos_y;
	double dir_x;
	double dir_y;
	double plane_x;
	double plane_y;
} camera_t;

typedef struct
{
  double x;
  double y;
  int texture;
} sprite_t;

#endif