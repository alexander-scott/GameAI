//------------------------------------------------------------------------
//  Author: Paul Roberts 2015
//------------------------------------------------------------------------

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <iostream>
#include <string>

#include "Constants.h"
#include "Commons.h"
#include "Texture2D.h"
#include "GameScreenManager.h"
#include "Chess\ChessConstants.h"
#include "Conway\ConwayConstants.h"
#include "Lunar\LunarConstants.h"
#include "RainbowIslands\Constants_RainbowIslands.h"
#include "Clustering\ClusteringConstants.h"


using namespace::std;

//-----------------------------------------------------------------------------------------------------
//Local function prototypes.
bool InitSDL();
SDL_Surface* LoadSurface(string path);
void		 LoadMusic(string path);
void CloseSDL();

void Render(size_t percentThroughFrame);
bool Update(size_t deltaTime);

//-----------------------------------------------------------------------------------------------------
//Globals.
SDL_Window*   gWindow    = NULL;
SDL_Renderer* gRenderer  = NULL;
SDL_Surface*  gSurface   = NULL;
Mix_Music*	  gMusic	 = NULL;
Uint32		  gOldTime;

GameScreenManager* gameScreenManager;

//-----------------------------------------------------------------------------------------------------

int main(int argc, char* args[])
{
	//Initialise SDL.
	if(InitSDL())
	{
		//Set up the game screen manager - Start with Level1
		gameScreenManager = new GameScreenManager(gRenderer, SCREEN_CONWAY);// SCREEN_CONWAY);// SCREEN_RAINBOWISLANDS);// SCREEN_LUNAR);//SCREEN_CHESS);
		
		//Start the music.
		//LoadMusic("Music/.mp3");
		if(Mix_PlayingMusic() == 0)
		{
			Mix_PlayMusic(gMusic, -1);
		}

		bool quit = false;
		gOldTime = SDL_GetTicks();
		size_t currentTime = 0;
		size_t AccumulatedElapsedTime = 0;
		size_t nextUpdate = 0;

		//Game Loop.
		while(!quit)
		{
			currentTime = SDL_GetTicks();
			//cout << "CurrentTime: " << currentTime << endl;

			size_t elapsedTime = currentTime - gOldTime;
			size_t interpolationPercent = 0;	

			//We want Chess to process as fast as possible.
			if (gameScreenManager->IsInScreen(SCREEN_CHESS)) //DO NOT CHANGE
			{
				//Variable step update.
				quit = Update(elapsedTime);
			}
			else
			{
				//Fixed step update - 16ms per update = 1000/60 = milliseconds in a second / fps
				while (currentTime - nextUpdate >= MS_PER_UPDATE)
				{
					quit = Update(MS_PER_UPDATE);
					nextUpdate += MS_PER_UPDATE;

					//cout << "Update - NextUpdate: " << nextUpdate << endl;
				}
			}

			gOldTime = currentTime;
			interpolationPercent = ((currentTime - nextUpdate) * 100) / MS_PER_UPDATE;
			//cout << "Interpolation Percent: " << interpolationPercent << endl;

			Render(interpolationPercent);
			
		}	
	}

	//Close Window and free resources.
	CloseSDL();

	return 0;
}

//-----------------------------------------------------------------------------------------------------

bool InitSDL()
{	
	//Setup SDL.
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		cout << "SDL did not initialise. Error: " << SDL_GetError();
		return false;
	}
	else
	{
		//Attempt to set texture filtering to linear.
		if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			cout << "Warning: Linear texture filtering not available";
		}

		//All good, so attempt to create the window.
		gWindow = SDL_CreateWindow("Game AI", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, kConwayScreenWidth, kConwayScreenHeight, SDL_WINDOW_SHOWN);
		//Did the window get created?
		if(gWindow != NULL)
		{
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if(gRenderer != NULL)
			{
				//Initialise PNG loading.
				int imageFlags = IMG_INIT_PNG;
				if(!(IMG_Init(imageFlags) & imageFlags))
				{
					cout << "SDL_Image could not initialise. Error: " << IMG_GetError();
					return false;
				}
			}
			else
			{
				cout << "Renderer could not initialise. Error: " << SDL_GetError();
				return false;
			}
			
/*			//Initialise the Mixer.
			if(Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0)
			{
				cout << "Mixer could not initialise. Error: " << Mix_GetError();
				return false;
			}*/
		}
		else
		{
			//Nope.
			cout << "Window was not created. Error: " << SDL_GetError();
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------------------------------

SDL_Surface* LoadSurface(string path)
{
	SDL_Surface* pLoadedSurface = NULL;
	SDL_Surface* pOptimizedSurface = NULL;

	//Load the image.
	pLoadedSurface = IMG_Load(path.c_str());
	if(!pLoadedSurface)
	{
		cout << "Failed to load image. Error: " << SDL_GetError();
		return NULL;
	}
	else
	{
		//Convert the surface to the screen format.
		pOptimizedSurface = SDL_ConvertSurface( pLoadedSurface, gSurface->format, NULL);
		if(!pOptimizedSurface)
		{
			cout << "Unable to optimize the surface. Error: " << SDL_GetError();
			return NULL;
		}

		//Free the memory used for the loaded surface.
		SDL_FreeSurface(pLoadedSurface);
	}

	return pOptimizedSurface;
}

//-----------------------------------------------------------------------------------------------------

void LoadMusic(string path)
{
	gMusic = Mix_LoadMUS(path.c_str());
	if(!gMusic)
	{
		cout << "Failed to load background music! Error: " << Mix_GetError() << endl;
	}
}

//-----------------------------------------------------------------------------------------------------

void CloseSDL()
{
	//Destroy the game screen manager.
	delete gameScreenManager;
	gameScreenManager = NULL;

	//Destroy the window.
	//SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;
	//SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Release music.
	Mix_FreeMusic(gMusic);
	gMusic = NULL;

	//Quit SDL subsystems.
	IMG_Quit();
	SDL_Quit();
}

//-----------------------------------------------------------------------------------------------------

void Render(size_t percentThroughFrame)
{
	//Clear the screen.
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);

	gameScreenManager->Render();

	//Update the screen.
	SDL_RenderPresent(gRenderer);
}

//-----------------------------------------------------------------------------------------------------

bool Update(size_t deltaTime)
{
	//Event Handler.
	SDL_Event e;

	//Get the events.
	SDL_PollEvent(&e);
	
	//Handle any input events.
	switch (e.type)
	{
		//Click the 'X' to quit.
		case SDL_QUIT:
			return true;
		break;
	}

	
	//Finally do the update.
	//cout << elapsedTime << endl;
	gameScreenManager->Update(deltaTime, e);

	return false;
}

//-----------------------------------------------------------------------------------------------------