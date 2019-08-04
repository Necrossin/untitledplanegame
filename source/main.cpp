#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_timer.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <list> 
#include <iterator> 
#include <ctime>

#include "structs.h"
#include "camera.h"
#include "ltexture.h"
#include "projectile.h"
#include "plane.h"
#include "cloud.h"
#include "explosion.h"


extern const int SCREEN_WIDTH = 1600;
extern const int SCREEN_HEIGHT = 800;

extern const int LEVEL_WIDTH = 8000;
extern const int LEVEL_HEIGHT = 4000;

extern const int WATER_HEIGHT = 500;
extern const int WATER_YPOS = LEVEL_HEIGHT - WATER_HEIGHT;

int GAME_STATE = STATE::READY;


const std::string cloud = " _(88)_(8)_\n"
							"(88888888)\n"
							"  (8) (88)\n";

const Vec2D viewStartPos = { LEVEL_WIDTH/2, WATER_YPOS - 150 };
const Vec2D viewStartSize = { SCREEN_WIDTH, SCREEN_HEIGHT };

const Vec2D viewStartOffset = { 0, 0 };

extern const int FPS = 60;
const int delay = 1000 / FPS;

const int MAX_ENEMIES = 25;

extern const Vec2D GRAVITY_DIR = { 0, -1 };

const char* GLOBAL_FONT = "arial.ttf";

static int nextEnemySpawn = 0;

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
TTF_Font *planeFont = NULL;
TTF_Font *projectileFont = NULL;
TTF_Font *explosionFont = NULL;
TTF_Font *healthFont = NULL;

std::list <Projectile> PROJECTILE_LIST;
std::list <Plane> ENEMIES_LIST;
std::list <Cloud> CLOUD_LIST;
std::list <Explosion> EXPLOSION_LIST;

LTexture healthOverlay;
LTexture startText;
LTexture restartText;

Camera viewPort = *(new Camera( viewStartPos, viewStartSize ));

bool init();
bool loadMedia();
void close();

int distSqr( Vec2D vec1, Vec2D vec2 )
{
    int x = vec2.x - vec1.x;
    int y = vec2.y - vec1.y;

    return x*x + y*y;
}

bool isColliding( Circle& obj1, Circle& obj2 )
{
	if ( GAME_STATE != STATE::PLAYING )
		return false;

	int radSqr = obj1.r + obj2.r;
    radSqr = radSqr * radSqr;

    if( distSqr( obj1.pos, obj2.pos ) < ( radSqr ) )
    {
        return true;
    }
	return false;
}

Vec2D lerpVec2D( float mul, Vec2D a, Vec2D b )
{
	Vec2D result;

	result.x = a.x + mul * ( b.x - a.x );
	result.y = a.y + mul * ( b.y - a.y );

	return result;
}

double clamp(double x, double upper, double lower)
{
    return std::min(upper, std::max(x, lower));
}

double normalizeAngle( double ang )
{
    ang = fmod( ang + 180,360 );
    if ( ang < 0 )
        ang += 360;
    return ang - 180;
}

double deg2rad( double deg ) 
{
    return deg * M_PI / 180.0;
}

float vec_length( Vec2D vec )
{
	return sqrt( pow( vec.x, 2 ) + pow( vec.y, 2 ) );
}

Vec2D vec_normalized( Vec2D vec )
{
	float d = vec_length( vec );

	if (d == 0.0) 
	{
		vec.y = d = 1.0;
	}

	d = 1 / d;

	vec.x *= d;
	vec.y *= d;

	return vec;
}

bool isOnScreen( Vec2D vec )
{
	return ( vec.x > ( viewPort.getPos().x ) & vec.x < ( viewPort.getPos().x + viewPort.getSize().x ) & vec.y > ( viewPort.getPos().y ) & vec.y < ( viewPort.getPos().y + viewPort.getSize().y ) );
}

void spawnEnemy( Vec2D spawnPos )
{
	SDL_Color enemyColor = { 142, 73, 78 };

	Plane* enemyPlane = new Plane( false, "A", spawnPos, rand() % 360, rand() % 18 + 13, 7, enemyColor, 25 );
	ENEMIES_LIST.push_back( *enemyPlane );
}

void spawnEnemiesOverTime( float delay )
{
	if ( ( ENEMIES_LIST.size() < MAX_ENEMIES ) & ( nextEnemySpawn <= SDL_GetTicks() ) )
	{

		Vec2D spawnPos;

		spawnPos.x = rand() % LEVEL_WIDTH;
		spawnPos.y = rand() % 200 + 50;

		spawnPos.y = spawnPos.y * -1;

		spawnEnemy( spawnPos );

		nextEnemySpawn = SDL_GetTicks() + delay * 1000.f;
	}
}

void createClouds()
{
	
	for ( int i = 1; i < ( rand() % 90 + 70  ); i++ )
	{
		Vec2D randPos;
		
		randPos.x = rand() % LEVEL_WIDTH;
		randPos.y = rand() % LEVEL_HEIGHT * 0.8;

		Cloud* newCloud = new Cloud( cloud, ( rand() % 9 + 1 ) * 0.2, randPos );
		newCloud->initVars( cloud, ( rand() % 9 + 1 ) * 0.2, randPos );

		randPos.x = clamp( randPos.x, LEVEL_WIDTH - newCloud->getWidth(), newCloud->getWidth() );

		newCloud->setPos( randPos );

		CLOUD_LIST.push_back( *newCloud );
	}

}

bool init()
{
	bool success = true;

	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		gWindow = SDL_CreateWindow( "Untitled Plane Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				SDL_SetRenderDrawColor( gRenderer, 229, 221, 172, 255 );
				SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND );


				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}

				if( TTF_Init() == -1 )
				{
					printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	bool success = true;

	planeFont = TTF_OpenFont( GLOBAL_FONT, 42 );
	projectileFont = TTF_OpenFont( GLOBAL_FONT, 48 );
	healthFont = TTF_OpenFont( GLOBAL_FONT, 200 );
	explosionFont = TTF_OpenFont( GLOBAL_FONT, 100 );

	SDL_Color colorWhite = { 255, 255, 255, 255 };
	SDL_Color colorBackground = { 229, 221, 172, 255 };
	SDL_Color textColor = { 96, 0, 28 };

	if( planeFont == NULL )
	{
		printf( "Failed to load font! SDL_ttf Error: %s\n", TTF_GetError() );
		success = false;
	}
	else
	{
		TTF_SetFontStyle( planeFont, TTF_STYLE_BOLD );
		TTF_SetFontStyle( healthFont, TTF_STYLE_BOLD );
		TTF_SetFontOutline( healthFont, 50 );

		if(!healthOverlay.loadFromRenderedText( "O", colorBackground, healthFont ))
			printf( "Failed to render health overlay texture!\n" );

		TTF_SetFontOutline( healthFont, 0 );

		if(!startText.loadFromRenderedTextWrapped( "PRESS SPACEBAR TO START", textColor, planeFont, SCREEN_WIDTH ))
			printf( "Failed to render start text texture!\n" );


		if(!restartText.loadFromRenderedTextWrapped( "PRESS SPACEBAR TO CONTINUE", textColor, planeFont, SCREEN_WIDTH ))
			printf( "Failed to render restart text texture!\n" );
	}

	return success;
}

void close()
{
	healthOverlay.free();
	startText.free();
	restartText.free();

	TTF_CloseFont( planeFont );
	TTF_CloseFont( projectileFont );
	TTF_CloseFont( healthFont );
	TTF_CloseFont( explosionFont );

	planeFont = NULL;
	projectileFont = NULL;
	explosionFont = NULL;

	ENEMIES_LIST.clear();
	PROJECTILE_LIST.clear();
	CLOUD_LIST.clear();


	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

bool is_projectile_dead(Projectile &item)
{ 
	if( item.getLifeTime() <= SDL_GetTicks() )
		return true;

	if( item.hitAlready() )
		return true;

	if ( ( item.getPos().y + item.getHeight()/2 ) > WATER_YPOS )
		return true;
	
	return false; 
} 

bool is_plane_dead(Plane &item)
{ 
	if( item.removeThisFrame() )
		return true;
	
	return false; 
} 

bool is_explosion_expired(Explosion &item)
{ 
	if( item.getLifeTime() <= SDL_GetTicks() )
		return true;
	
	return false; 
} 

int main( int argc, char* args[] )
{
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{	
			Uint32 startTime = SDL_GetTicks();

			std::list <Projectile>::iterator it_prj;
			std::list <Plane>::iterator it_pln;
			std::list <Cloud>::iterator it_cld;
			std::list <Explosion>::iterator it_exp;

			srand( time( NULL ) );

			viewPort.setPos( viewStartPos );
			viewPort.setSize( viewStartSize );

			SDL_Color playerColor = { 96, 0, 28 };
			SDL_Color colorWhite = { 255, 255, 255, 255 };

			SDL_Rect healthBackground;
			SDL_Rect waterSurface;

			float healthMul;
			float thresold;
			float velLength;
			float healthScale;

			float nextRestartPress = 0;

			createClouds();

			Plane testPlane( true, "<|>", viewStartPos, -90, 20, 7, playerColor, 125 );

			bool quit = false;

			SDL_Event e;

			while( !quit )
			{
				
				while( SDL_PollEvent( &e ) != 0 )
				{
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}

					if ( GAME_STATE == STATE::READY )
					{
						if( e.type == SDL_KEYDOWN & e.key.keysym.sym == SDLK_SPACE & nextRestartPress < SDL_GetTicks() )
						{
							testPlane.setPos( viewStartPos );
							testPlane.setRotAng( -90 );
							testPlane.setHealth( 125 );
							testPlane.forceVelDir();
							testPlane.forceInstantVelocityRaw( 0, -40 );
							testPlane.setAccelerating( true );
							testPlane.unMarkForRemoval();


							viewPort.setPos( viewStartPos );
							viewPort.setSize( viewStartSize );

							GAME_STATE = STATE::PLAYING;
						}

					}

					if ( GAME_STATE == STATE::RESTART )
					{
						if( e.type == SDL_KEYDOWN & e.key.keysym.sym == SDLK_SPACE & nextRestartPress < SDL_GetTicks() )
						{
							ENEMIES_LIST.clear();
							PROJECTILE_LIST.clear();
							//CLOUD_LIST.clear();

							viewPort.setPos( viewStartPos );
							viewPort.setSize( viewStartSize );

							nextRestartPress = SDL_GetTicks() + 0.5 * 1000.f;

							//createClouds();

							GAME_STATE = STATE::READY;
						}

					}
				}

				float dt = (SDL_GetTicks() - startTime) / 1000.f;

				SDL_SetRenderDrawColor( gRenderer, 229, 221, 172, 255 );
				SDL_RenderClear( gRenderer );	

				bool isPlaying = false;

				if ( GAME_STATE == STATE::PLAYING )
					isPlaying = true;

				if ( GAME_STATE == STATE::RESTART )
					isPlaying = true;
				
				
				if ( isPlaying )
				{
					spawnEnemiesOverTime( 0.7 );
					
					testPlane.handleKeyboardState( dt );
					testPlane.doLogic( testPlane, dt );

					healthMul = testPlane.getHealth() / testPlane.getMaxHealth();
					thresold = 0.98;

					testPlane.updateMovement( dt );

					Vec2D velOffset = testPlane.getCurVelocity();
					velLength = vec_length( velOffset );
					velOffset = vec_normalized( velOffset );

					viewPort.approachPos( testPlane.getPos(), velOffset, velLength * 10 + 10, dt );

					SDL_Point circleCenter;

					circleCenter.x = testPlane.getPos().x;
					circleCenter.y = testPlane.getPos().y;

					healthScale = healthMul * 3.5;

					healthBackground.x = 0;
					healthBackground.y = 0;
					healthBackground.w = SCREEN_WIDTH;
					healthBackground.h = SCREEN_HEIGHT;

					if ( healthMul < thresold & GAME_STATE == STATE::PLAYING ) 
					{
						SDL_SetRenderDrawColor( gRenderer, 255, 255, 255, ( 1 - healthMul ) * 230 + 10 );
						SDL_RenderFillRect( gRenderer, &healthBackground );
						healthOverlay.render( circleCenter.x - healthOverlay.getWidth() / 2 * healthScale , circleCenter.y - healthOverlay.getHeight() / 2 * healthScale, 0, 0, NULL, SDL_FLIP_NONE, healthScale, true );
					}

					for(it_cld = CLOUD_LIST.begin(); it_cld != CLOUD_LIST.end(); ++it_cld)
					{
						(*it_cld).render();
					}

					// render player plane
					testPlane.render();

					//render enemies
					for(it_pln = ENEMIES_LIST.begin(); it_pln != ENEMIES_LIST.end(); ++it_pln)
					{
						(*it_pln).doLogic( testPlane, dt );
						(*it_pln).updateMovement( dt );
						(*it_pln).render();

						// vs player impact collision
						if( ( isColliding( testPlane.getCollisionBounds(), (*it_pln).getCollisionBounds() ) ) & ( testPlane.getNextEnvDamage() < SDL_GetTicks() )  ) 
						{
							testPlane.setNextEnvDamage( 0.1 );

							if ( !testPlane.proceedDamage( 15 ) )
								testPlane.markForRemoval();

							if ( !(*it_pln).proceedDamage( 25 ) )
							{
								(*it_pln).markForRemoval();
								(*it_pln).doExplosion();
							}
						}

					}

					// projectiles render
					for(it_prj = PROJECTILE_LIST.begin(); it_prj != PROJECTILE_LIST.end(); ++it_prj)
					{
						(*it_prj).updateMovement( dt );
						(*it_prj).render();

						//check collision:
						//	vs enemy planes
						if ( (*it_prj).isPlayerProjectile() )
						{
							for(it_pln = ENEMIES_LIST.begin(); it_pln != ENEMIES_LIST.end(); ++it_pln)
							{
								if( ( isColliding( (*it_pln).getCollisionBounds(), (*it_prj).getCollisionBounds() ) ) & ! (*it_prj).hitAlready() ) 
								{
									(*it_prj).markForRemoval();
									if ( !(*it_pln).proceedDamage( 25 ) )
									{
										(*it_pln).markForRemoval();
										(*it_pln).doExplosion();
									}
								
								}
							}
				
						}// vs player plane
						else
						{
							if( ( isColliding( testPlane.getCollisionBounds(), (*it_prj).getCollisionBounds() ) ) & ! (*it_prj).hitAlready() ) 
								{
									(*it_prj).markForRemoval();
									if ( !testPlane.proceedDamage( 20 ) )
									{
										testPlane.markForRemoval();
									}
								
								}
						}
					}

					for(it_exp = EXPLOSION_LIST.begin(); it_exp != EXPLOSION_LIST.end(); ++it_exp )
					{
						(*it_exp).render();
					}
				
					// delete exploded/expired projectiles
					PROJECTILE_LIST.remove_if( is_projectile_dead ); 

					//delete dead planes
					ENEMIES_LIST.remove_if( is_plane_dead );

					//delete expired explosions
					EXPLOSION_LIST.remove_if( is_explosion_expired );

					if ( testPlane.removeThisFrame() & GAME_STATE != STATE::RESTART )
					{
						GAME_STATE = STATE::RESTART;
						testPlane.setAccelerating( false );
						testPlane.doExplosion();
						nextRestartPress = SDL_GetTicks() + 0.5 * 1000.f;
					}

					if ( GAME_STATE == STATE::RESTART )
					{
						restartText.render( viewPort.getPos().x + viewPort.getSize().x/2 - restartText.getWidth() / 2 * 1, viewPort.getPos().y + viewPort.getSize().y/2 - restartText.getHeight() / 2 * 1, 0, 0, NULL, SDL_FLIP_NONE, 1, false );
					}

				}
				else
				{
					for(it_cld = CLOUD_LIST.begin(); it_cld != CLOUD_LIST.end(); ++it_cld)
					{
						(*it_cld).render();
					}

					startText.render( viewStartPos.x - startText.getWidth() / 2 * 1, viewStartPos.y - startText.getHeight() / 2 * 1 , 0, 0, NULL, SDL_FLIP_NONE, 1, false );
	
				}


				//water rendering
				SDL_SetRenderDrawColor( gRenderer, 255, 255, 255, 255 );
				
				waterSurface.x = 0;
				waterSurface.y = WATER_YPOS - viewPort.getPos().y;
				waterSurface.w = viewPort.getSize().x;
				waterSurface.h = 5;

				SDL_RenderFillRect( gRenderer, &waterSurface );
				
				SDL_SetRenderDrawColor( gRenderer, 255, 255, 255, 90 );
				
				waterSurface.x = 0;
				waterSurface.y = WATER_YPOS - viewPort.getPos().y;
				waterSurface.w = viewPort.getSize().x;
				waterSurface.h = WATER_HEIGHT;

				SDL_RenderFillRect( gRenderer, &waterSurface );

				startTime = SDL_GetTicks();

				SDL_RenderPresent( gRenderer );
			}
		}
	}

	close();

	return 0;
}