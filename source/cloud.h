#ifndef CLOUD_H
#define CLOUD_H

#include "structs.h"
#include "ltexture.h"
#include <string>
#include <SDL_ttf.h>

class Cloud
{
	public:

		Cloud( std::string txt, float sc, Vec2D pos );
		~Cloud();

		void render();
		void initVars( std::string txt, float sc, Vec2D pos );

		void setPos( Vec2D pos );

		int getWidth();
        int getHeight();

	private:

		LTexture mTexture;

		Vec2D position;
		float scale;
		
		int mWidth;
        int mHeight;

};

#endif