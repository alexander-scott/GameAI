//------------------------------------------------------------------------
//  Author: Paul Roberts 2016
//------------------------------------------------------------------------

#include "GameScreen_ConwayGame.h"
#include <stdlib.h>
#include "../Texture2D.h"
#include "ConwayGameConstants.h"
#include "../TinyXML/tinyxml.h"

//--------------------------------------------------------------------------------------------------

GameScreen_ConwayGame::GameScreen_ConwayGame(SDL_Renderer* renderer) : GameScreen(renderer)
{
	mMap = new int*[kConwayGame_ActualBoardWidth];
	for(int i =0; i < kConwayGame_ActualBoardWidth; i++)
		mMap[i] = new int[kConwayGame_ActualBoardHeight];

	//Get all required textures.
	mWhiteTile = new Texture2D(renderer);
	mWhiteTile->LoadFromFile("Images/ConwayGame/WhiteTile.png");
	mBlackTile = new Texture2D(renderer);
	mBlackTile->LoadFromFile("Images/ConwayGame/BlackTile.png");

	//Start map has 50% active cells.
	CreateRandomMap(75);

	//Start game in frozen state.
	mPause = true;

	mAccumulatedDeltaTime = 0;
	mCurrentDelay = 2000.0f;
}

//--------------------------------------------------------------------------------------------------

GameScreen_ConwayGame::~GameScreen_ConwayGame()
{
	delete mWhiteTile;
	mWhiteTile = NULL;

	delete mBlackTile;
	mBlackTile = NULL;

	for(int i =0; i < kConwayGame_ActualBoardWidth; i++)
	{
		delete mMap[i];
		mMap[i] = NULL;
	}
	delete mMap;
	mMap = NULL;
}

//--------------------------------------------------------------------------------------------------

void GameScreen_ConwayGame::Render()
{
	for(int x = 0; x < kConwayGame_ActualBoardWidth; x++)
	{
		for(int y = 0; y < kConwayGame_ActualBoardHeight; y++)
		{
			switch(mMap[x][y])
			{
				case 0:
					mBlackTile->Render(Vector2D(x*kConwayGame_TileDimensions,y*kConwayGame_TileDimensions));
				break;
				
				case 1:
					mWhiteTile->Render(Vector2D(x*kConwayGame_TileDimensions,y*kConwayGame_TileDimensions));
				break;
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------

void GameScreen_ConwayGame::Update(size_t deltaTime, SDL_Event e)
{
	//Additional input outside of player.
	switch(e.type)
	{
		//Deal with mouse click input.
		case SDL_KEYUP:
			switch(e.key.keysym.sym)
			{
				case SDLK_SPACE:
					mPause = !mPause;
				break;

				case SDLK_r:
					mPause = true;
					CreateRandomMap(75);

					mAccumulatedDeltaTime = 0;
				break;

				case SDLK_LEFT:
					mCurrentDelay += 200.0f;
				break;

				case SDLK_RIGHT:
					mCurrentDelay -= 200.0f;
				break;
			}
		break;
	
		default:
		break;
	}
	
	mAccumulatedDeltaTime += deltaTime;
	if (mAccumulatedDeltaTime > mCurrentDelay)
	{
		mAccumulatedDeltaTime = 0;
		if (!mPause)
			UpdateMap();
	}
}

//--------------------------------------------------------------------------------------------------

void GameScreen_ConwayGame::UpdateMap()
{
	//Rules
	//1. Any living cell with less than 2 live neighbour dies.
	//2. Any living cell with 2 or 3 live cells lives on to next generation.
	//3. Any live cell with more than 3 live neighbours dies.
	//4. Any dead cell with exactly 3 live neighbours becomes a living cell.

	//First count the neightbours.
	int neighbours[kConwayGame_ActualBoardWidth][kConwayGame_ActualBoardHeight];
	for(int x = 0; x < kConwayGame_ActualBoardWidth; x++)
	{
		for(int y = 0; y < kConwayGame_ActualBoardHeight; y++)
		{
			//Default to zero.
			neighbours[x][y] = 0;

			//To the left of current cell.
			if(x-1 >= 0)
				neighbours[x][y] += mMap[x-1][y];

			//To the right of current cell.
			if(x+1 < kConwayGame_ActualBoardWidth)
				neighbours[x][y] += mMap[x+1][y];

			//Below current cell.
			if(y-1 >= 0)
				neighbours[x][y] += mMap[x][y-1];

			//Above current cell.
			if(y+1 < kConwayGame_ActualBoardHeight)
				neighbours[x][y] += mMap[x][y+1];

			//Above Left
			if(x-1 >= 0 && y-1 >= 0)
				neighbours[x][y] += mMap[x-1][y-1];

			//Above Right
			if(x+1 < kConwayGame_ActualBoardWidth && y-1 >= 0)
				neighbours[x][y] += mMap[x+1][y-1];

			//Below Left
			if(x-1 >= 0 && y+1 < kConwayGame_ActualBoardHeight)
				neighbours[x][y] += mMap[x-1][y+1];

			//Below Right
			if(x+1 < kConwayGame_ActualBoardWidth && y+1 < kConwayGame_ActualBoardHeight)
				neighbours[x][y] += mMap[x+1][y+1];
			
		}
	}

	//Next go through and apply the above rules.
	for(int x = 0; x < kConwayGame_ActualBoardWidth; x++)
	{
		for(int y = 0; y < kConwayGame_ActualBoardHeight; y++)
		{
			//1. Any living cell with less than 2 live neighbour dies.
			if(mMap[x][y] == 1 && neighbours[x][y] < 2)
				mMap[x][y] = 0;

			//2. Any living cell with 2 or 3 live cells lives on to next generation.
			if(mMap[x][y] == 1 && (neighbours[x][y] == 2 || neighbours[x][y] == 3))
				mMap[x][y] = 1;

			//3. Any live cell with more than 3 live neighbours dies.
			if(mMap[x][y] == 1 && neighbours[x][y] > 3)
				mMap[x][y] = 0;
			
			//4. Any dead cell with exactly 3 live neighbours becomes a living cell.
			if(mMap[x][y] == 0 && neighbours[x][y] == 3)
				mMap[x][y] = 1;
		}
	}
}

//--------------------------------------------------------------------------------------------------

void GameScreen_ConwayGame::CreateRandomMap(int percentageInactive)
{
	int randomNumber = 0;

	for(int x = 0; x < kConwayGame_ActualBoardWidth; x++)
	{
		for(int y = 0; y < kConwayGame_ActualBoardHeight; y++)
		{
			randomNumber = (rand() % 100)+1;

			if(randomNumber > percentageInactive)
				mMap[x][y] = 1;
			else
				mMap[x][y] = 0;
		}
	}
}

//--------------------------------------------------------------------------------------------------
