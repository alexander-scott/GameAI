//------------------------------------------------------------------------
//  Author: Paul Roberts 2017
//------------------------------------------------------------------------

#ifndef _CHARACTERCATERPILLAR_H
#define _CHARACTERCATERPILLAR_H

#include "Character.h"
#include <iostream>

using namespace::std;

class CharacterCaterpillar : public Character
{
//--------------------------------------------------------------------------------------------------
public:
	CharacterCaterpillar(SDL_Renderer* renderer, string imagePath, LevelMap* map, Vector2D startPosition, FACING startFacing);
	~CharacterCaterpillar();

	void Update(size_t deltaTime, SDL_Event e);
	void Render();

	void MakeAngry();

//--------------------------------------------------------------------------------------------------
private:
	int			  mCurrentFrame;
	float		  mFrameDelay;

	BADDIE_TYPE	  mCurrentMood;
};
#endif //_CHARACTERCATERPILLAR_H