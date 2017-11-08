//------------------------------------------------------------------------
//  Author: Paul Roberts 2017
//------------------------------------------------------------------------

#include "LevelMap.h"

//--------------------------------------------------------------------------------------------------
// When making a map, remember:
// 1 = Collidable Area     0 = Empty Area
//--------------------------------------------------------------------------------------------------

LevelMap::LevelMap(int collisionMap[MAP_HEIGHT][MAP_WIDTH], int backgroundMap[MAP_HEIGHT][MAP_WIDTH])
{
	//Allocate memory for the level map.
	mCollisionMap = new int*[MAP_HEIGHT];
	for(unsigned int i = 0; i < MAP_HEIGHT; i++)
	{
		mCollisionMap[i] = new int[MAP_WIDTH];
	}

	//Populate the array.
	for(unsigned int i = 0; i < MAP_HEIGHT; i++)
	{
		for(unsigned int j = 0; j < MAP_WIDTH; j++)
		{
			mCollisionMap[i][j] = collisionMap[i][j];
		}
	}

	//Allocate memory for the level map.
	mBackgroundMap = new int*[MAP_HEIGHT];
	for (unsigned int i = 0; i < MAP_HEIGHT; i++)
	{
		mBackgroundMap[i] = new int[MAP_WIDTH];
	}

	//Populate the array.
	for (unsigned int i = 0; i < MAP_HEIGHT; i++)
	{
		for (unsigned int j = 0; j < MAP_WIDTH; j++)
		{
			mBackgroundMap[i][j] = backgroundMap[i][j];
		}
	}
}

//--------------------------------------------------------------------------------------------------

LevelMap::~LevelMap()
{
	//Delete all elements of the array.
	for(unsigned int i = 0; i < MAP_HEIGHT; i++)
	{
		delete [] mCollisionMap[i];
	}
	delete [] mCollisionMap;

	for (unsigned int i = 0; i < MAP_HEIGHT; i++)
	{
		delete[] mBackgroundMap[i];
	}
	delete[] mBackgroundMap;
}

//--------------------------------------------------------------------------------------------------

int LevelMap::GetCollisionTileAt(unsigned int row, unsigned int column)
{
	if(row < MAP_HEIGHT && column < MAP_WIDTH)
	{
		return mCollisionMap[row][column];
	}

	return 0;
}

//--------------------------------------------------------------------------------------------------

int LevelMap::GetBackgroundTileAt(unsigned int row, unsigned int column)
{
	if (row < MAP_HEIGHT && column < MAP_WIDTH)
	{
		return mBackgroundMap[row][column];
	}

	return 0;
}

//--------------------------------------------------------------------------------------------------

void LevelMap::ChangeTileAt(unsigned int row, unsigned int column, unsigned int newValue)
{
	mCollisionMap[row][column] = newValue;
}

//--------------------------------------------------------------------------------------------------
