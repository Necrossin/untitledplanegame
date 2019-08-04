#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <SDL_ttf.h>
#include <cmath>

#include "structs.h"
#include "ltexture.h"


class Explosion
{
	public:

		Explosion( Vec2D pos, float sc, float time );
		~Explosion();

		void initVars( Vec2D pos, float sc, float time );
		void render();

		float getLifeTime();

	private:

		LTexture mCore;
		LTexture mBits;

		Vec2D *bitsPos;
		Vec2D position;

		float scale;
		float lifeTime;
		float lifeTimeDuration;
		int bitsAm;

};


#endif