//------------------------------------------------------------------------
//  Author: Paul Roberts 2015
//------------------------------------------------------------------------

#include "GameScreen.h"

//--------------------------------------------------------------------------------------------------

GameScreen::GameScreen(SDL_Renderer* renderer)
{
	mRenderer = renderer;
}

//--------------------------------------------------------------------------------------------------

GameScreen::~GameScreen()
{
	mRenderer = NULL;
}

//--------------------------------------------------------------------------------------------------

void GameScreen::Render()
{
}

//--------------------------------------------------------------------------------------------------

void GameScreen::Update(size_t deltaTime, SDL_Event e)
{
}

//--------------------------------------------------------------------------------------------------
//Debug output - Use to determine where you are checking
//--------------------------------------------------------------------------------------------------

void GameScreen::DrawDebugCircle(Vector2D centrePoint, double radius, int red, int green, int blue)
{
#ifdef DEBUG_LINES_VISIBLE 
	Vector2D polarVec(0.0f, radius);

	float stepSize = 0.02f;
	float _360DegAsRads = (float)DegsToRads(360.0f);

	while (polarVec.x < _360DegAsRads)
	{
		Vector2D polarAsCart(polarVec.y * cos(polarVec.x), polarVec.y * sin(polarVec.x));
		Vector2D drawPoint(centrePoint.x + polarAsCart.x, centrePoint.y + polarAsCart.y);

		SDL_SetRenderDrawColor(mRenderer, red, green, blue, 255);
		SDL_RenderDrawPoint(mRenderer, (int)drawPoint.x, (int)drawPoint.y);

		polarVec.x += stepSize;
	}
#endif
}

//--------------------------------------------------------------------------------------------------

void GameScreen::DrawDebugLine(Vector2D startPoint, Vector2D endPoint, int red, int green, int blue)
{
#ifdef DEBUG_LINES_VISIBLE 
	SDL_SetRenderDrawColor(mRenderer, red, green, blue, 255);
	SDL_RenderDrawLine(mRenderer, (int)startPoint.x, (int)startPoint.y, (int)endPoint.x, (int)endPoint.y);
#endif
}

//--------------------------------------------------------------------------------------------------