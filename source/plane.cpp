#include "plane.h"

extern TTF_Font *planeFont;
extern int GAME_STATE;

extern const int LEVEL_WIDTH;
extern const int LEVEL_HEIGHT;
extern const int FPS;

extern const int WATER_HEIGHT;
extern const int WATER_YPOS;

extern const Vec2D GRAVITY_DIR;

extern double normalizeAngle( double ang );
extern double deg2rad( double deg );
extern int distSqr( Vec2D vec1, Vec2D vec2 );
extern Vec2D vec_normalized( Vec2D vec );
extern double clamp(double x, double upper, double lower);
extern bool isOnScreen( Vec2D vec );

extern std::list <Projectile> PROJECTILE_LIST;
extern std::list <Explosion> EXPLOSION_LIST;


void Plane::initVars( bool isPlayer, std::string planeText, Vec2D pos, double ang, float mxSpeed, float mass, SDL_Color textColor, float hp )
{

	SDL_Color color_white = { 255, 255, 255, 255 };

	if ( !isPlayer )
	{
		TTF_SetFontStyle( planeFont, TTF_STYLE_BOLD );
		TTF_SetFontOutline( planeFont, 1 );
	}
	else
	{
		TTF_SetFontStyle( planeFont, TTF_STYLE_BOLD );
		TTF_SetFontOutline( planeFont, 0 );
	}

	if(!mTexture.loadFromRenderedText( planeText, textColor, planeFont ))
		printf( "Failed to render plane texture!\n" );

	if(!mJetTexture.loadFromRenderedText( "*", color_white, planeFont ))
		printf( "Failed to render jet texture!\n" );

	if(!mMuzzleTexture.loadFromRenderedText( "^", color_white, planeFont ))
		printf( "Failed to render muzzle texture!\n" );

	TTF_SetFontStyle( planeFont, TTF_STYLE_BOLD );
	TTF_SetFontOutline( planeFont, 0 );

	setPos( pos );

	mCollisionBounds.pos = getPos();
	mCollisionBounds.r = std::min( mTexture.getWidth()/2, mTexture.getHeight()/2 );

	setRotAng( ang );
	rotSpeed = 6;

	if ( !isPlayer )
		rotSpeed = 3;

	forceVelDir();

	curThrust = 0;
	maxThrust = 1;

	curSpeed = 0;
	maxSpeed = mxSpeed;

	curVelocity.x = 0;
	curVelocity.y = 0;

	weight = mass;

	nextFire = 0;

	setHealth( hp );
	setMaxHealth( hp );

	shouldRemove = false;

	accelerating = false;

	ownedByPlayer = isPlayer;

	nextRegen = 0;
	nextEnvDamage = 0;
}

Plane::Plane( bool isPlayer, std::string planeText, Vec2D pos, double ang, float mxSpeed, float mass, SDL_Color textColor, int hp )
{
	initVars( isPlayer, planeText, pos, ang, mxSpeed, mass, textColor, hp );
};

Plane::~Plane()
{
	if( &mTexture != NULL )
		mTexture.free();

	if( &mJetTexture != NULL )
		mTexture.free();

	if( &mMuzzleTexture != NULL )
		mTexture.free();
};

void Plane::render(SDL_Rect* clip, SDL_Point* center, SDL_RendererFlip flip) //, SDL_Renderer &curRenderer 
{
	if ( ownedByPlayer & GAME_STATE != STATE::PLAYING )
		return;
	
	//muzzleflash
	if ( ownedByPlayer & ( ( nextFire - ( 0.1/3 * 1000.f ) ) > SDL_GetTicks() ) )
	{
		mMuzzleTexture.render( position.x + getRotDir().x * ( mTexture.getHeight() / 3.5 ) - mMuzzleTexture.getWidth() / 2, position.y + getRotDir().y * ( mTexture.getHeight() / 3.5 ) - mMuzzleTexture.getHeight()/2, clip, getRotAng() + 90.0, NULL, flip );
	}

	//jet stream
	if ( isAccelerating() & ownedByPlayer )
	{
		//reusing mussleflash but as a jet stream
		mMuzzleTexture.render( position.x - getRotDir().x * ( mTexture.getHeight() / 1.2 ) - mMuzzleTexture.getWidth() / 2, position.y - getRotDir().y * ( mTexture.getHeight() / 1.2 ) - mMuzzleTexture.getHeight()/2, clip, getRotAng() + 90.0, NULL, flip );

		int am = rand() % 13 + 1;

		for( int i = 1; i<=am; ++i )
		{
			double spread_ang = normalizeAngle ( getRotAng() + ( rand() % 17 - 8 ) );
			float spread_rad = deg2rad( spread_ang );

			Vec2D spread_dir;
			spread_dir.x = std::cos( spread_rad );
			spread_dir.y = std::sin( spread_rad );

			int drawPosX = position.x - spread_dir.x * ( mTexture.getWidth() / 2 + 12 + i * 3 * ( curThrust/maxThrust ) );
			int drawPosY = position.y - spread_dir.y * ( mTexture.getHeight() / 2 + 12 + i * 3 * ( curThrust/maxThrust ) );

			mJetTexture.render( drawPosX - mJetTexture.getWidth() / 2 * 0.8, drawPosY - mJetTexture.getHeight()/2 * 0.8, clip, getRotAng() + 90.0, NULL, flip, 0.8 );

		}
	}

	// plane
	mTexture.render( position.x - mTexture.getWidth() / 2 , position.y - mTexture.getHeight() / 2, clip, getRotAng() + 90.0, NULL, flip );


};

void Plane::forceInstantVelocityRaw( float x, float y )
{
	curVelocity.x = x;
	curVelocity.y = y;
}

void Plane::setPos( Vec2D pos ) 
{
	position.x = pos.x;
	position.y = pos.y;

	mCollisionBounds.pos = getPos();
};

void Plane::setVelDir( Vec2D dir ) 
{
	direction.x = dir.x;
	direction.y = dir.y;
};

void Plane::setPosRaw( float x, float y ) 
{
	position.x = x;
	position.y = y;

	mCollisionBounds.pos = getPos();
};

void Plane::setVelDirRaw( float x, float y ) 
{
	direction.x = x;
	direction.y = y;
};

void Plane::forceVelDir()
{
	direction = getRotDir();
}

void Plane::setRotAng( double ang )
{
	rotation = ang;
};

Vec2D Plane::getPos()
{
	return position;
}

Vec2D Plane::getVelDir()
{
	return direction;
}

Vec2D Plane::getCurVelocity()
{
	return curVelocity;
}


Vec2D Plane::getRotDir()
{
	float rad = deg2rad( getRotAng() );
	Vec2D new_dir;
	new_dir.x = std::cos( rad );
	new_dir.y = std::sin( rad );

	return new_dir;
}

double Plane::getRotAng()
{
	return rotation;
};

float Plane::getRotPower()
{
	if (isAccelerating())
		return 0.3;
	else
		return 1;
};

void Plane::setNextEnvDamage( float delay )
{
	nextEnvDamage = SDL_GetTicks() + delay * 1000.f;
}

float Plane::getNextEnvDamage()
{
	return nextEnvDamage;
}

Circle Plane::getCollisionBounds()
{
	return mCollisionBounds;
}

void Plane::setAccelerating( bool acc )
{
	accelerating = acc;
}

bool Plane::isAccelerating()
{
	return accelerating;
}

void Plane::setHealth( float hp )
{
	health = hp;
}


void Plane::setMaxHealth( float hp )
{
	maxHealth = hp;
}

float Plane::getHealth()
{
	return health;
}

float Plane::getMaxHealth()
{
	return maxHealth;
}

bool Plane::proceedDamage( int dmg )
{
	nextRegen = SDL_GetTicks() + 0.5 * 1000.f;
	
	setHealth( getHealth() - dmg );

	if ( getHealth() <= 0 )
		return false;

	return true;
}

void Plane::handleEvent( SDL_Event& e )
{
	if( e.type == SDL_KEYDOWN )
    {
    }
};

void Plane::handleKeyboardState( float dt )
{
	if ( GAME_STATE != STATE::PLAYING )
		return;

	if (isPlayerPlane())
	{
		const Uint8* currentKeyStates = SDL_GetKeyboardState( NULL );
		if( currentKeyStates[ SDL_SCANCODE_LEFT ] )
		{
			setRotAng( getRotAng() - rotSpeed * getRotPower() * dt * FPS );
		}
		if( currentKeyStates[ SDL_SCANCODE_RIGHT ] )
		{
			setRotAng( getRotAng() + rotSpeed * getRotPower() * dt * FPS );
		}
		if( currentKeyStates[ SDL_SCANCODE_UP ] )
			setAccelerating( true );
		else
			setAccelerating( false );
		if( currentKeyStates[ SDL_SCANCODE_X ] )
		{
			fireProjectile();
		}
		if( currentKeyStates[ SDL_SCANCODE_SPACE ] )
		{
			fireProjectile();
		}
	}
};

void Plane::doLogic( Plane &playerPlane, float dt )
{
	if (!isPlayerPlane())
	{
		
		// calculate angle

		Vec2D dir;
		Vec2D playerPos = playerPlane.getPos();

		if ( std::abs( playerPos.x - getPos().x ) > LEVEL_WIDTH / 2 )
		{
			playerPos.x = playerPos.x - LEVEL_WIDTH;
		}

		dir.x = playerPos.x - getPos().x;
		dir.y = playerPos.y - getPos().y;

		dir = vec_normalized( dir );

		float goal_ang = std::atan2( dir.y, dir.x );
		goal_ang = (goal_ang / M_PI) * 180.0;
		
		float diff = normalizeAngle( goal_ang - getRotAng() );
		float diff_abs = std::abs( diff );

		if ( diff_abs > 15 )
		{
			if ( diff > 0 )
				setRotAng( getRotAng() + rotSpeed * getRotPower() * dt * FPS );
			else
				setRotAng( getRotAng() - rotSpeed * getRotPower() * dt * FPS );
		}
		

		Vec2D vec_ahead;

		vec_ahead.x = dir.x * 100;
		vec_ahead.y = dir.y * 100;

		// accelerate when too far and when too close
		float distanceSqr = distSqr( playerPos, getPos() );

		if ( ( distanceSqr > 300*300 ) & ( getPos().y < ( WATER_YPOS + 15 ) )  ) //& ( vec_ahead.y > WATER_YPOS ) &
		{
			setAccelerating( true );
		}
		else
		{
			if ( ( distanceSqr < 180*180 ) ) //  & ( vec_ahead.y > WATER_YPOS ) & ( getPos().x > ( WATER_YPOS ) )
				setAccelerating( true );
			else
				setAccelerating( false );
		}

		// fire bullets when on screen and within 30 degrees cone

		if ( isOnScreen( getPos() ) & ( diff_abs < 30 ) )
		{
			fireProjectile();
		}
	}
	else
	{
		if ( ( position.y + mTexture.getHeight() > WATER_YPOS ) & ( nextEnvDamage <= SDL_GetTicks() )  )
		{	
			if ( !proceedDamage( 1 ) )
			{
				markForRemoval();
			}
			nextEnvDamage = SDL_GetTicks() + 0.03 * 1000.f;
		}



		if ( ( health < maxHealth ) & ( nextRegen < SDL_GetTicks() )  )
		{
			setHealth( std::max( std::min( int(getHealth() + 1), int( maxHealth ) ), 0 ) );
			nextRegen = SDL_GetTicks() + 0.015 * 1000.f;
		}
	}
}

void Plane::updateMovement( float dt )
{
	if ( ownedByPlayer & GAME_STATE != STATE::PLAYING )
		return;
	
	if (isAccelerating())
	{
		
		forceVelDir();

		curThrust = std::min( curThrust + 0.08 * dt * FPS, maxThrust * 1.0 );

		float clamped_x = std::abs( direction.x * maxSpeed );
		float clamped_y = std::abs( direction.y * maxSpeed );

		curVelocity.x = clamp( curVelocity.x + direction.x * curThrust * dt * FPS, clamped_x, -1 * clamped_x );
		curVelocity.y = clamp( curVelocity.y + direction.y * curThrust * dt * FPS, clamped_y, -1 * clamped_y );
	}
	else
	{
		curThrust = 0;
		curVelocity.x = curVelocity.x * 0.99;
		curVelocity.y = curVelocity.y * 0.99;
	}


	position.x += curVelocity.x * dt * FPS;

	if ( position.y + mTexture.getHeight()/2 > WATER_YPOS )
	{
		position.y += curVelocity.y * dt * FPS + GRAVITY_DIR.y * weight * (  ( 1 - ( LEVEL_HEIGHT - position.y ) / ( WATER_HEIGHT ) ) * 1 ) * dt * FPS;
		curVelocity.y = curVelocity.y - 6 * ( ( 1 - ( LEVEL_HEIGHT - position.y ) / ( WATER_HEIGHT ) ) ) * dt * FPS;
	}
	else if ( position.y - mTexture.getHeight()/2 < 200 )
	{
		position.y += curVelocity.y * dt * FPS - GRAVITY_DIR.y * weight * (  ( 1 - ( position.y ) / ( 200 ) ) * 1 ) * dt * FPS;
		curVelocity.y = curVelocity.y + 2 * ( ( 1 - ( position.y ) / ( 200 ) ) ) * dt * FPS;
	}
	else
	{
		if (isAccelerating())
			position.y += curVelocity.y * dt * FPS;
		else
			position.y += curVelocity.y * dt * FPS - GRAVITY_DIR.y * weight * dt * FPS;
	}

	//bounds

	if ( position.x + mTexture.getWidth()/2 < 0 )
		position.x = LEVEL_WIDTH - mTexture.getWidth()/2;
	if ( position.x - mTexture.getWidth()/2 > LEVEL_WIDTH )
		position.x = mTexture.getWidth()/2 ;

	if ( isPlayerPlane() )
	{
		if ( position.y - mTexture.getHeight()/2 < 0 )
			position.y = mTexture.getHeight()/2;
		if ( position.y + mTexture.getHeight()/2 > LEVEL_HEIGHT )
			position.y = LEVEL_HEIGHT - mTexture.getHeight()/2 ;
	}

	mCollisionBounds.pos = getPos();

};

void Plane::fireProjectile()
{
	if( nextFire <= SDL_GetTicks() )
	{

		Vec2D spawnPos;
		float sz = std::max( mTexture.getWidth()/2, mTexture.getHeight()/2 );  

		spawnPos.x = getPos().x + getRotDir().x * sz;
		spawnPos.y = getPos().y + getRotDir().y * sz;

		double spread_ang = getRotAng() + ( rand() % 9 - 4 );
		float spread_rad = deg2rad( spread_ang );

		Vec2D spread_dir;
		spread_dir.x = std::cos( spread_rad );
		spread_dir.y = std::sin( spread_rad );

		Projectile* Bullet = new Projectile( isPlayerPlane(), spawnPos, spread_dir, maxSpeed + weight + 9 );
		Bullet->initVars( isPlayerPlane(), spawnPos, spread_dir, maxSpeed + weight + 9 );

		PROJECTILE_LIST.push_back( *Bullet );

		float delay = 0.1;

		if (!isPlayerPlane())
			delay = ( rand() % 8 + 6 ) * 0.1;

		nextFire = SDL_GetTicks() + delay * 1000.f;
		nextRegen = SDL_GetTicks() + 0.2 * 1000.f;
	}
}

int Plane::getWidth()
{
	return mTexture.getWidth();
}

int Plane::getHeight()
{
	return mTexture.getHeight();
}

bool Plane::isPlayerPlane()
{
	return ownedByPlayer;
}

void Plane::markForRemoval()
{
	shouldRemove = true;
}

void Plane::unMarkForRemoval()
{
	shouldRemove = false;
}

bool Plane::removeThisFrame()
{
	return shouldRemove;
}

void Plane::doExplosion()
{
	int r = rand() % 6 + 3;
	float time = r * 0.1;

	float scale = 0.8;

	if ( ownedByPlayer )
	{
		//scale = 2;
		time = r * 0.5;
	}

	Explosion* Exp = new Explosion( getPos(), scale, time );
	Exp->initVars( getPos(), scale, time  );

	EXPLOSION_LIST.push_back( *Exp );

}