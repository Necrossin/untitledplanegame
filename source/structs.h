#ifndef STRUCTS_H
#define STRUCTS_H

enum STATE 
{ 
	READY, 
	PLAYING, 
	RESTART 
};

struct Vec2D
{ 
	float x; 
	float y; 
};

struct Circle
{
    Vec2D pos;
    int r;
};

#endif