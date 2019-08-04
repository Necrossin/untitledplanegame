#include "cloud.h"

extern TTF_Font *planeFont;

void Cloud::initVars( std::string txt, float sc, Vec2D pos )
{
	SDL_Color cloud_col = { 239, 235, 205 };

	TTF_SetFontStyle( planeFont, TTF_STYLE_BOLD );
	TTF_SetFontOutline( planeFont, 30 );

	if(!mTexture.loadFromRenderedTextWrapped( txt, cloud_col, planeFont, 1000 ))
		printf( "Failed to render cloud texture!\n" );

	TTF_SetFontStyle( planeFont, TTF_STYLE_BOLD );
	TTF_SetFontOutline( planeFont, 0 );

	position = pos;	
	scale = sc;
}

Cloud::Cloud( std::string txt, float sc, Vec2D pos )
{
	initVars( txt, sc, pos );
}

Cloud::~Cloud()
{
	if( &mTexture != NULL )
		mTexture.free();
}

int Cloud::getWidth()
{
	return mTexture.getWidth();
}

int Cloud::getHeight()
{
	return mTexture.getHeight();
}

void Cloud::setPos( Vec2D pos )
{
	position = pos;
}

void Cloud::render()
{
	mTexture.render( position.x - mTexture.getWidth() / 2 * scale , position.y - mTexture.getHeight() / 2 * scale, 0, 0, NULL, SDL_FLIP_NONE, scale );
}