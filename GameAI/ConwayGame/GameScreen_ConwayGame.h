//------------------------------------------------------------------------
//  Author: Paul Roberts 2017
//------------------------------------------------------------------------

#ifndef _GAMESCREEN_CONWAYGAME_H
#define _GAMESCREEN_CONWAYGAME_H

#include "../GameScreen.h"
#include "../Commons.h"
#include <SDL.h>

using namespace::std;

class Texture2D;

class GameScreen_ConwayGame : GameScreen
{
//--------------------------------------------------------------------------------------------------
public:
	GameScreen_ConwayGame(SDL_Renderer* renderer);
	~GameScreen_ConwayGame();

	void Render();
	void Update(size_t deltaTime, SDL_Event e);

//--------------------------------------------------------------------------------------------------
protected:

	//TODO: Students to code these functions.
	void UpdateMap()								;//{;}
	void CreateRandomMap(int percentageInactive)	;//{;}

//--------------------------------------------------------------------------------------------------
private:
	int**		mMap;

	Texture2D*	mWhiteTile;
	Texture2D*	mBlackTile;

	bool		mPause;

	size_t		mAccumulatedDeltaTime;
	float		mCurrentDelay;
};


#endif //_GAMESCREEN_CONWAYGAME_H