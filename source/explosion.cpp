#include "explosion.h"

extern TTF_Font *explosionFont;

Explosion::Explosion( Vec2D pos, float sc, float time )
{
	initVars( pos, sc, time );
}

Explosion::~Explosion()
{
	if( &mCore != NULL )
		mCore.free();

	if( &mBits != NULL )
		mBits.free();

	delete [] bitsPos;
}

void Explosion::initVars( Vec2D pos, float sc, float time )
{
	SDL_Color colorWhite = { 255, 255, 255 };
	SDL_Color colorDark = { 96, 0, 28 };

	TTF_SetFontStyle( explosionFont, TTF_STYLE_BOLD );
	TTF_SetFontOutline( explosionFont, 20 );
	mBits.loadFromRenderedText( "O", colorDark, explosionFont );
	mCore.loadFromRenderedText( "X", colorWhite, explosionFont );
	TTF_SetFontOutline( explosionFont, 0 );

	position = pos;

	bitsAm = rand() % 5 + 3;

	bitsPos = new Vec2D[ bitsAm ]; 

	for ( int i=0; i < bitsAm; i++ )
	{
		bitsPos[ i ].x = rand() % 201 - 100;
		bitsPos[ i ].y = rand() % 201 - 100;
	}

	scale = sc;
	lifeTimeDuration = time * 1000.f;
	lifeTime = SDL_GetTicks() + lifeTimeDuration;

}

float Explosion::getLifeTime()
{
	return lifeTime;
}

void Explosion::render()
{
	float delta = ( 1 - ( lifeTime - SDL_GetTicks() ) / lifeTimeDuration );

	//core
	for ( int i = 1; i <= 2; i ++)
	{
		float pos_x = position.x - mCore.getWidth() / 2 * scale * ( 1 - delta ) + ( rand() % 15 - 7 );
		float pos_y = position.y - mCore.getHeight() / 2 * scale * ( 1 - delta ) + ( rand() % 15 - 7 );

		mCore.render( pos_x, pos_y, 0, 0, NULL, SDL_FLIP_NONE, scale * ( 1 - delta ) );
	}

	//bits

	int expBits = 3;

	for ( int i = 0; i < bitsAm; i++ )
	{
		for ( int j=1; j <= expBits; j++ )
		{
			float pos_x = position.x - mBits.getWidth() / 2 * scale * ( 1 - delta ) * ( 1 - 0.2 * j ) + ( j * bitsPos[ i ].x / expBits ) * delta + ( rand() % 5 - 2 );
			float pos_y = position.y - mBits.getHeight() / 2 * scale * ( 1 - delta ) * ( 1 - 0.2 * j ) + ( j * bitsPos[ i ].y / expBits ) * delta + ( rand() % 5 - 2 );

			mBits.render( pos_x, pos_y, 0, 0, NULL, SDL_FLIP_NONE, scale * ( 1 - delta ) * ( 1 - 0.2 * j ) );
		}

	}
}