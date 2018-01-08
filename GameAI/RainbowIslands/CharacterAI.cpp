#include "CharacterAI.h"

CharacterAI::CharacterAI(SDL_Renderer * renderer, string imagePath, LevelMap * map, Vector2D startPosition)
	: CharacterBub(renderer, imagePath, map, startPosition)
{
}

CharacterAI::~CharacterAI()
{
}

vector<double> CharacterAI::GetSurroundings(Vector2D enemyLocation, vector<CharacterRainbow*> _mRainbows)
{
	vector<double> surroundings;

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
			if (height < startHeight + (mSingleSpriteHeight * 2))
			{
				if (width == 0 || width <= 0 ||	width >= kRainbowIslandsScreenWidth || width + mSingleSpriteWidth <= 0 || enemyLocation.Distance(Vector2D(width, height)) < 20)
					surroundings.push_back(1);
				else
					surroundings.push_back(0);
			}
			else
			{
				bool rainbowFound = false;
				for each (CharacterRainbow* var in _mRainbows)
				{
					Vector2D rainbowPosition = Vector2D(var->GetCentralPosition().x, var->GetCentralPosition().y + 12);
					
					if (abs(rainbowPosition.Distance(Vector2D(width, height))) < 28) 
					{
						surroundings.push_back(1);
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
						surroundings.push_back(1);
					else
						surroundings.push_back(mCurrentLevelMap->GetCollisionTileAt(height / TILE_HEIGHT, width / TILE_WIDTH));
				}
			}
		}
	}

	if (surroundings.size() != 35)
		cout << "ERROR OCCURED: INCORRECT SURROUNDINGS" << endl;

	return surroundings;
}
