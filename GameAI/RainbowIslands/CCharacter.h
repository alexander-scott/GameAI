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

public:

	CCharacter(SDL_Renderer* renderer, string imagePath, LevelMap* map, Vector2D startPosition);

	//updates the ANN with information from the sweepers enviroment
	bool			Update(int yPositionToComplete, vector<Character*> enemies, vector<CharacterFruit*> fruit);
	virtual void	Update(size_t deltaTime, SDL_Event e);

	//returns a vector to the closest fruit
	Vector2D		GetClosestFruit(vector<CharacterFruit*> &objects);

	//returns a vector away from the closest enemy
	Vector2D		GetClosestEnemy(vector<Character*> &objects);

	void			Reset();


	//-------------------accessor functions
	void			IncrementFitness() { ++m_dFitness; }

	void			DecrementFitness() { --m_dFitness; }

	double			Fitness()const { return m_dFitness; }

	void			PutWeights(vector<double> &w) { m_ItsBrain.PutWeights(w); }

	int				GetNumberOfWeights()const {
		return m_ItsBrain.GetNumberOfWeights(); 
	}

	bool			SpawnARainbow() { return m_spawnRainbow; }
	void			RainbowSpawned() { m_spawnRainbow = false; }
};


#endif


