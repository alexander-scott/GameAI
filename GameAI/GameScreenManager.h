//------------------------------------------------------------------------
//  Author: Paul Roberts 2015
//------------------------------------------------------------------------

#ifndef _GAMESCREENMANAGER_H
#define _GAMESCREENMANAGER_H

#include <SDL.h>
#include <vector>
#include "Commons.h"

class GameScreen;

//------------------------------------------------------------------------
class GameScreenManager
{
//------------------------------------------------------------------------
public:
	GameScreenManager(SDL_Renderer* renderer, SCREENS startScreen);
	~GameScreenManager();

	void Render();
	void Update(size_t deltaTime, SDL_Event e);

	void ChangeScreen(SCREENS newScreen);

	bool IsInScreen(SCREENS screen) { return screen == mCurrentScreenID; }

//------------------------------------------------------------------------
private:
	SDL_Renderer* mRenderer;
	GameScreen*   mCurrentScreen;
	SCREENS		  mCurrentScreenID;
};

//------------------------------------------------------------------------
#endif //_GAMESCREENLEVEL1_H