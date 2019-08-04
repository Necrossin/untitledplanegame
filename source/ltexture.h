#ifndef LTEXTURE_H
#define LTEXTURE_H

#include "structs.h"
#include "camera.h"
#include <string>
#include <SDL_ttf.h>

class LTexture
{
    public:
        LTexture();
        ~LTexture();

        bool loadFromRenderedText( std::string textureText, SDL_Color textColor, TTF_Font* font );
		bool loadFromRenderedTextWrapped( std::string textureText, SDL_Color textColor, TTF_Font* font, Uint32 wraplength );

        void free();
        void setColor( Uint8 red, Uint8 green, Uint8 blue );
        void setBlendMode( SDL_BlendMode blending );
        void setAlpha( Uint8 alpha );
        void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE, float scale = 1.0, bool ignoreReflection = false );

        int getWidth();
        int getHeight();

    private:
        SDL_Texture* mTexture;

        int mWidth;
        int mHeight;
};

#endif