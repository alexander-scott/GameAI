//------------------------------------------------------------------------
//  Author: Paul Roberts 2017
//------------------------------------------------------------------------

#ifndef _GAMESCREEN_RAINBOW_H   
#define _GAMESCREEN_RAINBOW_H

#include "../GameScreen.h"
#include "Commons_RainbowIslands.h"
#include "../Commons.h"
#include <SDL.h>
#include <vector>

#include "CCharacter.h"
#include "CGenAlg.h"
#include "utils.h"

using namespace::std;

class Texture2D;
class Character;
class CharacterBub;
class CharacterFruit;
class CharacterRainbow;
class CharacterChest;
class LevelMap;


class GameScreen_RainbowIslands : GameScreen
{
//--------------------------------------------------------------------------------------------------
public:
	GameScreen_RainbowIslands(SDL_Renderer* renderer);
	~GameScreen_RainbowIslands();

	bool SetUpLevel();
	void Render();
	void RenderBackground();
	void Update(size_t deltaTime, SDL_Event e);

	void UpdateCharacter(size_t deltaTime, SDL_Event e);
	void UpdateCharacterNN(size_t deltaTime, SDL_Event e);

//--------------------------------------------------------------------------------------------------
private:
	void SetLevelMap();
	void RestartLevel();

	void CreateStartingCharacters();
	void CreateStartingCharactersNN();

	void MakeEnemiesAngry();
	void UpdateEnemies(size_t deltaTime, SDL_Event e);
	void CreateCaterpillar(Vector2D position, FACING direction);

	void UpdateRainbows(size_t deltaTime, SDL_Event e);
	void UpdateRainbowsNN(size_t deltaTime, SDL_Event e);
	void CreateRainbow(Vector2D position, int numberOfRainbows);
	void CreateRainbow(Vector2D position, int numberOfRainbows, CCharacter* character);

	void UpdateFruit(size_t deltaTime, SDL_Event e);
	void CreateFruit(Vector2D position, bool bounce);

	void CreateChest(Vector2D position);
	void TriggerChestSpawns();

//--------------------------------------------------------------------------------------------------
private:
	Texture2D*				  mBackgroundTexture;
	CharacterBub*			  mBubCharacter;
	bool					  mCanSpawnRainbow;
	vector<Character*>		  mEnemies;
	vector<CharacterFruit*>	  mFruit;
	vector<CharacterRainbow*> mRainbows;
	CharacterChest*			  mChest;
	LevelMap*				  mLevelMap;

	size_t					  mTimeToCompleteLevel;
	bool					  mTriggeredAnger;

	bool					  mTriggeredChestSpawns;

	//storage for the population of genomes
	vector<SGenome>			m_vecThePopulation;

	//and the character
	vector<CCharacter*>		m_vecCharacters;

	//pointer to the GA
	CGenAlg*		         m_pGA;

	//stores the average fitness per generation for use 
	//in graphing.
	vector<double>		   m_vecAvFitness;

	//stores the best fitness per generation
	vector<double>		   m_vecBestFitness;

	//cycles per generation
	int					m_iTicks;

	//generation counter
	int					m_iGenerations;
};


#endif //_GAMESCREEN_RAINBOW_H