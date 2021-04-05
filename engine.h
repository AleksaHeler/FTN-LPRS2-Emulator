#ifndef ENGINE_H
#define ENGINE_H

typedef struct {
	double posX;
	double posY;
	double dirX;
	double dirY;
	double planeX;
	double planeY;
} camera_t;

typedef struct
{
  double x;
  double y;
  int texture;
} sprite_t;

#endif