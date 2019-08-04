#ifndef CAMERA_H
#define CAMERA_H

#include "structs.h"

class Camera
{
	public:

		Camera( Vec2D pos, Vec2D size );
		~Camera();

		Vec2D getPos();
		Vec2D getSize();

		void setPos( Vec2D pos );
		void approachPos( Vec2D pos, Vec2D offset, float offsetSize, float dt );
		void setSize( Vec2D sz );

		bool isIntersectingLeftSide();
		bool isIntersectingRightSide();

	private:

		Vec2D offsetPosition;
		Vec2D position;
		Vec2D size;

};

#endif