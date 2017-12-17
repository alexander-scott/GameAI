//------------------------------------------------------------------------
//  Author: Paul Roberts 2017
//------------------------------------------------------------------------

#ifndef _CHARACTER_H
#define _CHARACTER_H

#include <SDL.h>
#include "Commons_RainbowIslands.h"
#include "../Commons.h"
#include "../Texture2D.h"
#include <iostream>
using namespace::std;

class LevelMap;

class Character
{
//--------------------------------------------------------------------------------------------------
public:
	Character(SDL_Renderer* renderer, string imagePath, LevelMap* map, Vector2D startPosition);
	~Character();

	void			SetCurrentLevelMap(LevelMap* newMap);

	virtual void	Render();
	virtual void	Update(size_t deltaTime, SDL_Event e);

	void			SetPosition(Vector2D newPosition);
	Vector2D		GetPosition();
	Vector2D		GetCentralPosition();

	FACING			 GetFacing()									{return mFacingDirection;}

	CHARACTER_STATE GetState()										{return mState;}
	virtual void	SetState(CHARACTER_STATE newState);
	bool			IsInState(CHARACTER_STATE state)				{return mState == state;}

	bool			IsJumping()										{return mJumping;}
	void			CancelJump();

	bool			GetAlive()										{return mAlive;}
	void			SetAlive(bool yesNo)							{mAlive = yesNo;}

	virtual bool	OnARainbow()									{ return false; }
	virtual void	SetOnARainbow(bool yesNo)						{ ; }

	float			GetCollisionRadius()							{return mCollisionRadius;}
	Rect2D			GetCollisionBox();

	virtual void	TakeDamage()									{;}//Override with specific functionality.
	virtual void	MakeAngry()										{;}//Override with specific functionality.
//--------------------------------------------------------------------------------------------------
protected:
	virtual void	MoveLeft(size_t deltaTime, int yCentreOnImage);
	virtual void	MoveRight(size_t deltaTime, int yCentreOnImage);
	virtual void	Jump();
	void			AddGravity(size_t deltaTime);
	
//--------------------------------------------------------------------------------------------------
protected:
	SDL_Renderer* mRenderer;
	Vector2D	  mPosition;
	Texture2D*	  mTexture;
	FACING		  mFacingDirection;
	float		  mSingleSpriteWidth;
	float		  mSingleSpriteHeight;

	LevelMap*	  mCurrentLevelMap;

	float	      mMovementSpeed;
	bool		  mMovingLeft;
	bool		  mMovingRight;
	bool		  mJumping;			//Currently on an upward trajectory.
	bool		  mOnGround;		//Flag to check if character is on the ground before allowing a jump.
	float		  mJumpForce;		//Force to add to y position when jumping.

	CHARACTER_STATE mState;
	bool			mAlive;

	float			mCollisionRadius;
};
#endif //_CHARACTER_H