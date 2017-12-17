//------------------------------------------------------------------------
//  Author: Paul Roberts 2017
//------------------------------------------------------------------------

#include "CharacterBub.h"
#include "Constants_RainbowIslands.h"
#include "LevelMap.h"
#include "../Texture2D.h"
#include "../Constants.h"

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
	mCanSpawnRainbow	= true;
	mPoints				= 0;
	SetState(CHARACTERSTATE_NONE);
}

//--------------------------------------------------------------------------------------------------

CharacterBub::~CharacterBub()
{
	//Rainbows.
	mRainbows.clear();
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

void CharacterBub::RenderRainbows()
{
	//Draw the Rainbows.
	for (unsigned int i = 0; i < mRainbows.size(); i++)
	{
		mRainbows[i]->Render();
	}
}

void CharacterBub::UpdateRainbows(size_t deltaTime, SDL_Event e, vector<Character*>& enemies)
{
	//--------------------------------------------------------------------------------------------------
	//Update the Rainbows.
	//--------------------------------------------------------------------------------------------------

	//Need to turn flag off each frame incase the rainbow disapated.
	SetOnARainbow(false);

	if (!mRainbows.empty())
	{
		int rainbowIndexToDelete = -1;

		for (unsigned int i = 0; i < mRainbows.size(); i++)
		{
			int xPosition = (int)GetPosition().x + (int)(GetCollisionBox().width*0.5f);
			int footPosition = (int)(GetPosition().y + GetCollisionBox().height);

			//Update the rainbow.
			mRainbows[i]->Update(deltaTime, e);

			if (!mRainbows[i]->GetAlive())
				rainbowIndexToDelete = i;
			else
			{
				//check if the player has collided with it.
				if (!IsJumping())
				{
					if (Collisions::Instance()->PointInBox(Vector2D(xPosition, footPosition), mRainbows[i]->GetCollisionBox()))
					{
						SetState(CHARACTERSTATE_WALK);
						SetOnARainbow(true);
						int xPointOfCollision = (int)(mRainbows[i]->GetPosition().x + mRainbows[i]->GetCollisionBox().width - xPosition);
						if (GetFacing() == FACING_RIGHT)
							xPointOfCollision = (int)(xPosition - mRainbows[i]->GetPosition().x);

						//We don't want to pop between walking on different rainbows. Ensure the switch between rainbows looks 'realistic'
						double distanceBetweenPoints = footPosition - (mRainbows[i]->GetPosition().y - RainbowOffsets[xPointOfCollision]);
						if (distanceBetweenPoints < 40.0)
							SetPosition(Vector2D(GetPosition().x, mRainbows[i]->GetPosition().y - RainbowOffsets[xPointOfCollision]));
					}
				}

				//Check for collisions with enemies.
				for (unsigned int j = 0; j < enemies.size(); j++)
				{
					if (mRainbows[i]->CanKill())
					{
						if (Collisions::Instance()->Circle(mRainbows[i]->GetStrikePosition(), mRainbows[i]->GetCollisionRadius(), enemies[j]->GetPosition(), enemies[j]->GetCollisionRadius()))
						{
							enemies[j]->SetAlive(false);
						}
					}
				}
			}
		}

		//--------------------------------------------------------------------------------------------------
		//Remove a dead rainbow - 1 each update.
		//--------------------------------------------------------------------------------------------------
		if (rainbowIndexToDelete != -1)
		{
			Character* toDelete = mRainbows[rainbowIndexToDelete];
			mRainbows.erase(mRainbows.begin() + rainbowIndexToDelete);
			delete toDelete;
			toDelete = NULL;
		}
	}
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

		if (VirtualJoypad::Instance()->DownArrow && mCanSpawnRainbow)
		{
			bool collidingWithRainbow = false;

			for (unsigned int j = 0; j < mRainbows.size(); j++)
			{
				if (Collisions::Instance()->Box(GetCollisionBox(), mRainbows[j]->GetCollisionBox()))
				{
					collidingWithRainbow = true;
					break;
				}
			}

			if (!collidingWithRainbow)
			{
				Vector2D pos = GetPosition();
				pos.x += 10;
				if (GetFacing() == FACING_RIGHT)
					pos.x += GetCollisionBox().width - 15;
				pos.y -= GetCollisionBox().height*0.3f;
				CreateRainbow(pos, GetRainbowsAllowed());

				mCanSpawnRainbow = false;
			}
		}
		else if (!VirtualJoypad::Instance()->DownArrow)
		{
			mCanSpawnRainbow = true;
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

void CharacterBub::CreateRainbow(Vector2D position, int numberOfRainbows)
{
	double xOffset = 0.0;
	float  spwnDelay = 0.0f;
	do
	{
		if (GetFacing() == FACING_LEFT)
			position.x -= xOffset;
		else
			position.x += xOffset;

		CharacterRainbow* rainbowCharacter = new CharacterRainbow(mRenderer, "Images/RainbowIslands/Rainbow.png", position, GetFacing(), spwnDelay);
		if (rainbowCharacter->GetFacing() == FACING_LEFT)
			rainbowCharacter->SetPosition(Vector2D(rainbowCharacter->GetPosition().x - rainbowCharacter->GetCollisionBox().width, rainbowCharacter->GetPosition().y));

		mRainbows.push_back(rainbowCharacter);
		numberOfRainbows--;
		xOffset = rainbowCharacter->GetCollisionBox().width - 10.0;
		spwnDelay += 200;
	} while (numberOfRainbows > 0);
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