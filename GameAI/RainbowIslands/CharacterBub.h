//------------------------------------------------------------------------
//  Author: Paul Roberts 2017
//------------------------------------------------------------------------

#ifndef _CHARACTERBUB_H
#define _CHARACTERBUB_H

#include "Character.h"
#include <iostream>

using namespace::std;

class CharacterBub : public Character
{

//--------------------------------------------------------------------------------------------------
public:
	CharacterBub(SDL_Renderer* renderer, string imagePath, LevelMap* map, Vector2D startPosition);
	~CharacterBub();

	virtual void AlternateCharacterUpdate(size_t deltaTime, SDL_Event e);  //Required as it is very similar to original, but with RAINBOW tweaks.
	virtual void Update(size_t deltaTime, SDL_Event e);
	virtual void Render();

	void MoveLeft(size_t deltaTime, int yCentreOnImage);
	void MoveRight(size_t deltaTime, int yCentreOnImage);
	void Jump();

	void SetState(CHARACTER_STATE newState);

	bool OnARainbow()						{ return mOnRainbow; }
	void SetOnARainbow(bool yesNo)			{ mOnRainbow = yesNo; }

	void AddPoints()						{ mPoints += 10; }
	int  GetPoints()						{ return mPoints; }

	int	GetRainbowsAllowed();

//--------------------------------------------------------------------------------------------------
private:
	int		mStartFrame;
	int		mEndFrame;
	int		mCurrentFrame;
	float	mFrameDelay;

	bool	mOnRainbow;
	int		mPoints;
};
#endif //_CHARACTERBUB_H