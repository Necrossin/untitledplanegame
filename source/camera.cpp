#include "camera.h"

extern Vec2D lerpVec2D( float mul, Vec2D a, Vec2D b );
extern double clamp(double x, double upper, double lower);

extern const int LEVEL_WIDTH;
extern const int LEVEL_HEIGHT;

extern const int FPS;

Camera::Camera( Vec2D pos, Vec2D size )
{
	Vec2D offset = { 0, 0 };
	setSize( size );
	setPos( pos ); 

	offsetPosition.x = 0;
	offsetPosition.y = 0;
}

Camera::~Camera()
{
}

void Camera::approachPos( Vec2D pos, Vec2D offset, float offsetSize, float dt )
{
	Vec2D goal;

	goal.x = offset.x * offsetSize;
	goal.y = offset.y * offsetSize;
	

	offsetPosition = lerpVec2D( dt * FPS * 0.07, offsetPosition, goal );
	

	position.x = pos.x + offsetPosition.x - size.x/2;
	position.y = pos.y + offsetPosition.y - size.y/2;

	//position.x = clamp( position.x, LEVEL_WIDTH - size.x, 0 );
	position.y = clamp( position.y, LEVEL_HEIGHT - size.y, 0 );
}

void Camera::setPos( Vec2D pos )
{
	position.x = pos.x - size.x/2;
	position.y = pos.y - size.y/2;

	//position.x = clamp( position.x, LEVEL_WIDTH - size.x, 0 );
	position.y = clamp( position.y, LEVEL_HEIGHT - size.y, 0 );
}

void Camera::setSize( Vec2D sz )
{
	size = sz;
}

Vec2D Camera::getPos()
{
	return position;
}

Vec2D Camera::getSize()
{
	return size;
}

bool Camera::isIntersectingLeftSide()
{
	return ( position.x < size.x * 2 );
}

bool Camera::isIntersectingRightSide()
{
	return ( ( position.x + size.x * 2 ) > LEVEL_WIDTH );
}
