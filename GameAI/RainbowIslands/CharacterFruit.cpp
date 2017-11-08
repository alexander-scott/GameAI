//------------------------------------------------------------------------
//  Author: Paul Roberts 2017
//------------------------------------------------------------------------

#include "CharacterFruit.h"
#include "Constants_RainbowIslands.h"
#include "../Texture2D.h"
#include "LevelMap.h"
#include "../Constants.h"

const int kSpritesOnSheet = FRUIT_MAX;
//--------------------------------------------------------------------------------------------------

CharacterFruit::CharacterFruit(SDL_Renderer* renderer, string imagePath, LevelMap* map, Vector2D startPosition, FRUIT_TYPE type)
	: Character(renderer, imagePath, map, startPosition)
{
	mMovementSpeed		= CALM_SPEED;
	mPosition			= startPosition;

	mSingleSpriteWidth	= (float)mTexture->GetWidth() / (float)kSpritesOnSheet;	//Number of sprites on this spritesheet in a row.
	mSingleSpriteHeight = (float)mTexture->GetHeight();// / 2.0f;

	mAngle				= 0.0;
	mFruitType			= type;

	mJumping			= false;
	mOnGround			= true;
}

//--------------------------------------------------------------------------------------------------

CharacterFruit::~CharacterFruit()
{
}

//--------------------------------------------------------------------------------------------------

void CharacterFruit::Update(size_t deltaTime, SDL_Event e)
{
	if (!mOnGround)
		mAngle += deltaTime;
	else
	{
		mMovingLeft  = false;
		mMovingRight = false;
	}

	Character::Update(deltaTime, e);
}

//--------------------------------------------------------------------------------------------------

void CharacterFruit::Render()
{
	int w = (int)mSingleSpriteWidth*(mFruitType % kSpritesOnSheet);
	int h = 0;

	//Get the portion of the spritesheet you want to draw.
	//								{XPos, YPos, WidthOfSingleSprite, HeightOfSingleSprite}
	SDL_Rect portionOfSpritesheet = { w, h, (int)mSingleSpriteWidth, (int)mSingleSpriteHeight };
	//Determine where you want it drawn.
	SDL_Rect destRect = {(int)(mPosition.x), (int)(mPosition.y), (int)mSingleSpriteWidth, (int)mSingleSpriteHeight};

	//Then draw it.
	mTexture->Render(portionOfSpritesheet, destRect, SDL_FLIP_NONE, mAngle);

}

//--------------------------------------------------------------------------------------------------

void CharacterFruit::Jump()
{
	if (!mJumping && mOnGround)
	{
		mJumpForce = JUMP_FORCE_INITIAL_FRUIT;
		mJumping = true;
		mOnGround = false;

		//Will bounce left, right or just up and down.
		int direction = rand() % 100;
		if (direction > 66)
			mMovingLeft = true;
		else if (direction < 33)
			mMovingRight = true;
	}
}

//--------------------------------------------------------------------------------------------------
