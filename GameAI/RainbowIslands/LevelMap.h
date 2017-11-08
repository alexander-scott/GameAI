//------------------------------------------------------------------------
//  Author: Paul Roberts 2017
//------------------------------------------------------------------------

#ifndef _LEVELMAP_H
#define _LEVELMAP_H

#include "Constants_RainbowIslands.h"

class LevelMap
{
public:
//--------------------------------------------------------------------------------------------------
	LevelMap(int collisionMap[MAP_HEIGHT][MAP_WIDTH], int backgroundMap[MAP_HEIGHT][MAP_WIDTH]);
	~LevelMap();

	int  GetCollisionTileAt(unsigned int h, unsigned int w);
	int  GetBackgroundTileAt(unsigned int h, unsigned int w);
	void ChangeTileAt(unsigned int row, unsigned int column, unsigned int newValue);

//--------------------------------------------------------------------------------------------------
private:
	int** mCollisionMap;
	int** mBackgroundMap;
};
#endif //_LEVELMAP_H