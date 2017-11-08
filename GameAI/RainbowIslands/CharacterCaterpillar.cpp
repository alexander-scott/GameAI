//------------------------------------------------------------------------
//  Author: Paul Roberts 2017
//------------------------------------------------------------------------

#include "CharacterCaterpillar.h"
#include "Constants_RainbowIslands.h"
#include "../Texture2D.h"
#include "LevelMap.h"
#include "../Constants.h"

//--------------------------------------------------------------------------------------------------

CharacterCaterpillar::CharacterCaterpillar(SDL_Renderer* renderer, string imagePath, LevelMap* map, Vector2D startPosition, FACING startFacing)
	: Character(renderer, imagePath, map, startPosition)
{
	mFacingDirection	= startFacing;
	mMovementSpeed		= CALM_SPEED;
	mPosition			= startPosition;

	mSingleSpriteWidth	= (float)mTexture->GetWidth() / 2.0f;	//2 sprites on this spritesheet in a row.
	mSingleSpriteHeight = (float)mTexture->GetHeight() / 2.0f;//2 rows.
	mCollisionRadius	= 4.0f;

	mFrameDelay			= ANIMATION_DELAY;
	mCurrentFrame		= 0;

	mCurrentMood		= BADDIE_NORMAL;
}

//--------------------------------------------------------------------------------------------------

CharacterCaterpillar::~CharacterCaterpillar()
{
}

//--------------------------------------------------------------------------------------------------

void CharacterCaterpillar::Update(size_t deltaTime, SDL_Event e)
{
	//Change frame?
	mFrameDelay -= deltaTime;
	if (mFrameDelay <= 0.0f)
	{
		//Reset frame delay count.
		mFrameDelay = ANIMATION_DELAY;

		//Move frame on.
		mCurrentFrame++;

		//Loop frame around if it goes beyond the number of frames.
		if (mCurrentFrame > 1)
			mCurrentFrame = 0;
	}

	int centralYPosition = (int)(mPosition.y + mSingleSpriteHeight*0.5f) / TILE_HEIGHT;
	if (mFacingDirection == FACING_LEFT)
	{
		int leftPosition = (int)((mPosition.x + mSingleSpriteWidth*0.5f) / TILE_WIDTH)-1;
		if ((mCurrentLevelMap->GetCollisionTileAt(centralYPosition+1, leftPosition) == 0 && mCurrentMood == BADDIE_NORMAL) ||
			(leftPosition < 0))
		{
			mMovingLeft = false;
			mMovingRight = true;
		}
		else
		{
			mMovingLeft = true;
			mMovingRight = false;
		}
	}
	else if(mFacingDirection == FACING_RIGHT)
	{
		int rightPosition = (int)((GetCentralPosition().x + mSingleSpriteWidth) / TILE_WIDTH);
		if ( (mCurrentLevelMap->GetCollisionTileAt(centralYPosition+1, rightPosition) == 0 && mCurrentMood == BADDIE_NORMAL) ||
			 (rightPosition > MAP_WIDTH) )
		{
			mMovingLeft = true;
			mMovingRight = false;
		}
		else
		{
			mMovingRight = true;
			mMovingLeft = false;
		}
	}

	Character::Update(deltaTime, e);
}

//--------------------------------------------------------------------------------------------------

void CharacterCaterpillar::Render()
{
	//Get the portion of the spritesheet you want to draw.
	//								{XPos, YPos, WidthOfSingleSprite, HeightOfSingleSprite}
	SDL_Rect portionOfSpritesheet = {(int)mSingleSpriteWidth*mCurrentFrame, mCurrentMood*(int)mSingleSpriteHeight, (int)mSingleSpriteWidth, (int)mSingleSpriteHeight};

	//Determine where you want it drawn.
	SDL_Rect destRect = {(int)(mPosition.x), (int)(mPosition.y), (int)mSingleSpriteWidth, (int)mSingleSpriteHeight};

	//Then draw it.
	if (mFacingDirection == FACING_LEFT)
		mTexture->Render(portionOfSpritesheet, destRect, SDL_FLIP_HORIZONTAL);
	else
		mTexture->Render(portionOfSpritesheet, destRect, SDL_FLIP_NONE);

}

//--------------------------------------------------------------------------------------------------

void CharacterCaterpillar::MakeAngry()
{
	mCurrentMood   = BADDIE_ANGRY;
	mMovementSpeed = ANGRY_SPEED;
}

//--------------------------------------------------------------------------------------------------
