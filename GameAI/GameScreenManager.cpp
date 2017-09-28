//------------------------------------------------------------------------
//  Author: Paul Roberts 2015
//------------------------------------------------------------------------

#include "GameScreenManager.h"
#include "GameScreen.h"
#include "Chess\GameScreen_Chess.h"
#include "Conway\GameScreen_Conway.h"
#include "Lunar\GameScreen_Lunar.h"
#include "Conway\GameScreen_Conway.h"

//--------------------------------------------------------------------------------------------------

GameScreenManager::GameScreenManager(SDL_Renderer* renderer, SCREENS startScreen)
{
	mRenderer = renderer;
	mCurrentScreen = NULL;

	//Ensure the first screen is set up.
	ChangeScreen(startScreen);
}

//--------------------------------------------------------------------------------------------------

GameScreenManager::~GameScreenManager()
{
	mRenderer = NULL;

	delete mCurrentScreen;
	mCurrentScreen = NULL;
}

//--------------------------------------------------------------------------------------------------

void GameScreenManager::Render()
{
	mCurrentScreen->Render();
}

//--------------------------------------------------------------------------------------------------

void GameScreenManager::Update(size_t deltaTime, SDL_Event e)
{
	mCurrentScreen->Update(deltaTime, e);
}

//--------------------------------------------------------------------------------------------------

void GameScreenManager::ChangeScreen(SCREENS newScreen)
{
	//Clear up the old screen.
	if(mCurrentScreen != NULL)
	{
		delete mCurrentScreen;
	}

	GameScreen_Chess*			tempChessScreen;
	GameScreen_Conway*			tempConwayScreen;
	GameScreen_Lunar*			tempLunarScreen;

	//Initialise the new screen.
	mCurrentScreenID = newScreen;
	switch(newScreen)
	{
		case SCREEN_INTRO:
		break;

		case SCREEN_MENU:
		break;

		case SCREEN_CHESS:
			tempChessScreen = new GameScreen_Chess(mRenderer);
			mCurrentScreen  = (GameScreen*)tempChessScreen;
			tempChessScreen = NULL;
		break;
		
		case SCREEN_CONWAY:
			tempConwayScreen = new GameScreen_Conway(mRenderer);
			mCurrentScreen   = (GameScreen*)tempConwayScreen;
			tempConwayScreen = NULL;
		break;

		case SCREEN_LUNAR:
			tempLunarScreen  = new GameScreen_Lunar(mRenderer);
			mCurrentScreen   = (GameScreen*)tempLunarScreen;
			tempLunarScreen  = NULL;
		break;

		case SCREEN_CLUSTERING:
		break;

		case SCREEN_RAINBOWISLANDS:
		break;

		default:
		break;
	}
}

//--------------------------------------------------------------------------------------------------