#include "CharacterAI.h"

CharacterAI::CharacterAI(SDL_Renderer * renderer, string imagePath, LevelMap * map, Vector2D startPosition)
	: CharacterBub(renderer, imagePath, map, startPosition)
{
}

CharacterAI::~CharacterAI()
{
}

void CharacterAI::GenerateSurroundings(Vector2D enemyLocation, vector<CharacterRainbow*> _mRainbows)
{
	mSurroundings.clear();
	mSurroundingPositions.clear();

	int centreXPos = (int)GetCentralPosition().x / TILE_WIDTH;
	int centreYPos = (int)GetCentralPosition().y / TILE_HEIGHT;
	int footPosition = (int)(mPosition.y + mTexture->GetHeight()) / TILE_HEIGHT;

	int startHeight = (int)GetCentralPosition().y - (mSingleSpriteHeight * 2);
	int startWidth = (int)GetCentralPosition().x - (mSingleSpriteWidth * 3);

	int endHeight = (int)GetCentralPosition().y + (mSingleSpriteHeight * 3);
	int endWidth = (int)GetCentralPosition().x + (mSingleSpriteWidth * 4);

	// Loop over every tile surrounding the centre tile
	for (int height = startHeight; height < endHeight; height += mSingleSpriteHeight)
	{
		for (int width = startWidth; width < endWidth; width += mSingleSpriteWidth)
		{
			mSurroundingPositions.push_back(Vector2D(width, height));
			if (height < startHeight + (mSingleSpriteHeight * 2))
			{
				if (width == 0 || width <= 0 ||	width >= kRainbowIslandsScreenWidth || width + mSingleSpriteWidth <= 0 || enemyLocation.Distance(Vector2D(width, height)) < 20)
					mSurroundings.push_back(1);
				else
					mSurroundings.push_back(0);
			}
			else
			{
				bool rainbowFound = false;
				for each (CharacterRainbow* var in _mRainbows)
				{
					Vector2D rainbowPosition = Vector2D(var->GetCentralPosition().x, var->GetCentralPosition().y + 12);
					
					if (abs(rainbowPosition.Distance(Vector2D(width, height))) < 28) 
					{
						mSurroundings.push_back(1);
						rainbowFound = true;
						break;
					}
				}

				if (rainbowFound)
				{
					continue;
				}
				else 
				{
					if (width == 0 || width <= 0 ||	width >= kRainbowIslandsScreenWidth || width + mSingleSpriteWidth <= 0 || enemyLocation.Distance(Vector2D(width, height)) < 20)
						mSurroundings.push_back(1);
					else
						mSurroundings.push_back(mCurrentLevelMap->GetCollisionTileAt(height / TILE_HEIGHT, width / TILE_WIDTH));
				}
			}
		}
	}
}

double CharacterAI::GetSurrounding(int index)
{
	if (index < mSurroundings.size())
	{
		return mSurroundings[index];
	}

	return 0.0;
}
