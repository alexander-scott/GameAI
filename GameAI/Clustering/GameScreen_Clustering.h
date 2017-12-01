//------------------------------------------------------------------------
//  Author: Paul Roberts 2016
//------------------------------------------------------------------------

#ifndef _GAMESCREEN_CLUSTERING_H
#define _GAMESCREEN_CLUSTERING_H

#include "../GameScreen.h"
#include "../Commons.h"
#include "ClusteringConstants.h"
#include <SDL.h>

using namespace::std;

class Texture2D;

class GameScreen_Clustering : GameScreen
{
//--------------------------------------------------------------------------------------------------
public:
	GameScreen_Clustering (SDL_Renderer* renderer);
	~GameScreen_Clustering ();

	void Render();
	void Update(size_t deltaTime, SDL_Event e);

//--------------------------------------------------------------------------------------------------
private:
	void SetRandomWeights_QtrScreen();
	void SetRandomWeights_HalfScreen();

	void SetTrainingSetsForSquare();
	void SetTrainingSetsForMiniSquares();
	void SetTrainingSetsForTriangle();
	void SetTrainingSetsForCircle();

	void DrawDebugCircle(Vector2D centrePoint, double radius, int red, int green, int blue);
	void DrawDebugLine(Vector2D startPoint, Vector2D endPoint, int red, int green, int blue);

//--------------------------------------------------------------------------------------------------
private:
	bool  mPause;

	float mWeights[kNumberOfInputNodes][kNumberOfClusterNodes];
	float mTrainingData[kNumberOfTrainingSets][kNumberOfInputNodes];

	size_t mAccumulatedDeltaTime;
	float mLearningRate;
	int   mCurrentEpoch;
};


#endif //_GAMESCREEN_CLUSTERING_H