//------------------------------------------------------------------------
//  Author: Paul Roberts 2017
//------------------------------------------------------------------------

#include "Character.h"
#include "Constants_RainbowIslands.h"
#include "../Texture2D.h"
#include "LevelMap.h"

//--------------------------------------------------------------------------------------------------

Character::Character(SDL_Renderer* renderer, string imagePath, LevelMap* map, Vector2D startPosition)
{
	mTexture = new Texture2D(renderer);
	if( !mTexture->LoadFromFile(imagePath.c_str()) )
	{
		cout << "Failed to load texture: " << imagePath << endl;
		return;
	}

	mRenderer			= renderer;
	mFacingDirection	= FACING_RIGHT;
	mSingleSpriteWidth	= (float)mTexture->GetWidth();
	mSingleSpriteHeight = (float)mTexture->GetHeight();
	mJumping			= false;
	mOnGround			= false; //Will be set to true when we hit the ground.
	mMovingLeft			= false;
	mMovingRight		= false;
	mMovementSpeed		= 0.0f;
	mAlive				= true;
	mCollisionRadius	= 15.0f;//sqrt((double)(mTexture->GetWidth()*mTexture->GetWidth())+(mTexture->GetHeight()*mTexture->GetHeight()))*0.5f;
	SetPosition(startPosition);
	SetCurrentLevelMap(map);
	//SetState(CHARACTERSTATE_NONE);
}

//--------------------------------------------------------------------------------------------------

Vector2D Character::GetCentralPosition()
{
	return Vector2D(mPosition.x + mSingleSpriteWidth*0.5f, mPosition.y + mSingleSpriteHeight*0.5f);
}

//--------------------------------------------------------------------------------------------------

Character::~Character()
{
	mRenderer		 = NULL;
	mCurrentLevelMap = NULL;
}

//--------------------------------------------------------------------------------------------------

Rect2D Character::GetCollisionBox()
{ 
	return Rect2D(mPosition.x, mPosition.y, mSingleSpriteWidth, mSingleSpriteHeight);
}

//--------------------------------------------------------------------------------------------------

void Character::Render()
{
	if(mFacingDirection == FACING_RIGHT)
	{
		mTexture->Render(mPosition, SDL_FLIP_NONE);
	}
	else
	{
		mTexture->Render(mPosition, SDL_FLIP_HORIZONTAL);
	}
}

//--------------------------------------------------------------------------------------------------

void Character::Update(size_t deltaTime, SDL_Event e)
{
	//Collision position variables.
	int centralXPosition = (int)(mPosition.x+(mSingleSpriteWidth*0.5f))/TILE_WIDTH;
	int centralYPosition = (int)(mPosition.y+ mSingleSpriteHeight*0.5f)/TILE_HEIGHT;
	int footPosition	 = (int)(mPosition.y+ mSingleSpriteHeight)/TILE_HEIGHT;

	//Deal with Jumping first.
	if(mJumping)
	{
		//Adjust the position.
		mJumpForce -= JUMP_FORCE_DECREMENT;
		mPosition.y -= mJumpForce*deltaTime;

		//Has the jump force reduced to zero?
		if(mJumpForce <= 0.0f)
			mJumping = false;
	}

	//Next the left/right movement.
	if(mMovingLeft)
	{
		MoveLeft(deltaTime, centralYPosition);
	}
	else if(mMovingRight)
	{
		MoveRight(deltaTime, centralYPosition);
	}

	//Deal with gravity.
	if(mCurrentLevelMap->GetCollisionTileAt(footPosition,centralXPosition) == 0)
	{
		AddGravity(deltaTime);
	}
	else
	{
		//We want to fall if we are beyond a couple of pixels past.
		if( (int)(mPosition.y+ mSingleSpriteHeight) % TILE_HEIGHT > 3)
		{
			AddGravity(deltaTime);
		}
		else
		{
			//Collided with ground so we can jump again.
			mOnGround = true;
		}
	}	
	
	//Finally - Deal with screen edges.
	if(mPosition.x <= 0.0f)
		mPosition.x = 0.0f;
	else if(mPosition.x > kRainbowIslandsScreenWidth - mSingleSpriteHeight)
		mPosition.x = (float)(kRainbowIslandsScreenWidth - mSingleSpriteHeight);
}

//--------------------------------------------------------------------------------------------------

void Character::MoveLeft(size_t deltaTime, int yCentreOnImage)
{
	int leftPosition;
	float movement = (mMovementSpeed * deltaTime);

	leftPosition = (int)mPosition.x/TILE_WIDTH;
	if( (mCurrentLevelMap->GetCollisionTileAt(yCentreOnImage,leftPosition) == 0) ||
		(!mOnGround && mCurrentLevelMap->GetCollisionTileAt(yCentreOnImage-1,leftPosition) == 0) ) //Special jump condition
	{																					 //Can jump through as long as there is not another wall above.
		SetPosition(Vector2D(mPosition.x - movement, mPosition.y));
	}
	mFacingDirection = FACING_LEFT;
}

//--------------------------------------------------------------------------------------------------

void Character::MoveRight(size_t deltaTime, int yCentreOnImage)
{
	int rightPosition;
	float movement = (mMovementSpeed * deltaTime);

	rightPosition = (int)(mPosition.x + mSingleSpriteHeight)/TILE_WIDTH;
	if( (mCurrentLevelMap->GetCollisionTileAt(yCentreOnImage,rightPosition) == 0) ||
		(!mOnGround && mCurrentLevelMap->GetCollisionTileAt(yCentreOnImage-1,rightPosition) == 0) ) //Special jump condition
	{																					  //Can jump through as long as there is not another wall above.
		SetPosition(Vector2D(mPosition.x + movement, mPosition.y));
	}
	mFacingDirection = FACING_RIGHT;
}

//--------------------------------------------------------------------------------------------------

void Character::Jump()
{
	if(!mJumping && mOnGround)
	{
		mJumpForce = JUMP_FORCE_INITIAL;
		mJumping = true;
		mOnGround = false;
	}
}

//--------------------------------------------------------------------------------------------------

void Character::CancelJump()
{
	if(mJumping)
	{
		mJumping = false;
	}
}

//--------------------------------------------------------------------------------------------------

void Character::AddGravity(size_t deltaTime)
{
	mPosition.y += (GRAVITY * deltaTime);
	mOnGround = false;
}

//--------------------------------------------------------------------------------------------------

void Character::SetPosition(Vector2D newPosition)
{
	//Set the new position.
	mPosition.x = newPosition.x;
	mPosition.y = newPosition.y;
}

//--------------------------------------------------------------------------------------------------

Vector2D Character::GetPosition()
{
	return mPosition;
}

//--------------------------------------------------------------------------------------------------

void Character::SetCurrentLevelMap(LevelMap* newMap)
{
	mCurrentLevelMap = newMap;
}

//--------------------------------------------------------------------------------------------------

void Character::SetState(CHARACTER_STATE newState)				
{
	mState = newState;
}

//--------------------------------------------------------------------------------------------------