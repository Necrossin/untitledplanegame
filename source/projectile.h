#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <SDL_ttf.h>
#include <cmath>

#include "structs.h"
#include "ltexture.h"


class Projectile
{
	public:

		Projectile( bool isPlayer, Vec2D pos, Vec2D dir, float speed );
		~Projectile();

		void initVars( bool isPlayer, Vec2D pos, Vec2D dir, float speed );
		void setPos( Vec2D pos );
		void setPosRaw( float x, float y );
		void setVelDir( Vec2D dir );
		void setVelDirRaw( float x, float y );

		void render( SDL_Rect* clip = NULL, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

		void updateMovement( float dt );

		Vec2D getPos();
		float getLifeTime();

		int getWidth();
        int getHeight();

		bool isPlayerProjectile();
		bool hitAlready();

		void markForRemoval();

		Circle getCollisionBounds();

	private:

		LTexture mTexture;

		Vec2D position;
		Vec2D direction;
		Vec2D curVelocity;

		float curSpeed;
		float maxSpeed;

		float lifeTime;

		bool ownedByPlayer;
		bool shouldRemove;

		Circle mCollisionBounds;

};

#endif