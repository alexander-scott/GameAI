//------------------------------------------------------------------------
//  Author: Paul Roberts 2017
//------------------------------------------------------------------------

#ifndef _CHARACTERFRUIT_H
#define _CHARACTERFRUIT_H

#include "Character.h"
#include <iostream>

using namespace::std;

class CharacterFruit : public Character
{
//--------------------------------------------------------------------------------------------------
public:
	CharacterFruit(SDL_Renderer* renderer, string imagePath, LevelMap* map, Vector2D startPosition, FRUIT_TYPE type);
	~CharacterFruit();

	void Update(size_t deltaTime, SDL_Event e);
	void Render();

	void Jump();

//--------------------------------------------------------------------------------------------------
private:
	FRUIT_TYPE	  mFruitType;
	double		  mAngle;
};
#endif //_CHARACTERFRUIT_H