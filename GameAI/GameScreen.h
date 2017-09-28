//------------------------------------------------------------------------
//  Author: Paul Roberts 2015
//------------------------------------------------------------------------

#ifndef _GAMESCREEN_H
#define _GAMESCREEN_H

#include <SDL.h>
#include "Commons.h"

//------------------------------------------------------------------------
class GameScreen
{
//------------------------------------------------------------------------
public:
	GameScreen(SDL_Renderer* renderer);
	~GameScreen();

	virtual void Render();
	virtual void Update(size_t deltaTime, SDL_Event e);

	void DrawDebugCircle(Vector2D centrePoint, double radius, int red, int green, int blue);
	void DrawDebugLine(Vector2D startPoint, Vector2D endPoint, int red, int green, int blue);

//------------------------------------------------------------------------
protected:
	SDL_Renderer* mRenderer;
};

//------------------------------------------------------------------------
#endif //_GAMESCREEN_H