#include "projectile.h"

extern TTF_Font *projectileFont;

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

extern const int LEVEL_WIDTH;
extern const int LEVEL_HEIGHT;

extern const int FPS;

Projectile::Projectile( bool isPlayer, Vec2D pos, Vec2D dir, float speed )
{
	initVars( isPlayer, pos, dir, speed );
};

void Projectile::initVars( bool isPlayer, Vec2D pos, Vec2D dir, float speed )
{

	SDL_Color enemyColor = { 128, 80, 80 };
	SDL_Color playerColor = { 255, 255, 255 };

	TTF_SetFontOutline( projectileFont, 0 );

	if ( isPlayer )
	{
		TTF_SetFontOutline( projectileFont, 4 );
		mTexture.loadFromRenderedText( "O", playerColor, projectileFont );
	}
	else
		mTexture.loadFromRenderedText( "U", enemyColor, projectileFont );

	TTF_SetFontOutline( projectileFont, 0 );

	setPosRaw( SCREEN_WIDTH/4 - mTexture.getWidth()/2, SCREEN_HEIGHT/4 - mTexture.getHeight()/2 );

	curSpeed = speed;
	maxSpeed = speed;

	setPos( pos );
	setVelDir( dir );

	mCollisionBounds.pos = getPos();
	mCollisionBounds.r = ( mTexture.getWidth()/2 + mTexture.getHeight()/2 ) / 2;

	ownedByPlayer = isPlayer;

	shouldRemove = false;

	lifeTime = SDL_GetTicks() + 3 * 1000.f;
}

Projectile::~Projectile()
{
	if( &mTexture != NULL )
		mTexture.free();
};

void Projectile::render(SDL_Rect* clip, SDL_Point* center, SDL_RendererFlip flip)
{
	float goal_ang = std::atan2( direction.y, direction.x );
	goal_ang = (goal_ang / M_PI) * 180.0;

	mTexture.render( position.x - mTexture.getWidth() / 2 , position.y - mTexture.getHeight() / 2, clip, goal_ang - 90.0, NULL, flip );
};

void Projectile::markForRemoval()
{
	shouldRemove = true;
}

bool Projectile::hitAlready()
{
	return shouldRemove;
}

bool Projectile::isPlayerProjectile()
{
	return ownedByPlayer;
}

void Projectile::setPos( Vec2D pos ) 
{
	position.x = pos.x;
	position.y = pos.y;
};

void Projectile::setVelDir( Vec2D dir ) 
{
	direction.x = dir.x;
	direction.y = dir.y;
};


void Projectile::setPosRaw( float x, float y ) 
{
	position.x = x;
	position.y = y;
};

void Projectile::setVelDirRaw( float x, float y ) 
{
	direction.x = x;
	direction.y = y;
};

Vec2D Projectile::getPos()
{
	return position;
}

float Projectile::getLifeTime()
{
	return lifeTime;
}

Circle Projectile::getCollisionBounds()
{
	return mCollisionBounds;
}

int Projectile::getWidth()
{
	return mTexture.getWidth();
}

int Projectile::getHeight()
{
	return mTexture.getHeight();
}


void Projectile::updateMovement( float dt )
{
	curVelocity.x = direction.x * curSpeed;
	curVelocity.y = direction.y * curSpeed;

	position.x += curVelocity.x * dt * FPS;
	position.y += curVelocity.y * dt * FPS;

	mCollisionBounds.pos = getPos();

	//bounds

	if ( position.x + mTexture.getWidth()/2 < 0 )
		position.x = LEVEL_WIDTH - mTexture.getWidth()/2;
	if ( position.x - mTexture.getWidth()/2 > LEVEL_WIDTH )
		position.x = mTexture.getWidth()/2 ;

};