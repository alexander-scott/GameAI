//------------------------------------------------------------------------
//  Author: Paul Roberts 2017
//------------------------------------------------------------------------

#include "GameScreen_Lunar.h"
#include <stdlib.h>
#include "../Texture2D.h"
#include "LunarConstants.h"
#include "LunarLander.h"
#include <time.h>
#include <fstream>

//--------------------------------------------------------------------------------------------------

GameScreen_Lunar::GameScreen_Lunar(SDL_Renderer* renderer) : GameScreen(renderer)
{
	srand(static_cast <unsigned> (time(0)));

	//Set up the platform to land on.
	mPlatform = new Texture2D(renderer);
	mPlatform->LoadFromFile("Images/Lunar/Platform.png");

	//Create a lunar lander to use for tests.
	mPlatformPosition = Vector2D(198,430);

	//Player controlled lander.
	mLunarLander = new LunarLander(renderer, Vector2D(400,125), Rect2D(mPlatformPosition.x, mPlatformPosition.y, mPlatform->GetWidth(), mPlatform->GetHeight()));

	//AI controlled landers.
	mGeneration = 0;
	GenerateRandomChromosomes();
	for(int i = 0; i < kNumberOfAILanders; i++)
		mAILanders[i] = new LunarLander(renderer, Vector2D(400,125), Rect2D(mPlatformPosition.x, mPlatformPosition.y, mPlatform->GetWidth(), mPlatform->GetHeight()));
	RestartGA();
	mAllowMutation = true;
	mMutationRate = kStartMutationRate;
	mAccumulatedDeltaTime = 0;
	//-------------------------------------

	//Start game in frozen state.
	mPause = true;

	//Start game in player control.
	mAIOn = false;
}

//--------------------------------------------------------------------------------------------------

GameScreen_Lunar::~GameScreen_Lunar()
{
	delete mPlatform;
	mPlatform = NULL;

	delete mLunarLander;
	mLunarLander = NULL;

	for(int i = 0; i < kNumberOfAILanders; i++)
		delete *mAILanders;
}

//--------------------------------------------------------------------------------------------------

void GameScreen_Lunar::Render()
{
	mPlatform->Render(mPlatformPosition);

	if(mAIOn)
	{
		for(int i = 0; i < kNumberOfAILanders; i++)
			mAILanders[i]->Render();
	}
	else
		mLunarLander->Render();
}

//--------------------------------------------------------------------------------------------------

void GameScreen_Lunar::Update(size_t deltaTime, SDL_Event e)
{
	//Additional input outside of player.
	switch(e.type)
	{
		//Deal with mouse click input.
		case SDL_KEYUP:
			switch(e.key.keysym.sym)
			{
				case SDLK_SPACE:
					mPause = !mPause;
					if(mPause)
						cout << "Paused" << endl;
					else
						cout << "Unpaused" << endl;
				break;

				case SDLK_a:
					mAIOn = !mAIOn;
					if(mAIOn)
						cout << "AI on" << endl;
					else
						cout << "AI off" << endl;
				break;

				case SDLK_LEFT:
					if(!mAIOn)
						mLunarLander->TurnLeft(deltaTime);
				break;

				case SDLK_RIGHT:
					if(!mAIOn)
						mLunarLander->TurnRight(deltaTime);
				break;

				case SDLK_UP:
					if(!mAIOn)
						mLunarLander->Thrust();
				break;

				case SDLK_r:
					if (!mAIOn)
					{
						mLunarLander->Reset();
						return;
					}
				break;
			}
		break;
	
		default:
		break;
	}
	
	if(!mPause)
	{
		mAccumulatedDeltaTime += deltaTime;
		if (mAccumulatedDeltaTime > 50)
		{
			//Reset the count.
			mAccumulatedDeltaTime = 0;

			if (!mAIOn)
				mLunarLander->Update(deltaTime, e);
			else
				UpdateAILanders(deltaTime, e);
		}
	}
}

//--------------------------------------------------------------------------------------------------

void GameScreen_Lunar::GenerateRandomChromosomes()
{
	for(int lander = 0; lander < kNumberOfAILanders; lander++)
	{
		for(int action = 0; action < kNumberOfChromosomeElements; action++)
		{
			mChromosomes[lander][action] = (LunarAction)(rand()%LunarAction_MaxActions);
		}
	}
}

//--------------------------------------------------------------------------------------------------

void GameScreen_Lunar::ClearChromosomes()
{
	for(int lander = 0; lander < kNumberOfAILanders; lander++)
	{
		for(int action = 0; action < kNumberOfChromosomeElements; action++)
		{
			mChromosomes[lander][action] = LunarAction_None;
		}
	}
}

//--------------------------------------------------------------------------------------------------

void GameScreen_Lunar::UpdateAILanders(size_t deltaTime, SDL_Event e)
{
	int numberDeadOrLanded = 0;

	for (int i = 0; i < kNumberOfAILanders; i++)
	{
		mAILanders[i]->Update(deltaTime, e);

		if (!mAILanders[i]->IsAlive() || mAILanders[i]->HasLanded())
			numberDeadOrLanded++;
	}

	//Are all finished?
	if (numberDeadOrLanded == kNumberOfAILanders)
	{
		int landedAICount = 0;
		for (int i = 0; i < kNumberOfAILanders; i++)
		{
			if (mAILanders[i]->HasLanded())
			{
				landedAICount++;
			}
		}

		cout << landedAICount << " landers landed out of " << kNumberOfAILanders << endl;

		if (landedAICount == 100) // Simulation complete
		{
			mPause = true;
			SaveSolution();
		}
		else
		{
			// Update the mutation rate
			if (landedAICount > 15)
				mMutationRate = 0;
			else if (landedAICount > 0)
				mMutationRate /= (3 * landedAICount);
			else if (landedAICount == 0)
			{
				if (mMutationRate < kStartMutationRate)
					mMutationRate += 50;
			}

			CalculateFitness();
		}

		return;
	}

	//Move on to the next action in the Chromosomes.
	mCurrentAction++;
	if (mCurrentAction >= kNumberOfChromosomeElements)
		return;

	//Assign all actions to landers.
	for (int i = 0; i < kNumberOfAILanders; i++)
	{
		switch (mChromosomes[i][mCurrentAction])
		{
			case LunarAction_LunarLeft:
				mAILanders[i]->TurnLeft(deltaTime);
			break;

			case LunarAction_LunarRight:
				mAILanders[i]->TurnRight(deltaTime);
			break;

			case LunarAction_LunarThrust:
				mAILanders[i]->Thrust();
			break;
		}
	}
}
//--------------------------------------------------------------------------------------------------

// Give each lander a score based on distance from platform centre and time taken to reach platform. Higher score is better
void GameScreen_Lunar::CalculateFitness()
{
	// Variable used to determine to total fitness of all landers this generation to see if the overall fitness is improving round by round
	double totalFitness = 0;

	for (int i = 0; i < kNumberOfAILanders; i++)
	{
		double x_diff = mAILanders[i]->GetCentralPosition().x - mPlatformPosition.x;
		double y_diff = mAILanders[i]->GetCentralPosition().y - mPlatformPosition.y;
		double rotAngle = mAILanders[i]->GetRotationAngle();
		double survivalTime = mAILanders[i]->GetSurvivalTime();
		double speed = mAILanders[i]->GetSpeed();

		double disFit = kLunarScreenWidth - std::sqrt(x_diff * x_diff + y_diff * y_diff);
		double rotFit = 500 * (1 / (std::abs(rotAngle) + 1));
		double speedFit = survivalTime / (speed + 1);

		if (mAILanders[i]->IsAlive()) // If the lander survived
		{
			// The shortest time to reach the platform is best
			mFitnessValues[i] = 10000 - survivalTime;
		}
		else
		{
			// A mixture of closest distance to platform, closest rotation to 0, time survived/speed
			mFitnessValues[i] = (disFit) + (rotFit) + (speedFit);
		}

		totalFitness += mFitnessValues[i];
	}

	cout << "Total fitness: " << std::to_string(totalFitness) << endl;

	Selection();
}

//--------------------------------------------------------------------------------------------------

// Select the top 'kChromosomesToEvolve' landers based on their score
void GameScreen_Lunar::Selection()
{
	int randLanderIndex = 0;
	int highestFitnessIndex = 0;

	for (int iLander = 0; iLander < kNumberOfAILanders; iLander++) // For every lander
	{
		highestFitnessIndex = rand() % kNumberOfAILanders; // Find a random lander index

		for (int i = 0; i < kLandersToEvolve; i++) // This loop decides the number of landers that will be competing to find the highest fitness
		{
			randLanderIndex = rand() % kNumberOfAILanders; 

			if (mFitnessValues[highestFitnessIndex] < mFitnessValues[randLanderIndex]) // Find the highest fitness in 'kLandersToEvolve' landers
			{
				highestFitnessIndex = randLanderIndex;
			}
		}

		for (int i = 0; i < kNumberOfChromosomeElements; i++) // Add the fittest chromsomes to mSelectedAIChromosomes
		{
			mSelectedAIChromosomes[iLander][i] = mChromosomes[highestFitnessIndex][i];
		}
	}

	Crossover();
}

//--------------------------------------------------------------------------------------------------

// Choose two random landers from the new pool. Combine half and half each of the landers chromosomes to create a new lander. 
// Fill the rest of the new chromosomes with a mix of genes from the high scoring landers.
void GameScreen_Lunar::Crossover()
{
	if (mMutationRate < 1)
	{
		for (int i = 0; i < kNumberOfAILanders; i += 2)
		{
			for (int j = 0; j < kNumberOfAILanders / 2; j++)
			{
				mChromosomes[i][j] = mSelectedAIChromosomes[i][j];
				mChromosomes[i + 1][j] = mSelectedAIChromosomes[i + 1][j];
			}

			for (int j = kNumberOfAILanders / 2; j < kNumberOfAILanders; j++)
			{
				mChromosomes[i][j] = mSelectedAIChromosomes[i + 1][j];
				mChromosomes[i + 1][j] = mSelectedAIChromosomes[i][j];
			}
		}
	}
	else
	{
		for (int i = 0; i < kNumberOfAILanders; i++)
		{
			int randomIndex1 = (rand() % kLandersToEvolve);
			int randomIndex2 = (rand() % kLandersToEvolve);

			// Half the chromosomes will be come from randomIndex1
			for (int action = 0; action < kNumberOfChromosomeElements / 2; action++)
			{
				mChromosomes[i][action] = mSelectedAIChromosomes[randomIndex1][action];
			}

			// Half the chromosomes will be come from randomIndex2
			for (int action = kNumberOfChromosomeElements / 2; action < kNumberOfChromosomeElements; action++)
			{
				mChromosomes[i][action] = mSelectedAIChromosomes[randomIndex2][action];
			}
		}
	}
	
	Mutation();
}

//--------------------------------------------------------------------------------------------------

// Randomly change a single gene in the landers.
void GameScreen_Lunar::Mutation()
{
	for (int i = 0; i < kNumberOfAILanders; i++)
	{
		if (rand() % 10000 < mMutationRate)
		{
			mChromosomes[i][(rand() % kNumberOfChromosomeElements)] = (LunarAction)(rand() % LunarAction_MaxActions);
		}
	}

	RestartGA();
}

//--------------------------------------------------------------------------------------------------

void GameScreen_Lunar::RestartGA()
{
	if(mGeneration >= 500)
	{
		GenerateRandomChromosomes();
		mGeneration = 0;
		mAllowMutation = true;
		cout << endl << "---FAILED (Restart)---" << endl << endl;
	}

	for(int i = 0; i < kNumberOfAILanders; i++)
	{
		mFitnessValues[i] = 0.0f;
		mAILanders[i]->Reset();
	}

	mAccumulatedDeltaTime = 0;
	mCurrentAction		  = 0;
	//mPause				  = true;
	//cout << "Paused" << endl;
	mGeneration++;
	cout << "---GENERATION " << mGeneration << "---" << endl;
}

//--------------------------------------------------------------------------------------------------

void GameScreen_Lunar::SaveSolution()
{
	//Output the solution of the first chromosome - They should all match at the point we output so it
	//shouldn't matter which chromosome is used for the solution.
	ofstream outFile("Solution.txt");
	if(outFile.is_open())
	{
		for(int action = 0; action < kNumberOfChromosomeElements; action++)
		{
			switch(mChromosomes[0][action])
			{
				case LunarAction_None:
					outFile << "0";
				break;
			
				case LunarAction_LunarThrust:
					outFile << "1";
				break;

				case LunarAction_LunarLeft:
					outFile << "2";
				break;

				case LunarAction_LunarRight:
					outFile << "3";
				break;
			}
		
			outFile << ",";
		}

		outFile << endl << "Generations taken: " << mGeneration;
		outFile << endl << "Platform start (" << mPlatformPosition.x << "," << mPlatformPosition.y << ")";
		outFile << endl << "Ship start (" << mAILanders[0]->GetStartPosition().x << "," << mAILanders[0]->GetStartPosition().y << ")";

		outFile.close();
	}
}

//--------------------------------------------------------------------------------------------------
