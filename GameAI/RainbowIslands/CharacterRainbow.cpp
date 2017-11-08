//------------------------------------------------------------------------
//  Author: Paul Roberts 2017
//------------------------------------------------------------------------

#include "CharacterRainbow.h"
#include "Constants_RainbowIslands.h"
#include "../Texture2D.h"
#include "../Constants.h"

const int kSpritesOnSheet = 5;

//--------------------------------------------------------------------------------------------------

CharacterRainbow::CharacterRainbow(SDL_Renderer* renderer, string imagePath, Vector2D startPosition, FACING startFacing, float delayToSpawn)
	: Character(renderer, imagePath, NULL, startPosition)
{
	mFacingDirection	= startFacing;
	mPosition			= startPosition;

	mSingleSpriteWidth	= (float)mTexture->GetWidth() / kSpritesOnSheet;	//5 sprites on this spritesheet in a row.
	mSingleSpriteHeight = (float)mTexture->GetHeight() / 2,.0f;

	mFrameDelay			= delayToSpawn;
	mCurrentFrame		= -1;
	mSpriteRow			= 0;

	mCollisionRadius	= 8.0f;
}

//--------------------------------------------------------------------------------------------------

CharacterRainbow::~CharacterRainbow()
{
}

//--------------------------------------------------------------------------------------------------

void CharacterRainbow::Update(size_t deltaTime, SDL_Event e)
{
	//Change frame?
	mFrameDelay -= deltaTime;
	if (mFrameDelay <= 0.0f)
	{
		//Reset frame delay count.
		mFrameDelay = RAINBOW_DELAY;

		//Move frame on.
		mCurrentFrame++;

		if (mCurrentFrame == 4)
		{
			mFrameDelay = RAINBOW_TIME;
			mSpriteRow = 1;
		}
		else if (mCurrentFrame >= 9)
			mAlive = false;
	}
}

//--------------------------------------------------------------------------------------------------

void CharacterRainbow::Render()
{
	//Rainbows have a delayed start if spawned as 2nd or 3rd rainbow in a row,
	//so we require no rendering for the delay.
	if (mCurrentFrame != -1)
	{
		int w = (int)mSingleSpriteWidth*(mCurrentFrame % kSpritesOnSheet);
		int h = (int)mSingleSpriteHeight*(mCurrentFrame / kSpritesOnSheet);

#		//Get the portion of the spritesheet you want to draw.
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
}

//--------------------------------------------------------------------------------------------------

Vector2D CharacterRainbow::GetStrikePosition()
{
	//This is the position of the star as a rainbow is fired.
	Vector2D pos = mPosition;

	switch (mCurrentFrame)
	{
		case 0:
			if(mFacingDirection == FACING_RIGHT)
				pos.x += mSingleSpriteWidth*0.2f;
			else
				pos.x += mSingleSpriteWidth*0.9f;

			pos.y += mSingleSpriteHeight*0.5f;
		break;

		case 1:
			if (mFacingDirection == FACING_RIGHT)
				pos.x += mSingleSpriteWidth*0.3f;
			else
				pos.x += mSingleSpriteWidth*0.7f;

			pos.y += mSingleSpriteHeight*0.2f;
		break;

		case 2:	
			if (mFacingDirection == FACING_RIGHT)
				pos.x += mSingleSpriteWidth*0.7f;
			else
				pos.x += mSingleSpriteWidth*0.3f;

			pos.y += mSingleSpriteHeight*0.2f;
		break;

		case 3:
			if (mFacingDirection == FACING_RIGHT)
				pos.x += mSingleSpriteWidth*0.9f;
			else
				pos.x += mSingleSpriteWidth*0.2f;

			pos.y += mSingleSpriteHeight*0.8f;
		break;

		default:
		break;
	}

	return pos;

}

//--------------------------------------------------------------------------------------------------