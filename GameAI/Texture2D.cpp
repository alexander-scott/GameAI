//------------------------------------------------------------------------
//  Author: Paul Roberts 2015
//------------------------------------------------------------------------

#include "Texture2D.h"
#include <iostream>
#include <SDL_image.h>

using namespace::std;
//--------------------------------------------------------------------------------------------------

Texture2D::Texture2D(SDL_Renderer* renderer)
{
	//Initialise member variables.
	mRenderer = renderer;
	mTexture  = NULL;
	mWidth    = 0;
	mHeight   = 0;
}

//--------------------------------------------------------------------------------------------------

Texture2D::~Texture2D()
{
	//Free up the memory.
	Free();

	mRenderer = NULL;
}

//--------------------------------------------------------------------------------------------------

bool Texture2D::LoadFromFile( string path )
{
	//Remove the memory used for a previous texture.
	Free();

	SDL_Texture* pTexture = NULL;

	//Load the image.
	SDL_Surface* pSurface = IMG_Load( path.c_str() );
	if(pSurface != NULL)
	{
		//Colour key the image - The colour to be transparent.
		SDL_SetColorKey(pSurface, SDL_TRUE, SDL_MapRGB(pSurface->format, 0, 0xFF, 0xFF));

		//Create the texture from the pixels on the surface.
		pTexture = SDL_CreateTextureFromSurface(mRenderer, pSurface);
		if(pTexture != NULL)
		{
			//Set the dimensions.
			mWidth  = pSurface->w;
			mHeight = pSurface->h;
		}
		else
		{
			cout << "Unable to create texture from surface. Error: " << IMG_GetError() << endl;
		}

		//Remove the loaded surface now that we have the texture.
		SDL_FreeSurface(pSurface);
	}
	else
	{
		cout << "Image not loaded. Error: " << SDL_GetError() << endl;
	}

	//Set the internal texture.
	mTexture = pTexture;

	//Return whether the process was successful.
	return mTexture != NULL;
}

//--------------------------------------------------------------------------------------------------

void Texture2D::Free()
{
	//Check if the texture exists before removing it.
	if(mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth   = 0;
		mHeight  = 0;
	}
}

//--------------------------------------------------------------------------------------------------

void Texture2D::Render(double x, double y, SDL_RendererFlip flip, double angle)
{
	//Set where to render the texture.
	SDL_Rect renderLocation = {(int)x, (int)y, mWidth, mHeight};

	//Render to screen.
	SDL_RenderCopyEx(mRenderer, mTexture, NULL, &renderLocation, angle, NULL, flip);
}

//--------------------------------------------------------------------------------------------------

void Texture2D::Render(Vector2D newPosition, SDL_RendererFlip flip, double angle)
{
	Render(newPosition.x, newPosition.y, flip, angle);
}

//--------------------------------------------------------------------------------------------------

void Texture2D::Render(SDL_Rect srcRect, SDL_Rect destRect, SDL_RendererFlip flip, double angle)
{
	//Render to screen.
	SDL_RenderCopyEx(mRenderer, mTexture, &srcRect, &destRect, angle, NULL, flip);
}

//--------------------------------------------------------------------------------------------------