#ifndef PLANE_H
#define PLANE_H

#include <SDL_ttf.h>
#include <cmath>
#include <string>
#include <list> 
#include "structs.h"
#include "ltexture.h"
#include "projectile.h"
#include "explosion.h"

class Plane
{
	public:
		Plane( bool isPlayer, std::string planeText, Vec2D pos, double ang, float mxSpeed, float mass, SDL_Color textColor, int hp );
		~Plane();

		void initVars( bool isPlayer, std::string planeText, Vec2D pos, double ang, float mxSpeed, float mass, SDL_Color textColor, float hp );
		void setPos( Vec2D pos );
		void setPosRaw( float x, float y );
		void setVelDir( Vec2D dir );
		void setVelDirRaw( float x, float y );
		void setRotAng( double ang );
		void forceVelDir();
		void setHealth( float hp );
		void setMaxHealth( float hp );
		float getHealth();
		float getMaxHealth();

		void forceInstantVelocityRaw( float x, float y ); 

		bool proceedDamage( int dmg );

		void render( SDL_Rect* clip = NULL, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

		void setAccelerating( bool acc ); 
		bool isAccelerating();
		bool isPlayerPlane();

		void updateMovement( float dt );

		void doExplosion();
		
		Vec2D getPos();
		Vec2D getVelDir();
		Vec2D getRotDir();
		Vec2D getCurVelocity();

		double getRotAng();
		float getRotPower();
		float getNextEnvDamage();

		void setNextEnvDamage( float delay );

		int getWidth();
        int getHeight();

		void handleEvent( SDL_Event& e );
		void handleKeyboardState( float dt );
		void doLogic( Plane &playerPlane, float dt );

		void markForRemoval();
		void unMarkForRemoval();
		bool removeThisFrame();

		Circle getCollisionBounds();

	private:
		
		LTexture mTexture;
		LTexture mJetTexture;
		LTexture mMuzzleTexture;

		Vec2D position;
		Vec2D direction;
		Vec2D curVelocity;

		float health;
		float maxHealth;

		float curThrust;
		float maxThrust;

		float curSpeed;
		float maxSpeed;
		float rotSpeed;
		bool accelerating;
		bool ownedByPlayer;
		float weight;
		float nextFire;
		float nextEnvDamage;

		void fireProjectile();

		bool shouldRemove;

		double rotation;
		Circle mCollisionBounds;

		float nextRegen;

};

#endif