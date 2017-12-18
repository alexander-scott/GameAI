#pragma once

#include "CharacterBub.h"
#include "CharacterRainbow.h"

#include "Constants_RainbowIslands.h"
#include "LevelMap.h"

#include <vector>

using namespace::std;

class CharacterAI : public CharacterBub
{
public:
	CharacterAI(SDL_Renderer* renderer, string imagePath, LevelMap* map, Vector2D startPosition);
	~CharacterAI();

	vector<double> GetSurroundings(Vector2D enemyLocation, vector<CharacterRainbow*> _mRainbows);

	vector<Vector2D> surroundingPositions;
	vector<double> surroundings;
};