//------------------------------------------------------------------------
//  Author: Paul Roberts 2017
//------------------------------------------------------------------------

#include "CharacterChest.h"
#include "Constants_RainbowIslands.h"
#include "../Texture2D.h"
#include "LevelMap.h"
#include "../Constants.h"

const int kSpritesOnSheet = 2;
//--------------------------------------------------------------------------------------------------

CharacterChest::CharacterChest(SDL_Renderer* renderer, string imagePath, LevelMap* map, Vector2D startPosition)
	: Character(renderer, imagePath, map, startPosition)
{
	mMovementSpeed		= CALM_SPEED;
	mPosition			= startPosition;

	mSingleSpriteWidth	= (float)mTexture->GetWidth() / (float)kSpritesOnSheet;	//Number of sprites on this spritesheet in a row.
	mSingleSpriteHeight = (float)mTexture->GetHeight();// / 2.0f;

	mCurrentFrame		= 0;
	mOpen				= false;
	mOnGround			= false;
}

//--------------------------------------------------------------------------------------------------

CharacterChest::~CharacterChest()
{
}

//--------------------------------------------------------------------------------------------------

void CharacterChest::Update(size_t deltaTime, SDL_Event e)
{
	if (mOnGround && !mOpen)
		OpenChest();

	Character::Update(deltaTime, e);
}

//--------------------------------------------------------------------------------------------------

void CharacterChest::Render()
{
	int w = (int)mSingleSpriteWidth*(mCurrentFrame % kSpritesOnSheet);
	int h = 0;

	//Get the portion of the spritesheet you want to draw.
	//								{XPos, YPos, WidthOfSingleSprite, HeightOfSingleSprite}
	SDL_Rect portionOfSpritesheet = { w, h, (int)mSingleSpriteWidth, (int)mSingleSpriteHeight };
	//Determine where you want it drawn.
	SDL_Rect destRect = {(int)(mPosition.x), (int)(mPosition.y), (int)mSingleSpriteWidth, (int)mSingleSpriteHeight};

	//Then draw it.
	mTexture->Render(portionOfSpritesheet, destRect, SDL_FLIP_NONE);

}

//--------------------------------------------------------------------------------------------------

void CharacterChest::OpenChest()
{
	mCurrentFrame = 1;
	mOpen		  = true;
}

//--------------------------------------------------------------------------------------------------
