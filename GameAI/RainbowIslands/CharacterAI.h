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

	void GenerateSurroundings(Vector2D enemyLocation, vector<CharacterRainbow*> _mRainbows);
	vector<Vector2D> GetSurroundingPositions() { return mSurroundingPositions; }
	double GetSurrounding(int index);

	vector<double> GetSurroundings() { return mSurroundings; }

private:
	vector<Vector2D>		mSurroundingPositions;
	vector<double>			mSurroundings;
};