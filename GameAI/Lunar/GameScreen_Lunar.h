//------------------------------------------------------------------------
//  Author: Paul Roberts 2017
//------------------------------------------------------------------------

#ifndef _GAMESCREEN_LUNAR_H
#define _GAMESCREEN_LUNAR_H

#include "../GameScreen.h"
#include "../Commons.h"
#include <SDL.h>
#include "LunarConstants.h"

using namespace::std;

class Texture2D;
class LunarLander;

//-------------------------------
//Take this out for Students
enum LunarAction
{
	LunarAction_None = 0,
	LunarAction_LunarThrust,
	LunarAction_LunarLeft,
	LunarAction_LunarRight,

	LunarAction_MaxActions
};
//-------------------------------

class GameScreen_Lunar : GameScreen
{
//--------------------------------------------------------------------------------------------------
public:
	GameScreen_Lunar(SDL_Renderer* renderer);
	~GameScreen_Lunar();

	void Render();
	void Update(size_t deltaTime, SDL_Event e);

//--------------------------------------------------------------------------------------------------
protected:

	//TODO: Students to code these functions.
	void GenerateRandomChromosomes()					;//{;}
	void ClearChromosomes()								;//{;}

	void UpdateAILanders(size_t deltaTime, SDL_Event e)	;//{;}
	void CalculateFitness()								;//{;}
	
	void Selection()									;//{;}
	void Crossover()									;//{;}
	void Mutation()										;//{;}
	void RestartGA()									;//{;}

	void SaveSolution()									;//{;}

//--------------------------------------------------------------------------------------------------
private:
	Texture2D*	 mPlatform;
	Vector2D	 mPlatformPosition;
	LunarLander* mLunarLander;

	//-----------------------------------
	LunarLander* mAILanders[kNumberOfAILanders];
	LunarAction  mChromosomes[kNumberOfAILanders][kNumberOfChromosomeElements];
	double		 mFitnessValues[kNumberOfAILanders];
	LunarAction  mSelectedAIChromosomes[kNumberOfAILanders][kNumberOfChromosomeElements];
	int			 mCurrentAction;
	size_t		 mAccumulatedDeltaTime;
	int			 mGeneration;
	//-----------------------------------

	bool		 mPause;
	bool		 mAIOn;

	bool		 mAllowMutation;
};

//--------------------------------------------------------------------------------------------------
#endif //_GAMESCREEN_LUNAR_H