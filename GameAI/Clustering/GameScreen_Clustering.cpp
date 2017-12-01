//------------------------------------------------------------------------
//  Author: Paul Roberts 2016
//------------------------------------------------------------------------

#include "GameScreen_Clustering.h"
#include <stdlib.h>
#include "../Texture2D.h"
#include "ClusteringConstants.h"
#include "../TinyXML/tinyxml.h"
#include <time.h>
#include "../Commons.h"

//--------------------------------------------------------------------------------------------------

GameScreen_Clustering::GameScreen_Clustering(SDL_Renderer* renderer) : GameScreen(renderer)
{
	srand(unsigned int(time(NULL)));
	SetRandomWeights_QtrScreen(); 
	//SetRandomWeights_HalfScreen();
	//SetTrainingSetsForMiniSquares();
	//SetTrainingSetsForSquare();
	SetTrainingSetsForTriangle();
	mAccumulatedDeltaTime = 0;
	mLearningRate		  = 0.9f;
	mCurrentEpoch		  = 1;
	mPause				  = true;
}

//--------------------------------------------------------------------------------------------------

GameScreen_Clustering::~GameScreen_Clustering()
{
}

//--------------------------------------------------------------------------------------------------

void GameScreen_Clustering::Render()
{
	//TODO: Render the board here.
	DrawDebugLine(Vector2D(0.0, kClusteringScreenHeight_Half), Vector2D(kClusteringScreenWidth, kClusteringScreenHeight_Half), 0, 0, 0);
	DrawDebugLine(Vector2D(kClusteringScreenWidth_Half, 0.0), Vector2D(kClusteringScreenWidth_Half, kClusteringScreenHeight), 0, 0, 0);

	if (!mPause)
	{
		//Draw the Training nodes.
		for (int j = 0; j < kNumberOfTrainingSets; j++)
		{
			DrawDebugCircle(Vector2D(mTrainingData[j][0], mTrainingData[j][1]), 2.0, 0, 0, 255);
		}
	}

	//Draw the cluster nodes.
	for(int j = 0; j < kNumberOfClusterNodes; j++)
	{
		DrawDebugCircle(Vector2D(mWeights[0][j], mWeights[1][j]), 2.0, 255, 0, 0);
	}

	if (mPause)
	{
		for (int i = 0; i < kNumberOfClusterNodes; i++)
		{
			//Find the closest node.
			for (int j = 0; j < kNumberOfClusterNodes; j++)
			{
				Vector2D currentVector(mWeights[0][j] - mWeights[0][i], mWeights[1][j] - mWeights[1][i]);
				double currentDistance = currentVector.Length();
				if (currentDistance > 0.0f && currentDistance < 60.0)
				{
					//Draw the line to the nearest node.
					DrawDebugLine(Vector2D(mWeights[0][i], mWeights[1][i]), Vector2D(mWeights[0][j], mWeights[1][j]), 255, 0, 0);
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------

void GameScreen_Clustering::Update(size_t deltaTime, SDL_Event e)
{
	//Additional input outside of player.
	switch (e.type)
	{
		case SDL_KEYUP:
			switch (e.key.keysym.sym)
			{
				case SDLK_SPACE:
					mPause = !mPause;
					if (mPause)
						cout << "Paused" << endl;
					else
						cout << "Unpaused" << endl;
				break;

				default:
				break;
			}
		break;

		default:
		break;
	}

	if (!mPause)
	{
		mAccumulatedDeltaTime += deltaTime;
		if (mAccumulatedDeltaTime > 1)
		{
			cout << "Epoch: " << mCurrentEpoch++ << endl;
			cout << "Learning Rate: " << mLearningRate << endl;
			cout << "--------------------" << endl;

			//Reset the count.
			mAccumulatedDeltaTime = 0;
			double closestDistance = 999999.9;
			int   closestIndex = -1;

			//Do the calculations - For this example radius is set to zero.
			for (int i = 0; i < kNumberOfTrainingSets; i++)
			{
				closestDistance = 999999.9;

				//Find the closest node.
				for (int j = 0; j < kNumberOfClusterNodes; j++)
				{
					Vector2D currentVector(mWeights[0][j] - mTrainingData[i][0], mWeights[1][j] - mTrainingData[i][1]);
					double currentDistance = currentVector.Length();
					if (currentDistance < closestDistance)
					{
						closestDistance = currentDistance;
						closestIndex = j;
					}
				}

				//Move the closest node.
				mWeights[0][closestIndex] += mLearningRate*(mTrainingData[i][0] - mWeights[0][closestIndex]);
				mWeights[1][closestIndex] += mLearningRate*(mTrainingData[i][1] - mWeights[1][closestIndex]);
			}

			//Decrease the learning rate.
			if (mLearningRate > 0.2f)
				mLearningRate -= 0.0005f;
			else
				mLearningRate -= 0.00005f;
			mLearningRate = max(mLearningRate, 0.00005f);

			//Get a new set of training data.
			//SetTrainingSetsForMiniSquares();
			//SetTrainingSetsForSquare();
			SetTrainingSetsForTriangle();
		}
	}
}

//--------------------------------------------------------------------------------------------------

void GameScreen_Clustering::SetRandomWeights_QtrScreen()
{
	for(int i = 0; i < kNumberOfInputNodes; i++)
	{
		for(int j = 0; j < kNumberOfClusterNodes; j++)
		{
			mWeights[i][j] = ((float)(rand()%100)/100.0f)-0.5f;
		}
	}

	//Scale up to the board size - TEST: SQUARE
	for (int j = 0; j < kNumberOfClusterNodes; j++)
	{
		//X Component.
		mWeights[0][j] = kClusteringScreenWidth_Half + (mWeights[0][j] * kClusteringScreenWidth_Qtr);

		//Y Component.
		mWeights[1][j] = kClusteringScreenHeight_Half + (mWeights[1][j] * kClusteringScreenHeight_Qtr);
	}
}

//--------------------------------------------------------------------------------------------------

void GameScreen_Clustering::SetRandomWeights_HalfScreen()
{
	for (int i = 0; i < kNumberOfInputNodes; i++)
	{
		for (int j = 0; j < kNumberOfClusterNodes; j++)
		{
			mWeights[i][j] = ((float)(rand() % 100) / 100.0f) - 0.5f;
		}
	}

	//Scale up to the board size - TEST: 4 SQUARES
	for (int j = 0; j < kNumberOfClusterNodes; j++)
	{
		//X Component.
		mWeights[0][j] = kClusteringScreenWidth_Half + (mWeights[0][j] * kClusteringScreenWidth_Half);

		//Y Component.
		mWeights[1][j] = kClusteringScreenHeight_Half + (mWeights[1][j] * kClusteringScreenHeight_Half);
	}
}

//--------------------------------------------------------------------------------------------------

void GameScreen_Clustering::SetTrainingSetsForSquare()
{
	//TEST: SQUARE
	for (int i = 0; i < kNumberOfTrainingSets; i++)
	{
		for (int j = 0; j < kNumberOfInputNodes; j++)
		{
			mTrainingData[i][j] = ((float)(rand() % 100) / 100.0f) - 0.5f;
		}
	}

	//Scale up to the size of the square we are using as training data.
	for (int j = 0; j < kNumberOfTrainingSets; j++)
	{
		//X Component.
		mTrainingData[j][0] = kClusteringScreenWidth_Half + (mTrainingData[j][0] * kClusteringScreenWidth_Half);

		//Y Component.
		mTrainingData[j][1] = kClusteringScreenHeight_Half + (mTrainingData[j][1] * kClusteringScreenHeight_Half);
	}
}

//--------------------------------------------------------------------------------------------------

void GameScreen_Clustering::SetTrainingSetsForMiniSquares()
{
	//TEST: 4 SQUARES
	for (int i = 0; i < kNumberOfTrainingSets; i++)
	{
		for (int j = 0; j < kNumberOfInputNodes; j++)
		{
			mTrainingData[i][j] = ((float)(rand() % 100) / 100.0f);
		}
	}

	int numberOfTrainingSetQtr = kNumberOfTrainingSets / 4;

	for (int j = 0; j < numberOfTrainingSetQtr; j++)
	{
		//X Component.
		mTrainingData[j][0] = kClusteringScreenWidth_Qtr/2 + (mTrainingData[j][0] * kClusteringScreenWidth_Qtr);

		//Y Component.
		mTrainingData[j][1] = kClusteringScreenHeight_Qtr/2+ (mTrainingData[j][1] * kClusteringScreenHeight_Qtr);
	}

	for (int j = numberOfTrainingSetQtr; j < numberOfTrainingSetQtr*2; j++)
	{
		//X Component.
		mTrainingData[j][0] = kClusteringScreenWidth_Half+kClusteringScreenWidth_Qtr/2 + (mTrainingData[j][0] * kClusteringScreenWidth_Qtr);

		//Y Component.
		mTrainingData[j][1] = kClusteringScreenHeight_Half+kClusteringScreenHeight_Qtr/2 + (mTrainingData[j][1] * kClusteringScreenHeight_Qtr);
	}

	for (int j = numberOfTrainingSetQtr*2; j < numberOfTrainingSetQtr * 3; j++)
	{
		//X Component.
		mTrainingData[j][0] = kClusteringScreenWidth_Half + kClusteringScreenWidth_Qtr / 2 + (mTrainingData[j][0] * kClusteringScreenWidth_Qtr);

		//Y Component.
		mTrainingData[j][1] = kClusteringScreenHeight_Qtr / 2 + (mTrainingData[j][1] * kClusteringScreenHeight_Qtr);
	}

	for (int j = numberOfTrainingSetQtr*3; j < kNumberOfTrainingSets; j++)
	{
		//X Component.
		mTrainingData[j][0] = kClusteringScreenWidth_Qtr / 2 + (mTrainingData[j][0] * kClusteringScreenWidth_Qtr);

		//Y Component.
		mTrainingData[j][1] = kClusteringScreenHeight_Half + kClusteringScreenHeight_Qtr / 2 + (mTrainingData[j][1] * kClusteringScreenHeight_Qtr);
	}
}

//--------------------------------------------------------------------------------------------------

void GameScreen_Clustering::SetTrainingSetsForTriangle()
{
	Vector2D origin = Vector2D();
	Vector2D AB = Vector2D(kClusteringScreenWidth_Half, -kClusteringScreenHeight_Qtr);
	Vector2D AC = Vector2D(kClusteringScreenWidth_Half, kClusteringScreenHeight_Qtr);

	//TEST: TRIANGLE
	for (int i = 0; i < kNumberOfTrainingSets; i++)
	{
		float r = (float)(rand() % 100) / 100.0f;
		float s = (float)(rand() % 100) / 100.0f;

		if (r + s >= 1.0f)
		{
			r = 1.0f - r;
			s = 1.0f - s;
		}

		Vector2D point = origin + ((AB*r) + (AC*s));
		mTrainingData[i][0] = kClusteringScreenWidth_Qtr + (float)point.x;
		mTrainingData[i][1] = kClusteringScreenHeight_Half + (float)point.y;
	}
}

//--------------------------------------------------------------------------------------------------

void GameScreen_Clustering::SetTrainingSetsForCircle()
{

}

//--------------------------------------------------------------------------------------------------

void GameScreen_Clustering::DrawDebugCircle(Vector2D centrePoint, double radius, int red, int green, int blue)
{ 
	Vector2D polarVec(0.0f, radius);

	float stepSize = 0.02f;
	float _360DegAsRads = (float)DegsToRads(360.0f);

	while (polarVec.x < _360DegAsRads)
	{
		Vector2D polarAsCart(polarVec.y * cos(polarVec.x), polarVec.y * sin(polarVec.x));
		Vector2D drawPoint(centrePoint.x + polarAsCart.x, centrePoint.y + polarAsCart.y);

		SDL_SetRenderDrawColor(mRenderer, red, green, blue, 255);
		SDL_RenderDrawPoint(mRenderer, (int)drawPoint.x, (int)drawPoint.y);

		polarVec.x += stepSize;
	}
}

//--------------------------------------------------------------------------------------------------

void GameScreen_Clustering::DrawDebugLine(Vector2D startPoint, Vector2D endPoint, int red, int green, int blue)
{
	SDL_SetRenderDrawColor(mRenderer, red, green, blue, 255);
	SDL_RenderDrawLine(mRenderer, (int)startPoint.x, (int)startPoint.y, (int)endPoint.x, (int)endPoint.y);
}

//--------------------------------------------------------------------------------------------------