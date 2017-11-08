//------------------------------------------------------------------------
//  Author: Paul Roberts 2017
//------------------------------------------------------------------------

#ifndef _CHARACTERCHEST_H
#define _CHARACTERCHEST_H

#include "Character.h"
#include <iostream>

using namespace::std;

class CharacterChest : public Character
{
//--------------------------------------------------------------------------------------------------
public:
	CharacterChest(SDL_Renderer* renderer, string imagePath, LevelMap* map, Vector2D startPosition);
	~CharacterChest();

	void Update(size_t deltaTime, SDL_Event e);
	void Render();

	bool IsChestOpen()									{ return mOpen; }

//--------------------------------------------------------------------------------------------------
private:
	void OpenChest();

//--------------------------------------------------------------------------------------------------
private:
	bool mOpen;
	int  mCurrentFrame;
};
//--------------------------------------------------------------------------------------------------
#endif //_CHARACTERCHEST_H