#ifndef CMINESWEEPER_H
#define CMINESWEEPER_H

//------------------------------------------------------------------------
//
//	Name: CMineSweeper.h
//
//  Author: Mat Buckland 2002
//
//  Desc: Class to create a minesweeper object 
//
//------------------------------------------------------------------------
#include <vector>
#include <math.h>

#include "CNeuralNet.h"
#include "utils.h"
#include "Constants_RainbowIslands.h"
#include "LevelMap.h"

#include "CharacterBub.h"
#include "CharacterFruit.h"

using namespace std;


class CCharacter : public CharacterBub
{

private:

	//the minesweeper's neural net
	CNeuralNet		m_ItsBrain;

	//the sweeper's fitness score 
	double			m_dFitness;

	//start position
	Vector2D		m_startPosition;

	bool			m_spawnRainbow;

	int				m_previousInput;

	vector<Vector2D> surroundingPositions;
	vector<double> surroundings;

public:

	CCharacter(SDL_Renderer* renderer, string imagePath, LevelMap* map, Vector2D startPosition);

	//updates the ANN with information from the sweepers enviroment
	bool			UpdateNN(int yPositionToComplete, vector<Character*> enemies, vector<CharacterFruit*> fruit);

	//returns a vector to the closest fruit
	CharacterFruit*		GetClosestFruit(vector<CharacterFruit*> &objects);

	//returns a vector away from the closest enemy
	Character*		GetClosestEnemy(vector<Character*> &objects);

	vector<double> GetSurroundings(Vector2D enemyLocation, vector<CharacterRainbow*> _mRainbows);

	void			Reset();


	//-------------------accessor functions
	void			IncrementFitness() { ++m_dFitness; }

	void			DecrementFitness() { --m_dFitness; }

	double			Fitness()const { return m_dFitness; }

	void			PutWeights(vector<double> &w) { m_ItsBrain.PutWeights(w); }

	int				GetNumberOfWeights()const {	return m_ItsBrain.GetNumberOfWeights(); }

	bool			SpawnARainbow() { return m_spawnRainbow; }
	void			RainbowSpawned() { m_spawnRainbow = false; }
};


#endif


