//------------------------------------------------------------------------
//  Author: Paul Roberts 2017
//------------------------------------------------------------------------

#ifndef _CHARACTERRAINBOW_H
#define _CHARACTERRAINBOW_H

#include "Character.h"
#include <iostream>

using namespace::std;

class CharacterRainbow : public Character
{
//--------------------------------------------------------------------------------------------------
public:
	CharacterRainbow(SDL_Renderer* renderer, string imagePath, Vector2D startPosition, FACING startFacing, float delayToSpawn);
	~CharacterRainbow();

	void		Update(size_t deltaTime, SDL_Event e);
	void		Render();

	bool		CanKill()								{ return mCurrentFrame <= 3; } //3 is the frame just before the rainbow is complete.
	Vector2D	GetStrikePosition();

//--------------------------------------------------------------------------------------------------
private:
	int			  mCurrentFrame;
	int			  mSpriteRow;
	float		  mFrameDelay;
};
#endif //_CHARACTERRAINBOW_H