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
	surroundingPositions.clear();
	surroundings.clear();

	int centralXPosition = (int)GetCentralPosition().x / TILE_WIDTH;
	int centralYPosition = (int)GetCentralPosition().y / TILE_HEIGHT;
	int footPosition = (int)(mPosition.y + mTexture->GetHeight()) / TILE_HEIGHT;

	int startHeight = (int)GetCentralPosition().y - (mSingleSpriteHeight * 2);
	int startWidth = (int)GetCentralPosition().x - (mSingleSpriteWidth * 3);

	int endHeight = (int)GetCentralPosition().y + (mSingleSpriteHeight * 3);
	int endWidth = (int)GetCentralPosition().x + (mSingleSpriteWidth * 4);

	/*
	//LEFT
	int leftPosition;

	leftPosition = (int)mPosition.x / TILE_WIDTH;

	if (mPosition.x == 0 || mPosition.x - mSingleSpriteWidth <= 0)
	surroundings.push_back(1);
	else
	surroundings.push_back(mCurrentLevelMap->GetCollisionTileAt(centralYPosition, leftPosition));

	//RIGHT
	int rightPosition;

	rightPosition = ((int)mPosition.x + mSingleSpriteWidth) / TILE_WIDTH;
	if (mPosition.x + mSingleSpriteWidth >= kRainbowIslandsScreenWidth || mPosition.x + mSingleSpriteWidth <= 0)
	surroundings.push_back(1);
	else
	surroundings.push_back(mCurrentLevelMap->GetCollisionTileAt(centralYPosition, rightPosition));

	//ABOVE
	int upPosition;

	upPosition = (int)mPosition.y / TILE_HEIGHT;
	surroundings.push_back(mCurrentLevelMap->GetCollisionTileAt(upPosition, centralXPosition));

	//BELOW
	int downPosition;

	downPosition = (int)mPosition.y - TILE_HEIGHT / TILE_HEIGHT;
	surroundings.push_back(mCurrentLevelMap->GetCollisionTileAt(downPosition, centralXPosition));
	*/

	for (int height = startHeight; height < endHeight; height += mSingleSpriteHeight)
	{
		for (int width = startWidth; width < endWidth; width += mSingleSpriteWidth)
		{
			surroundingPositions.push_back(Vector2D(width, height));
			if (height < startHeight + (mSingleSpriteHeight * 2))
			{
				if (width == 0 || width <= 0 ||
					width >= kRainbowIslandsScreenWidth || width + mSingleSpriteWidth <= 0 ||
					enemyLocation.Distance(Vector2D(width, height)) < 20)
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
					//cout << var->GetCentralPosition().x << "," << var->GetCentralPosition().y << "		" << width << "," << height << "		= " << abs(var->GetCentralPosition().Distance(Vector2D(width, height))) << endl;
					if (abs(rainbowPosition.Distance(Vector2D(width, height))) < 28)
					{
						surroundings.push_back(1);
						rainbowFound = true;
						break;
					}
				}

				if (rainbowFound)
					continue;
				else {

					if (width == 0 || width <= 0 ||
						width >= kRainbowIslandsScreenWidth || width + mSingleSpriteWidth <= 0 ||
						enemyLocation.Distance(Vector2D(width, height)) < 20)
						surroundings.push_back(1);
					else
						surroundings.push_back(mCurrentLevelMap->GetCollisionTileAt(height / TILE_HEIGHT, width / TILE_WIDTH));
				}
			}
		}
	}

	if (surroundings.size() != 35)
		cout << "ERROR OCCURED: INCORRECT SURROUNDINGS" << endl;
	if (surroundingPositions.size() != 35)
		cout << "ERROR OCCURED: INCORRECT SURROUNDINGS SIZE" << endl;
	return surroundings;
}
