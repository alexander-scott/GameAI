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

#include "RainbowIslands\CharacterBub.h"
#include "phenotype.h" 
#include "utils.h" 
#include "C2DMatrix.h" 
#include "CParams.h" 
#include "CMapper.h" 

using namespace std;

class CharacterAI : public CharacterBub
{

private:

	CNeuralNet*  m_pItsBrain;

	//its memory 
	CMapper     m_MemoryMap;

	//to store output from the ANN 
	double			m_lTrack, m_rTrack;

	//the sweepers fitness score.  
	double			m_dFitness;


public:


	CharacterAI(SDL_Renderer* renderer, string imagePath, LevelMap* map, Vector2D startPosition);

	//updates the ANN with information from the sweepers enviroment 
	bool			Update(vector<SPoint> &objects);

	void			Reset();

	void			EndOfRunCalculations();

	float			Fitness()const { return m_dFitness; }

	int             NumCellsVisited() { return m_MemoryMap.NumCellsVisited(); }

	void            InsertNewBrain(CNeuralNet* brain) { m_pItsBrain = brain; }

};


#endif 