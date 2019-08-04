//a nice wrapper for sdl textures based on random tutorials online, but with added camera/water reflection support

#include "ltexture.h"

extern SDL_Renderer* gRenderer;

extern Camera viewPort;

extern const int WATER_HEIGHT;
extern const int WATER_YPOS;

extern const int LEVEL_WIDTH;
extern const int LEVEL_HEIGHT;

LTexture::LTexture()
{
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	free();
}

bool LTexture::loadFromRenderedTextWrapped( std::string textureText, SDL_Color textColor, TTF_Font* font, Uint32 wraplength )
{

	free();

	SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped( font, textureText.c_str(), textColor, wraplength );//TTF_RenderText_Blended( font, textureText.c_str(), textColor );
	if( textSurface == NULL )
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}
	else
	{
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( mTexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		SDL_FreeSurface( textSurface );
	}
	
	return mTexture != NULL;
}


bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor, TTF_Font* font )
{
	free();

	SDL_Surface* textSurface = TTF_RenderText_Blended( font, textureText.c_str(), textColor );
	if( textSurface == NULL )
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}
	else
	{
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( mTexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		SDL_FreeSurface( textSurface );
	}
	
	return mTexture != NULL;
}

void LTexture::free()
{
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
	SDL_SetTextureBlendMode( mTexture, blending );
}
		
void LTexture::setAlpha( Uint8 alpha )
{
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip, float scale, bool ignoreReflection )
{
	
	SDL_Rect clipQuad = { 0, 0, viewPort.getSize().x, WATER_YPOS - viewPort.getPos().y };

	SDL_RenderSetClipRect( gRenderer, &clipQuad );

	int posX = x - viewPort.getPos().x;
	int posY = y - viewPort.getPos().y;


	SDL_Rect renderQuad = { x - viewPort.getPos().x, y - viewPort.getPos().y, mWidth * scale, mHeight * scale };

	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );

	//copy all sprites on the opposite side of the level, for seamless transition and looping the level in some way. like a tiled texture
	
	//camera is on far left side, texture is on far right
	if ( viewPort.isIntersectingLeftSide() & ( x - mWidth/2 * scale ) > ( LEVEL_WIDTH - viewPort.getSize().x ) )
	{
		renderQuad.x = x - LEVEL_WIDTH - viewPort.getPos().x;//- viewPort.getSize().x;

		if( clip != NULL )
		{
			renderQuad.w = clip->w;
			renderQuad.h = clip->h;
		}

		SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
	}

	//camera is on far right side, texture is on far left
	if ( viewPort.isIntersectingRightSide() & ( x + mWidth/2 * scale ) < ( viewPort.getSize().x ) )
	{

		renderQuad.x = x + LEVEL_WIDTH - viewPort.getPos().x;//- viewPort.getSize().x;

		if( clip != NULL )
		{
			renderQuad.w = clip->w;
			renderQuad.h = clip->h;
		}

		SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
	}

	if ( ( ( y + mHeight/2 * scale ) > ( WATER_YPOS - WATER_HEIGHT ) ) & ( !ignoreReflection ) )
	{

		renderQuad.y = WATER_YPOS + ( WATER_YPOS - y - mHeight * scale ) - viewPort.getPos().y;

		if( clip != NULL )
		{
			renderQuad.w = clip->w;
			renderQuad.h = clip->h;
		}


		//SDL_Rect clipQuad = { 0, WATER_YPOS - viewPort.getPos().y, viewPort.getSize().x, WATER_HEIGHT };

		clipQuad.x = 0;
		clipQuad.y = WATER_YPOS - viewPort.getPos().y;
		clipQuad.w = viewPort.getSize().x;
		clipQuad.h = WATER_HEIGHT;

		SDL_RenderSetClipRect( gRenderer, &clipQuad );

		SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle * -1, center,  SDL_FLIP_VERTICAL );

		/*clipQuad.x = 0;
		clipQuad.y = 0;
		clipQuad.w = viewPort.getSize().x;
		clipQuad.h = viewPort.getSize().y;

		SDL_RenderSetClipRect( gRenderer, &clipQuad );*/

	}

	clipQuad.x = 0;
	clipQuad.y = 0;
	clipQuad.w = viewPort.getSize().x;
	clipQuad.h = viewPort.getSize().y;

	SDL_RenderSetClipRect( gRenderer, &clipQuad );
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}