//------------------------------------------------------------------------
//  Author: Paul Roberts 2015
//------------------------------------------------------------------------

#ifndef _TEXTURE2D_H
#define _TEXTURE2D_H

#include <SDL.h>
#include <string>
#include "Commons.h"
using namespace::std;

//------------------------------------------------------------------------
class Texture2D
{

//------------------------------------------------------------------------
public:
	Texture2D(SDL_Renderer* renderer);																					//Constructor.
	~Texture2D();																										//Destructor.

	bool LoadFromFile( string path );																					//Load at specified path.
	void Free();																										//Deallocate memory.

	void Render(double x, double y, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0f);						//Render at position [x,y] - Flip the image if necessary.
	void Render(Vector2D newPosition, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0f);
	void Render(SDL_Rect srcRect, SDL_Rect destRect, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0.0f);

	int GetWidth()	{return mWidth;}
	int GetHeight() {return mHeight;}

//------------------------------------------------------------------------
private:
	SDL_Renderer* mRenderer;															//Renderer to render to.
	SDL_Texture*  mTexture;																//SDL internal texture.

	int	 		  mWidth;																//Dimensions.
	int			  mHeight;
};

//------------------------------------------------------------------------
#endif //_TEXTURE2D_H