//------------------------------------------------------------------------
//  Author: Paul Roberts 2017
//------------------------------------------------------------------------

#include "CharacterBub.h"
#include "Constants_RainbowIslands.h"
#include "LevelMap.h"
#include "../Texture2D.h"
#include "../Constants.h"
#include "VirtualJoypad.h"

//--------------------------------------------------------------------------------------------------

const int kSpritesOnSheet = 7;

CharacterBub::CharacterBub(SDL_Renderer* renderer, string imagePath, LevelMap* map, Vector2D startPosition)
	: Character(renderer, imagePath, map, startPosition)
{
	mFacingDirection	= FACING_RIGHT;
	mPosition			= startPosition;

	mSingleSpriteWidth	= (float)mTexture->GetWidth() / (float)kSpritesOnSheet;	//7 sprites on this spritesheet in a row.
	mSingleSpriteHeight = (float)mTexture->GetHeight() / 4.0f;  //4 rows.

	mCollisionRadius	= 10.0f;
	mOnRainbow			= false;
	mPoints				= 0;
	SetState(CHARACTERSTATE_NONE);
}

//--------------------------------------------------------------------------------------------------

CharacterBub::~CharacterBub()
{
}

//--------------------------------------------------------------------------------------------------

void CharacterBub::Render()
{
	int w = (int)mSingleSpriteWidth*(mCurrentFrame % kSpritesOnSheet);
	int h = (int)mSingleSpriteHeight*(mCurrentFrame / kSpritesOnSheet);

#	//Get the portion of the spritesheet you want to draw.
	//								{XPos, YPos, WidthOfSingleSprite, HeightOfSingleSprite}
	SDL_Rect portionOfSpritesheet = { w, h, (int)mSingleSpriteWidth, (int)mSingleSpriteHeight };

	//Determine where you want it drawn.
	SDL_Rect destRect = { (int)(mPosition.x), (int)(mPosition.y), (int)mSingleSpriteWidth, (int)mSingleSpriteHeight };

	//Then draw it.
	if (mFacingDirection == FACING_LEFT)
		mTexture->Render(portionOfSpritesheet, destRect, SDL_FLIP_HORIZONTAL);
	else
		mTexture->Render(portionOfSpritesheet, destRect, SDL_FLIP_NONE);
}

//--------------------------------------------------------------------------------------------------

void CharacterBub::Update(size_t deltaTime, SDL_Event e)
{
	//cout << deltaTime << endl;

	//Change frame?
	mFrameDelay -= deltaTime;
	if (mFrameDelay <= 0.0f)
	{
		//Reset frame delay count.
		mFrameDelay = ANIMATION_DELAY;

		//Move frame on.
		mCurrentFrame++;

		//Loop frame around if it goes beyond the number of frames.
		if (mCurrentFrame > mEndFrame)
		{
			if (IsInState(CHARACTERSTATE_PLAYER_DEATH))
				mAlive = false;
			else
				mCurrentFrame = mStartFrame;
		}
	}

	//Update if not in death state.
	if(!IsInState(CHARACTERSTATE_PLAYER_DEATH))
	{
		AlternateCharacterUpdate(deltaTime, e);

		if (mOnGround && IsInState(CHARACTERSTATE_JUMP))
		{
			if (mMovingLeft || mMovingRight)
				SetState(CHARACTERSTATE_WALK);
			else
				SetState(CHARACTERSTATE_NONE);
		}

		//Collision position variables.
		int centralXPosition = (int)(mPosition.x+(mTexture->GetWidth()*0.5f))/TILE_WIDTH;
		int centralYPosition  = (int)(mPosition.y+mTexture->GetHeight()*0.5f)/TILE_HEIGHT;
		int footPosition  = (int)(mPosition.y+mTexture->GetHeight())/TILE_HEIGHT;

	
		if (VirtualJoypad::Instance()->LeftArrow)
		{
			mMovingLeft = true;
			mMovingRight = false;
			if (mOnGround)
				SetState(CHARACTERSTATE_WALK);
		}
		else if (VirtualJoypad::Instance()->RightArrow)
		{
			mMovingRight = true;
			mMovingLeft = false;
			if (mOnGround)
				SetState(CHARACTERSTATE_WALK);
		}
		else
		{
			mMovingLeft = false;
			mMovingRight = false;
			if (mOnGround)
				SetState(CHARACTERSTATE_NONE);
		}

		if (VirtualJoypad::Instance()->UpArrow)
		{
			if (!mJumping)
			{
				SetState(CHARACTERSTATE_JUMP);
				Jump();
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------

void CharacterBub::AlternateCharacterUpdate(size_t deltaTime, SDL_Event e)
{
	//Collision position variables.
	int centralXPosition = (int)(mPosition.x + (mSingleSpriteWidth*0.5f)) / TILE_WIDTH;
	int centralYPosition = (int)(mPosition.y + mSingleSpriteHeight*0.5f) / TILE_HEIGHT;
	int footPosition = (int)(mPosition.y + mSingleSpriteHeight) / TILE_HEIGHT;

	//Deal with Jumping first.
	if (mJumping)
	{
		//Adjust the position.
		mJumpForce -= JUMP_FORCE_DECREMENT;
		mPosition.y -= mJumpForce*deltaTime;

		//Has the jump force reduced to zero?
		if (mJumpForce <= 0.0f)
			mJumping = false;
	}

	//Next the left/right movement.
	if (mMovingLeft)
	{
		MoveLeft(deltaTime, centralYPosition);
	}
	else if (mMovingRight)
	{
		MoveRight(deltaTime, centralYPosition);
	}

	//Deal with gravity.
	if (mCurrentLevelMap->GetCollisionTileAt(footPosition, centralXPosition) == 0 && !mOnRainbow)
	{
		AddGravity(deltaTime);
	}
	else
	{
		//We want to fall if we are beyond a couple of pixels past.
		if ((int)(mPosition.y + mSingleSpriteHeight) % TILE_HEIGHT > 3)
		{
			AddGravity(deltaTime);
		}
		else
		{
			//TODO: Fix exiting a rainbow onto a platform.
			if (mOnRainbow)
				mOnRainbow = false;

			//Collided with ground so we can jump again.
			mOnGround = true;
		}
	}

	//Finally - Deal with screen edges.
	if (mPosition.x <= 0.0f)
		mPosition.x = 0.0f;
	else if (mPosition.x > kRainbowIslandsScreenWidth - mSingleSpriteHeight)
		mPosition.x = (float)(kRainbowIslandsScreenWidth - mSingleSpriteHeight);
}

//--------------------------------------------------------------------------------------------------

void CharacterBub::SetState(CHARACTER_STATE newState)
{
	if (mState != newState)
	{
		mState = newState;
		//Reset frame delay count.
		mFrameDelay = ANIMATION_DELAY;

		switch (mState)
		{
			case CHARACTERSTATE_NONE:
				mMovementSpeed = 0.0f;
				mStartFrame = 0;
				mCurrentFrame = 0;
				mEndFrame = 0;

				//Stop movement.
				mMovingLeft = false;
				mMovingRight = false;
			break;

			case CHARACTERSTATE_WALK:
				mMovementSpeed = MOVE_SPEED;
				mStartFrame = 1;
				mCurrentFrame = 1;
				mEndFrame = 5;
			break;

			case CHARACTERSTATE_JUMP:
				mMovementSpeed = MOVE_SPEED;
				mStartFrame = 6;
				mCurrentFrame = 6;
				mEndFrame = 7;
			break;

			case CHARACTERSTATE_PLAYER_DEATH:
				mMovementSpeed = 0.0f;
				mStartFrame = 8;
				mCurrentFrame = 8;
				mEndFrame = 16;

				//Stop movement.
				mMovingLeft = false;
				mMovingRight = false;
			break;

			default:
			break;
		}
	}
}

//--------------------------------------------------------------------------------------------------

void CharacterBub::MoveLeft(size_t deltaTime, int yCentreOnImage)
{
	int leftPosition;
	float movement = (mMovementSpeed * deltaTime);

	leftPosition = (int)mPosition.x / TILE_WIDTH;
	if ( mOnRainbow ||
		(mCurrentLevelMap->GetCollisionTileAt(yCentreOnImage, leftPosition) == 0) ||
		(!mOnGround && mCurrentLevelMap->GetCollisionTileAt(yCentreOnImage - 1, leftPosition) == 0)) //Special jump condition
	{																					 //Can jump through as long as there is not another wall above.
		SetPosition(Vector2D(mPosition.x - movement, mPosition.y));
	}
	mFacingDirection = FACING_LEFT;
}

//--------------------------------------------------------------------------------------------------

void CharacterBub::MoveRight(size_t deltaTime, int yCentreOnImage)
{
	int rightPosition;
	float movement = (mMovementSpeed * deltaTime);

	rightPosition = (int)(mPosition.x + mSingleSpriteHeight) / TILE_WIDTH;
	if ( mOnRainbow || 
		(mCurrentLevelMap->GetCollisionTileAt(yCentreOnImage, rightPosition) == 0) ||
		(!mOnGround && mCurrentLevelMap->GetCollisionTileAt(yCentreOnImage - 1, rightPosition) == 0))	//Special jump condition
	{																									//Can jump through as long as there is not another wall above.
		SetPosition(Vector2D(mPosition.x + movement, mPosition.y));
	}
	mFacingDirection = FACING_RIGHT;
}

//--------------------------------------------------------------------------------------------------

void CharacterBub::Jump()
{
	if (!mJumping && (mOnGround || mOnRainbow))
	{
		mJumpForce = JUMP_FORCE_INITIAL;
		mJumping = true;
		mOnGround = false;
		mOnRainbow = false;
	}
}

//--------------------------------------------------------------------------------------------------

int	CharacterBub::GetRainbowsAllowed()
{
	if (mPoints < 70)
		return 1;
	else if (mPoints < 170)
		return 2;
	else
		return 3;
}

//--------------------------------------------------------------------------------------------------