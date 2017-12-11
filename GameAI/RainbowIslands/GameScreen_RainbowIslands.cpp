//------------------------------------------------------------------------
//  Author: Paul Roberts 2017
//------------------------------------------------------------------------

#include "GameScreen_RainbowIslands.h"
#include <iostream>
#include "LevelMap.h"
#include "Character.h"
#include "CharacterBub.h"
#include "CharacterCaterpillar.h"
#include "CharacterRainbow.h"
#include "CharacterFruit.h"
#include "CharacterChest.h"
#include "Commons_RainbowIslands.h"
#include "Constants_RainbowIslands.h"
#include "../Texture2D.h"
#include "../Collisions.h"
#include "VirtualJoypad.h"
using namespace::std;

int RainbowOffsets[] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,22,22,23,23,
						 23,22,22,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0, };
//--------------------------------------------------------------------------------------------------

GameScreen_RainbowIslands::GameScreen_RainbowIslands(SDL_Renderer* renderer) : GameScreen(renderer)
{
	srand(NULL);
	mLevelMap = NULL;

	m_iTicks = 0;

	SetUpLevel();
}

//--------------------------------------------------------------------------------------------------

GameScreen_RainbowIslands::~GameScreen_RainbowIslands()
{
	//Background image.
	delete mBackgroundTexture;
	mBackgroundTexture = NULL;

	if (!USE_NEURAL_NETWORK)
	{
		//Player character.
		delete mBubCharacter;
		mBubCharacter = NULL;
	}
	else
	{
		for (int i = 0; i < kNumOfCharacters; i++)
		{
			delete m_vecCharacters[i];
			m_vecCharacters[i] = NULL;
		}
	}
	
	//Level map.
	delete mLevelMap;
	mLevelMap = NULL;

	//Treasure chest.
	delete mChest;
	mChest = NULL;

	//Enemies.
	mEnemies.clear();

	//Fruit.
	mFruit.clear();

	//Rainbows.
	mRainbows.clear();

	if (m_pGA)
	{
		delete	m_pGA;
	}
}

//--------------------------------------------------------------------------------------------------

void GameScreen_RainbowIslands::Render()
{
	//Draw the background.
	RenderBackground();

	//Draw the chest.
	if (mChest != NULL)
		mChest->Render();

	//Draw the Fruit.
	for (unsigned int i = 0; i < mFruit.size(); i++)
	{
		mFruit[i]->Render();
	}

	//Draw the Enemies.
	for(unsigned int i = 0; i < mEnemies.size(); i++)
	{
		mEnemies[i]->Render();
		DrawDebugCircle(mEnemies[i]->GetCentralPosition(), mEnemies[i]->GetCollisionRadius(), 255, 0, 0);
	}

	//Draw the Rainbows.
	for (unsigned int i = 0; i < mRainbows.size(); i++)
	{
		mRainbows[i]->Render();
		if(mRainbows[i]->CanKill())
			DrawDebugCircle(mRainbows[i]->GetStrikePosition(), mRainbows[i]->GetCollisionRadius(), 255, 255, 255);
	}

	if (!USE_NEURAL_NETWORK)
	{
		//Draw the player.
		mBubCharacter->Render();
		DrawDebugCircle(mBubCharacter->GetCentralPosition(), mBubCharacter->GetCollisionRadius(), 0, 255, 0);
	}
	else
	{
		for (int i = 0; i < kNumOfCharacters; i++)
		{
			m_vecCharacters[i]->Render();
			DrawDebugCircle(m_vecCharacters[i]->GetCentralPosition(), m_vecCharacters[i]->GetCollisionRadius(), 0, 255, 0);
		}
	}
}

//--------------------------------------------------------------------------------------------------

void GameScreen_RainbowIslands::RenderBackground()
{
	if (mLevelMap != NULL)
	{
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			for (int y = 0; y < MAP_HEIGHT; y++)
			{
				//Get the portion of the spritesheet you want to draw.
				int num = mLevelMap->GetBackgroundTileAt(y, x);
				int w = TILE_WIDTH*(num % kTileSpriteSheetWidth);
				int h = TILE_HEIGHT*(num / kTileSpriteSheetWidth);
				//								{XPos, YPos, WidthOfSingleSprite, HeightOfSingleSprite}
				SDL_Rect portionOfSpritesheet = { w, h, TILE_WIDTH, TILE_HEIGHT };

				//Determine where you want it drawn.
				SDL_Rect destRect = { x*TILE_WIDTH, kRainbowIslandsScreenHeight-((MAP_HEIGHT-y)*TILE_HEIGHT), TILE_WIDTH, TILE_HEIGHT };

				//Then draw it.
				mBackgroundTexture->Render(portionOfSpritesheet, destRect, SDL_FLIP_NONE);
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------

void GameScreen_RainbowIslands::Update(size_t deltaTime, SDL_Event e)
{
	//--------------------------------------------------------------------------------------------------
	//Update the Virtual Joypad.
	//--------------------------------------------------------------------------------------------------
	VirtualJoypad::Instance()->SetJoypadState(e);

	//--------------------------------------------------------------------------------------------------
	//Update the level time.
	//--------------------------------------------------------------------------------------------------
	mTimeToCompleteLevel -= deltaTime;
	if (mTimeToCompleteLevel < TRIGGER_ANGRY_TIME && !mTriggeredAnger)
	{
		MakeEnemiesAngry();
		if (mTimeToCompleteLevel <= 0)
			RestartLevel();
	}

	if (USE_NEURAL_NETWORK)
	{
		UpdateCharacterNN(deltaTime, e);
		UpdateRainbowsNN(deltaTime, e);
	}
	else 
	{
		UpdateCharacter(deltaTime, e);
		UpdateFruit(deltaTime, e);
		UpdateEnemies(deltaTime, e);
		UpdateRainbows(deltaTime, e);
	}

	//We have a chest, but is it open yet.
	if (mChest != NULL)
	{
		mChest->Update(deltaTime, e);
		if (mChest->IsChestOpen() && !mTriggeredChestSpawns)
		{
			TriggerChestSpawns();
		}
	}

	if (USE_NEURAL_NETWORK)
	{
		for (int i = 0; i < kNumOfCharacters; i++)
		{
			if (m_vecCharacters[i]->SpawnARainbow())
			{
				bool collidingWithRainbow = false;

				for (unsigned int j = 0; j < mRainbows.size(); j++)
				{
					if (Collisions::Instance()->Box(m_vecCharacters[i]->GetCollisionBox(), mRainbows[j]->GetCollisionBox()))
					{
						collidingWithRainbow = true;
						break;
					}
				}

				if (!collidingWithRainbow)
				{
					Vector2D pos = m_vecCharacters[i]->GetPosition();
					pos.x += 10;
					if (m_vecCharacters[i]->GetFacing() == FACING_RIGHT)
						pos.x += m_vecCharacters[i]->GetCollisionBox().width - 15;
					pos.y -= m_vecCharacters[i]->GetCollisionBox().height*0.3f;
					CreateRainbow(pos, m_vecCharacters[i]->GetRainbowsAllowed(), m_vecCharacters[i]);

					m_vecCharacters[i]->RainbowSpawned();
				}
			}
		}
	}
	else 
	{
		//--------------------------------------------------------------------------------------------------
		//Check if we need to create a new rainbow.
		//--------------------------------------------------------------------------------------------------
		if (VirtualJoypad::Instance()->DownArrow && mCanSpawnRainbow)
		{
			bool collidingWithRainbow = false;

			for (unsigned int i = 0; i < mRainbows.size(); i++)
			{
				if (Collisions::Instance()->Box(mBubCharacter->GetCollisionBox(), mRainbows[i]->GetCollisionBox()))
				{
					collidingWithRainbow = true;
					break;
				}
			}

			if (!collidingWithRainbow)
			{
				Vector2D pos = mBubCharacter->GetPosition();
				pos.x += 10;
				if (mBubCharacter->GetFacing() == FACING_RIGHT)
					pos.x += mBubCharacter->GetCollisionBox().width - 15;
				pos.y -= mBubCharacter->GetCollisionBox().height*0.3f;
				CreateRainbow(pos, mBubCharacter->GetRainbowsAllowed());

				mCanSpawnRainbow = false;
			}
		}
		else if (!VirtualJoypad::Instance()->DownArrow)
		{
			mCanSpawnRainbow = true;
		}
	}
}

void GameScreen_RainbowIslands::UpdateCharacter(size_t deltaTime, SDL_Event e)
{
	//--------------------------------------------------------------------------------------------------
	//Update the player.
	//--------------------------------------------------------------------------------------------------
	mBubCharacter->Update(deltaTime, e);
	if (!mBubCharacter->GetAlive())
		RestartLevel();
	else if (mBubCharacter->GetCentralPosition().y < Y_POSITION_TO_COMPLETE)
		CreateChest(Vector2D(kRainbowIslandsScreenWidth*0.25f, -50.0f));
}

void GameScreen_RainbowIslands::UpdateCharacterNN(size_t deltaTime, SDL_Event e)
{
	//run the sweepers through CParams::iNumTicks amount of cycles. During
	//this loop each sweepers NN is constantly updated with the appropriate
	//information from its surroundings. The output from the NN is obtained
	//and the sweeper is moved. If it encounters a mine its fitness is
	//updated appropriately,
	if (m_iTicks++ < kNumTicks)
	{
		for (int iChar = 0; iChar < kNumOfCharacters; ++iChar)
		{
			//update the NN and position
			m_vecCharacters[iChar]->Update(Y_POSITION_TO_COMPLETE, mEnemies, mFruit);

			//--------------------------------------------------------------------------------------------------
			//Update the Fruit.
			//--------------------------------------------------------------------------------------------------
			if (!mFruit.empty())
			{
				int fruitIndexToDelete = -1;
				for (unsigned int iFruit = 0; iFruit < mFruit.size(); iFruit++)
				{
					//check if the player has collided with it.
					if (Collisions::Instance()->Circle(mFruit[iFruit]->GetCentralPosition(), mFruit[iFruit]->GetCollisionRadius(), m_vecCharacters[iChar]->GetCentralPosition(), m_vecCharacters[iChar]->GetCollisionRadius()))
					{
						mFruit[iFruit]->SetAlive(false);
						fruitIndexToDelete = iFruit;
					}
				}

				//--------------------------------------------------------------------------------------------------
				//Remove a dead fruit - 1 each update.
				//--------------------------------------------------------------------------------------------------
				if (fruitIndexToDelete != -1)
				{
					m_vecCharacters[iChar]->AddPoints();
					m_vecCharacters[iChar]->IncrementFitness();

					Character* toDelete = mFruit[fruitIndexToDelete];
					mFruit.erase(mFruit.begin() + fruitIndexToDelete);
					delete toDelete;
					toDelete = NULL;
				}
			}

			if (!mEnemies.empty())
			{
				for (unsigned int iEnemy = 0; iEnemy < mEnemies.size(); iEnemy++)
				{
					//Check to see if the enemy collides with the player.
					if (Collisions::Instance()->Circle(mEnemies[iEnemy]->GetCentralPosition(), mEnemies[iEnemy]->GetCollisionRadius(), 
						m_vecCharacters[iChar]->GetCentralPosition(), m_vecCharacters[iChar]->GetCollisionRadius()))
					{
						m_vecCharacters[iChar]->SetState(CHARACTERSTATE_PLAYER_DEATH);
						m_vecCharacters[iChar]->DecrementFitness();
					}
				}
			}



			//update the chromos fitness score
			m_vecThePopulation[iChar].dFitness = m_vecCharacters[iChar]->Fitness();

		}
	}

	//Another generation has been completed.

	//Time to run the GA and update the sweepers with their new NNs
	else
	{
		//update the stats to be used in our stat window
		m_vecAvFitness.push_back(m_pGA->AverageFitness());
		m_vecBestFitness.push_back(m_pGA->BestFitness());

		//increment the generation counter
		++m_iGenerations;

		//reset cycles
		m_iTicks = 0;

		//run the GA to create a new population
		m_vecThePopulation = m_pGA->Epoch(m_vecThePopulation);

		RestartLevel();
		return;
	}

	for (int i = 0; i < kNumOfCharacters; i++)
	{
		m_vecCharacters[i]->Update(deltaTime, e);
	}

	bool allDead = false;
	for (int i = 0; i < kNumOfCharacters; i++)
	{
		allDead = !m_vecCharacters[i]->GetAlive();
		if (!allDead)
			break;
	}

	if (allDead)
	{
		RestartLevel();
	}

	CreateChest(Vector2D(kRainbowIslandsScreenWidth*0.25f, -50.0f));

	if (!mFruit.empty())
	{
		for (unsigned int iFruit = 0; iFruit < mFruit.size(); iFruit++)
		{
			//Update the fruit.
			mFruit[iFruit]->Update(deltaTime, e);
		}
	}

	if (!mEnemies.empty())
	{
		int enemyIndexToDelete = -1;
		for (unsigned int iEnemy = 0; iEnemy < mEnemies.size(); iEnemy++)
		{
			mEnemies[iEnemy]->Update(deltaTime, e);

			//If the enemy is no longer alive, then schedule it for deletion.
			if (!mEnemies[iEnemy]->GetAlive())
			{
				enemyIndexToDelete = iEnemy;
			}
		}

		//--------------------------------------------------------------------------------------------------
		//Remove a dead enemies - 1 each update.
		//--------------------------------------------------------------------------------------------------
		if (enemyIndexToDelete != -1)
		{
			Character* toDelete = mEnemies[enemyIndexToDelete];

			//We need a pickup to be generated.
			CreateFruit(toDelete->GetPosition(), true);

			mEnemies.erase(mEnemies.begin() + enemyIndexToDelete);
			delete toDelete;
			toDelete = NULL;
		}
	}
}

//--------------------------------------------------------------------------------------------------

void GameScreen_RainbowIslands::MakeEnemiesAngry()
{
	if (!mEnemies.empty())
	{
		int enemyIndexToDelete = -1;
		for (unsigned int i = 0; i < mEnemies.size(); i++)
		{
			mEnemies[i]->MakeAngry();
		}
	}
}

//--------------------------------------------------------------------------------------------------

void GameScreen_RainbowIslands::UpdateEnemies(size_t deltaTime, SDL_Event e)
{
	//--------------------------------------------------------------------------------------------------
	//Update the Enemies.
	//--------------------------------------------------------------------------------------------------
	if(!mEnemies.empty())
	{
		int enemyIndexToDelete = -1;
		for(unsigned int i = 0; i < mEnemies.size(); i++)
		{
			mEnemies[i]->Update(deltaTime, e);

			//Check to see if the enemy collides with the player.
			if (Collisions::Instance()->Circle(mEnemies[i]->GetCentralPosition(), mEnemies[i]->GetCollisionRadius(), mBubCharacter->GetCentralPosition(), mBubCharacter->GetCollisionRadius()))
			{
				mBubCharacter->SetState(CHARACTERSTATE_PLAYER_DEATH);
			}

			//If the enemy is no longer alive, then schedule it for deletion.
			if(!mEnemies[i]->GetAlive())
			{
				enemyIndexToDelete = i;
			}
		}

		//--------------------------------------------------------------------------------------------------
		//Remove a dead enemies - 1 each update.
		//--------------------------------------------------------------------------------------------------
		if(enemyIndexToDelete != -1)
		{
			Character* toDelete = mEnemies[enemyIndexToDelete];

			//We need a pickup to be generated.
			CreateFruit(toDelete->GetPosition(), true);

			mEnemies.erase(mEnemies.begin()+enemyIndexToDelete);
			delete toDelete;
			toDelete = NULL;
		}
	}
}

//--------------------------------------------------------------------------------------------------

void GameScreen_RainbowIslands::UpdateFruit(size_t deltaTime, SDL_Event e)
{
	//--------------------------------------------------------------------------------------------------
	//Update the Fruit.
	//--------------------------------------------------------------------------------------------------
	if(!mFruit.empty())
	{
		int fruitIndexToDelete = -1;
		for(unsigned int i = 0; i < mFruit.size(); i++)
		{
			//Update the fruit.
			mFruit[i]->Update(deltaTime, e);

			//check if the player has collided with it.
			if (Collisions::Instance()->Circle(mFruit[i]->GetCentralPosition(), mFruit[i]->GetCollisionRadius(), mBubCharacter->GetCentralPosition(), mBubCharacter->GetCollisionRadius()))
			{
				mFruit[i]->SetAlive(false);
				fruitIndexToDelete = i;
			}
		}

		//--------------------------------------------------------------------------------------------------
		//Remove a dead fruit - 1 each update.
		//--------------------------------------------------------------------------------------------------
		if(fruitIndexToDelete != -1)
		{
			mBubCharacter->AddPoints();

			Character* toDelete = mFruit[fruitIndexToDelete];
			mFruit.erase(mFruit.begin() + fruitIndexToDelete);
			delete toDelete;
			toDelete = NULL;
		}
	}
}

//--------------------------------------------------------------------------------------------------

void GameScreen_RainbowIslands::UpdateRainbows(size_t deltaTime, SDL_Event e)
{
	//--------------------------------------------------------------------------------------------------
	//Update the Rainbows.
	//--------------------------------------------------------------------------------------------------
	
	//Need to turn flag off each frame incase the rainbow disapated.
	mBubCharacter->SetOnARainbow(false);

	if (!mRainbows.empty())
	{
		int rainbowIndexToDelete = -1;

		for (unsigned int i = 0; i < mRainbows.size(); i++)
		{
			int xPosition = (int)mBubCharacter->GetPosition().x + (int)(mBubCharacter->GetCollisionBox().width*0.5f);
			int footPosition = (int)(mBubCharacter->GetPosition().y + mBubCharacter->GetCollisionBox().height);

			//Update the rainbow.
			mRainbows[i]->Update(deltaTime, e);

			if (!mRainbows[i]->GetAlive())
				rainbowIndexToDelete = i;
			else
			{
				//check if the player has collided with it.
				if (!mBubCharacter->IsJumping())
				{
					if (Collisions::Instance()->PointInBox(Vector2D(xPosition, footPosition), mRainbows[i]->GetCollisionBox()))
					{
						mBubCharacter->SetState(CHARACTERSTATE_WALK);
						mBubCharacter->SetOnARainbow(true);
						int xPointOfCollision = (int)(mRainbows[i]->GetPosition().x + mRainbows[i]->GetCollisionBox().width - xPosition);
						if (mBubCharacter->GetFacing() == FACING_RIGHT)
							xPointOfCollision = (int)(xPosition - mRainbows[i]->GetPosition().x);

						//We don't want to pop between walking on different rainbows. Ensure the switch between rainbows looks 'realistic'
						double distanceBetweenPoints = footPosition - (mRainbows[i]->GetPosition().y - RainbowOffsets[xPointOfCollision]);
						if(distanceBetweenPoints < 40.0)
							mBubCharacter->SetPosition(Vector2D(mBubCharacter->GetPosition().x, mRainbows[i]->GetPosition().y - RainbowOffsets[xPointOfCollision]));
					}
				}

				//Check for collisions with enemies.
				for (unsigned int j = 0; j < mEnemies.size(); j++)
				{
					if (mRainbows[i]->CanKill())
					{
						if (Collisions::Instance()->Circle(mRainbows[i]->GetStrikePosition(), mRainbows[i]->GetCollisionRadius(), mEnemies[j]->GetPosition(), mEnemies[j]->GetCollisionRadius()))
						{
							mEnemies[j]->SetAlive(false);
						}
					}
				}
			}
		}

		//--------------------------------------------------------------------------------------------------
		//Remove a dead rainbow - 1 each update.
		//--------------------------------------------------------------------------------------------------
		if (rainbowIndexToDelete != -1)
		{
			Character* toDelete = mRainbows[rainbowIndexToDelete];
			mRainbows.erase(mRainbows.begin() + rainbowIndexToDelete);
			delete toDelete;
			toDelete = NULL;
		}
	}
}

void GameScreen_RainbowIslands::UpdateRainbowsNN(size_t deltaTime, SDL_Event e)
{
	//--------------------------------------------------------------------------------------------------
	//Update the Rainbows.
	//--------------------------------------------------------------------------------------------------

	for (int i = 0; i < kNumOfCharacters; i++)
	{
		m_vecCharacters[i]->SetOnARainbow(false);
	}

	if (!mRainbows.empty())
	{
		int rainbowIndexToDelete = -1;

		for (unsigned int iRainbow = 0; iRainbow < mRainbows.size(); iRainbow++)
		{
			//Update the rainbow.
			mRainbows[iRainbow]->Update(deltaTime, e);

			if (!mRainbows[iRainbow]->GetAlive())
				rainbowIndexToDelete = iRainbow;
			else
			{
				for (int iCharacter = 0; iCharacter < kNumOfCharacters; iCharacter++)
				{
					int xPosition = (int)m_vecCharacters[iCharacter]->GetPosition().x + (int)(m_vecCharacters[iCharacter]->GetCollisionBox().width*0.5f);
					int footPosition = (int)(m_vecCharacters[iCharacter]->GetPosition().y + m_vecCharacters[iCharacter]->GetCollisionBox().height);

					//check if the player has collided with it.
					if (!m_vecCharacters[iCharacter]->IsJumping())
					{
						if (Collisions::Instance()->PointInBox(Vector2D(xPosition, footPosition), mRainbows[iRainbow]->GetCollisionBox()))
						{
							m_vecCharacters[iCharacter]->SetState(CHARACTERSTATE_WALK);
							m_vecCharacters[iCharacter]->SetOnARainbow(true);
							int xPointOfCollision = (int)(mRainbows[iRainbow]->GetPosition().x + mRainbows[iRainbow]->GetCollisionBox().width - xPosition);
							if (m_vecCharacters[iCharacter]->GetFacing() == FACING_RIGHT)
								xPointOfCollision = (int)(xPosition - mRainbows[iRainbow]->GetPosition().x);

							//We don't want to pop between walking on different rainbows. Ensure the switch between rainbows looks 'realistic'
							double distanceBetweenPoints = footPosition - (mRainbows[iRainbow]->GetPosition().y - RainbowOffsets[xPointOfCollision]);
							if (distanceBetweenPoints < 40.0)
								m_vecCharacters[iCharacter]->SetPosition(Vector2D(m_vecCharacters[iCharacter]->GetPosition().x, mRainbows[iRainbow]->GetPosition().y - RainbowOffsets[xPointOfCollision]));
						}
					}

					//Check for collisions with enemies.
					for (unsigned int iEnemy = 0; iEnemy < mEnemies.size(); iEnemy++)
					{
						if (mRainbows[iRainbow]->CanKill())
						{
							if (Collisions::Instance()->Circle(mRainbows[iRainbow]->GetStrikePosition(), mRainbows[iRainbow]->GetCollisionRadius(), mEnemies[iEnemy]->GetPosition(), mEnemies[iEnemy]->GetCollisionRadius()))
							{
								mEnemies[iEnemy]->SetAlive(false);
							}
						}
					}
				}
			}
		}

		//--------------------------------------------------------------------------------------------------
		//Remove a dead rainbow - 1 each update.
		//--------------------------------------------------------------------------------------------------
		if (rainbowIndexToDelete != -1)
		{
			Character* toDelete = mRainbows[rainbowIndexToDelete];
			mRainbows.erase(mRainbows.begin() + rainbowIndexToDelete);
			delete toDelete;
			toDelete = NULL;
		}
	}
}

//--------------------------------------------------------------------------------------------------

bool GameScreen_RainbowIslands::SetUpLevel()
{
	//Load the background texture.
	mBackgroundTexture = new Texture2D(mRenderer);
	if( !mBackgroundTexture->LoadFromFile("Images/RainbowIslands/Tiles.png"))
	{
		cout << "Failed to load background texture!";
		return false;
	}

	//Create the level map.
	SetLevelMap();

	if (!USE_NEURAL_NETWORK)
	{
		CreateStartingCharacters();
	}
	else
	{
		CreateStartingCharactersNN();
	}

	mCanSpawnRainbow = true;

	//Set up the bad guys.
	CreateCaterpillar(Vector2D(150, 500), FACING_LEFT);
	CreateCaterpillar(Vector2D(130, 403), FACING_LEFT);
	CreateCaterpillar(Vector2D(270, 450), FACING_LEFT);
	CreateCaterpillar(Vector2D(30, 350), FACING_RIGHT);
	CreateCaterpillar(Vector2D(30, 160), FACING_RIGHT);
	CreateCaterpillar(Vector2D(270, 160), FACING_LEFT);
	CreateCaterpillar(Vector2D(250, 227), FACING_RIGHT);

	//Setup starting pickups.
	CreateFruit(Vector2D(26, 575), false);
	CreateFruit(Vector2D(46, 575), false);
	CreateFruit(Vector2D(256, 575), false);
	CreateFruit(Vector2D(276, 575), false);
	CreateFruit(Vector2D(26, 544), false);
	CreateFruit(Vector2D(256, 544), false);
	CreateFruit(Vector2D(300, 544), false);
	CreateFruit(Vector2D(100, 496), false);
	CreateFruit(Vector2D(200, 496), false);
	CreateFruit(Vector2D(256, 450), false);
	CreateFruit(Vector2D(246, 227), false);
	CreateFruit(Vector2D(276, 227), false);
	CreateFruit(Vector2D(8, 160), false);
	CreateFruit(Vector2D(32, 160), false);
	CreateFruit(Vector2D(56, 160), false);
	CreateFruit(Vector2D(270, 160), false);
	CreateFruit(Vector2D(140, 177), false);
	CreateFruit(Vector2D(83, 112), false);
	CreateFruit(Vector2D(103, 112), false);
	CreateFruit(Vector2D(123, 112), false);

	mChest = NULL;
	mTriggeredChestSpawns = false;

	mTimeToCompleteLevel = LEVEL_TIME;
	mTriggeredAnger = false;

	return true;
}

//--------------------------------------------------------------------------------------------------

void GameScreen_RainbowIslands::RestartLevel()
{
	if (!USE_NEURAL_NETWORK)
	{
		//Player character.
		delete mBubCharacter;
		mBubCharacter = NULL;
	}
	else
	{
		for (int i = 0; i < kNumOfCharacters; i++)
		{
			delete m_vecCharacters[i];
			m_vecCharacters[i] = NULL;
		}
	}

	//Level map.
	delete mLevelMap;
	mLevelMap = NULL;

	//Enemies.
	mEnemies.clear();

	//Fruit.
	mFruit.clear();

	//Rainbows.
	mRainbows.clear();

	//Treasure chest.
	if (mChest != NULL)
	{
		delete mChest;
		mChest = NULL;
	}

	//Create the level map.
	SetLevelMap();

	//Respawn characters and map.
	if (!USE_NEURAL_NETWORK)
	{
		CreateStartingCharacters();
	}
	else
	{
		CreateStartingCharactersNN();
	}

	mCanSpawnRainbow = true;

	//Set up the bad guys.
	CreateCaterpillar(Vector2D(150, 500), FACING_LEFT);
	CreateCaterpillar(Vector2D(130, 403), FACING_LEFT);
	CreateCaterpillar(Vector2D(270, 450), FACING_LEFT);
	CreateCaterpillar(Vector2D(30, 350), FACING_RIGHT);
	CreateCaterpillar(Vector2D(30, 160), FACING_RIGHT);
	CreateCaterpillar(Vector2D(270, 160), FACING_LEFT);
	CreateCaterpillar(Vector2D(250, 227), FACING_RIGHT);

	//Setup starting pickups.
	CreateFruit(Vector2D(26, 575), false);
	CreateFruit(Vector2D(46, 575), false);
	CreateFruit(Vector2D(256, 575), false);
	CreateFruit(Vector2D(276, 575), false);
	CreateFruit(Vector2D(26, 544), false);
	CreateFruit(Vector2D(256, 544), false);
	CreateFruit(Vector2D(300, 544), false);
	CreateFruit(Vector2D(100, 496), false);
	CreateFruit(Vector2D(200, 496), false);
	CreateFruit(Vector2D(256, 450), false);
	CreateFruit(Vector2D(246, 227), false);
	CreateFruit(Vector2D(276, 227), false);
	CreateFruit(Vector2D(8, 160), false);
	CreateFruit(Vector2D(32, 160), false);
	CreateFruit(Vector2D(56, 160), false);
	CreateFruit(Vector2D(270, 160), false);
	CreateFruit(Vector2D(140, 177), false);
	CreateFruit(Vector2D(83, 112), false);
	CreateFruit(Vector2D(103, 112), false);
	CreateFruit(Vector2D(123, 112), false);

	mChest = NULL;
	mTriggeredChestSpawns = false;

	mTimeToCompleteLevel = LEVEL_TIME;
	mTriggeredAnger		 = false;
}

//--------------------------------------------------------------------------------------------------

void GameScreen_RainbowIslands::CreateStartingCharacters()
{
	//Set up the player character.
	mBubCharacter = new CharacterBub(mRenderer, "Images/RainbowIslands/bub.png", mLevelMap, Vector2D(100, 570));
}

void GameScreen_RainbowIslands::CreateStartingCharactersNN()
{
	m_vecCharacters.clear();

	//let's create the characters
	for (int i = 0; i < kNumOfCharacters; ++i)
	{
		m_vecCharacters.push_back(new CCharacter(mRenderer, "Images/RainbowIslands/bub.png", mLevelMap, Vector2D(100, 570)));
	}

	//initialize the Genetic Algorithm class
	m_pGA = new CGenAlg(kNumOfCharacters,
		kMutationRateNN,
		kCrossoverRateNN,
		m_vecCharacters[0]->GetNumberOfWeights());

	m_vecThePopulation = m_pGA->GetChromos();

	for (int i = 0; i < kNumOfCharacters; i++)
	{
		m_vecCharacters[i]->PutWeights(m_vecThePopulation[i].vecWeights);
	}
}

//--------------------------------------------------------------------------------------------------

void GameScreen_RainbowIslands::SetLevelMap()
{
	int backgroundMap[MAP_HEIGHT][MAP_WIDTH] = {
	{5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5},
	{5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5},
	{5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5},
	{5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,30,38,31},
	{7,7,7,7,8,5,5,6,7,7,7,7,7,7,7,8,5,5,5,5},
	{12,12,12,12,13,4,4,11,12,12,12,12,12,12,12,13,4,4,4,4},
	{5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5},
	{5,5,27,28,29,5,5,5,5,5,5,5,5,5,5,25,26,5,5,5},
	{5,5,32,33,34,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5},
	{5,5,5,5,5,5,5,5,5,5,5,25,26,5,5,5,5,5,5,5},
	{22,5,14,14,5,5,5,5,5,5,5,5,5,5,5,5,14,5,21,22},
	{4,4,4,4,4,5,14,14,14,14,5,5,5,5,5,5,4,4,4,4},
	{5,5,5,5,5,5,4,4,4,4,4,5,5,5,5,5,5,5,5,5},
	{5,5,5,5,5,5,5,5,5,5,5,5,5,27,28,29,5,30,37,31},
	{5,5,5,5,5,5,5,5,5,5,5,5,5,32,33,34,14,5,14,14},
	{4,4,4,5,5,14,14,25,26,5,5,5,5,5,5,4,4,4,4,4},
	{5,5,5,5,5,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5},
	{5,25,26,5,5,5,5,14,14,14,14,14,6,7,7,7,7,8,5,5},
	{5,5,5,5,4,4,4,4,4,4,4,4,11,12,12,12,12,13,4,4},
	{5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5},
	{4,4,4,5,5,5,19,5,19,5,25,26,5,5,5,5,5,5,5,25},
	{5,5,5,5,5,17,18,17,18,5,5,5,5,5,5,5,5,5,5,5},
	{21,22,21,22,5,15,16,15,16,5,5,5,5,19,5,5,5,30,36,31},
	{0,0,0,1,0,0,1,0,0,0,0,1,0,0,5,5,5,19,5,5},
	{5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,0,0,0},
	{26,5,5,5,5,5,14,14,14,14,14,14,14,14,14,5,5,5,5,5},
	{5,5,5,19,0,0,0,0,0,0,0,0,0,0,0,5,5,5,5,19},
	{5,5,17,18,5,5,5,5,5,5,5,5,5,5,5,5,5,5,17,18},
	{14,14,15,16,5,5,5,5,5,5,5,5,27,28,29,5,14,14,15,16},
	{0,1,0,1,5,5,25,26,5,5,5,5,32,33,34,5,1,0,1,0},
	{5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5},
	{28,29,5,5,5,14,14,5,5,14,14,5,5,14,14,5,5,5,5,25},
	{33,34,19,5,5,0,0,1,0,0,0,0,1,0,0,5,5,5,5,5},
	{5,17,18,5,5,5,5,5,5,5,5,5,5,5,5,5,5,30,35,31},
	{14,15,16,5,27,28,29,5,19,5,5,5,19,5,5,5,5,5,5,5},
	{2,1,0,0,32,33,34,17,18,5,5,17,18,5,5,5,0,0,1,23},
	{24,24,24,9,9,9,5,15,16,5,5,15,16,5,9,9,9,24,24,24},
	{2,0,0,0,0,1,0,0,0,1,1,0,0,0,1,0,0,0,0,23},
	{20,20,20,20,20,20,10,10,10,20,20,10,10,10,20,20,20,20,20,20}};

	int collisionMap[MAP_HEIGHT][MAP_WIDTH] = {
		{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1 },
		{ 0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 },
		{ 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 } };

	//Clear up any old map.
	if(mLevelMap != NULL)
	{
		delete mLevelMap;
	}

	//Set the new one.
	mLevelMap = new LevelMap(collisionMap, backgroundMap);
}

//--------------------------------------------------------------------------------------------------

void GameScreen_RainbowIslands::CreateCaterpillar(Vector2D position, FACING direction)
{
	CharacterCaterpillar* caterpillarCharacter = new CharacterCaterpillar(mRenderer, "Images/RainbowIslands/Caterpillar.png", mLevelMap, position, direction);
	Character* tempCharacter = (Character*)caterpillarCharacter;
	mEnemies.push_back(tempCharacter);
	caterpillarCharacter = NULL;
}

//--------------------------------------------------------------------------------------------------

void GameScreen_RainbowIslands::CreateRainbow(Vector2D position, int numberOfRainbows)
{
	double xOffset   = 0.0;
	float  spwnDelay = 0.0f;
	do
	{
		if(mBubCharacter->GetFacing() == FACING_LEFT)
			position.x -= xOffset;
		else
			position.x += xOffset;
		
		CharacterRainbow* rainbowCharacter = new CharacterRainbow(mRenderer, "Images/RainbowIslands/Rainbow.png", position, mBubCharacter->GetFacing(), spwnDelay);
		if (rainbowCharacter->GetFacing() == FACING_LEFT)
			rainbowCharacter->SetPosition(Vector2D(rainbowCharacter->GetPosition().x - rainbowCharacter->GetCollisionBox().width, rainbowCharacter->GetPosition().y));
		
		mRainbows.push_back(rainbowCharacter);
		numberOfRainbows--;
		xOffset = rainbowCharacter->GetCollisionBox().width-10.0;
		spwnDelay += 200;
	} while (numberOfRainbows > 0);
}

void GameScreen_RainbowIslands::CreateRainbow(Vector2D position, int numberOfRainbows, CCharacter * character)
{
	double xOffset = 0.0;
	float  spwnDelay = 0.0f;
	do
	{
		if (character->GetFacing() == FACING_LEFT)
			position.x -= xOffset;
		else
			position.x += xOffset;

		CharacterRainbow* rainbowCharacter = new CharacterRainbow(mRenderer, "Images/RainbowIslands/Rainbow.png", position, character->GetFacing(), spwnDelay);
		if (rainbowCharacter->GetFacing() == FACING_LEFT)
			rainbowCharacter->SetPosition(Vector2D(rainbowCharacter->GetPosition().x - rainbowCharacter->GetCollisionBox().width, rainbowCharacter->GetPosition().y));

		mRainbows.push_back(rainbowCharacter);
		numberOfRainbows--;
		xOffset = rainbowCharacter->GetCollisionBox().width - 10.0;
		spwnDelay += 200;
	} while (numberOfRainbows > 0);
}

//--------------------------------------------------------------------------------------------------

void GameScreen_RainbowIslands::CreateFruit(Vector2D position, bool bounce)
{
	CharacterFruit* fruitCharacter = new CharacterFruit(mRenderer, "Images/RainbowIslands/Pickups.png", mLevelMap, position, (FRUIT_TYPE)(rand()%FRUIT_MAX));
	mFruit.push_back(fruitCharacter);

	if (bounce)
		fruitCharacter->Jump();
}

//--------------------------------------------------------------------------------------------------

void GameScreen_RainbowIslands::CreateChest(Vector2D position)
{
	if (mChest == NULL)
	{
		mChest = new CharacterChest(mRenderer, "Images/RainbowIslands/Chest.png", mLevelMap, position);
	}
}

//--------------------------------------------------------------------------------------------------

void GameScreen_RainbowIslands::TriggerChestSpawns()
{
	Vector2D pos		= mChest->GetCentralPosition();
	int xRange			= (int)mChest->GetCollisionBox().width;
	int xDeductRange	= (int)mChest->GetCollisionBox().width/2;

	CreateFruit(Vector2D(pos.x + (rand() % xRange) - xDeductRange, pos.y), true);
	CreateFruit(Vector2D(pos.x + (rand() % xRange) - xDeductRange, pos.y), true);
	CreateFruit(Vector2D(pos.x + (rand() % xRange) - xDeductRange, pos.y), true);
	CreateFruit(Vector2D(pos.x + (rand() % xRange) - xDeductRange, pos.y), true);
	CreateFruit(Vector2D(pos.x + (rand() % xRange) - xDeductRange, pos.y), true);
	CreateFruit(Vector2D(pos.x + (rand() % xRange) - xDeductRange, pos.y), true);
	CreateFruit(Vector2D(pos.x + (rand() % xRange) - xDeductRange, pos.y), true);
	CreateFruit(Vector2D(pos.x + (rand() % xRange) - xDeductRange, pos.y), true);
	CreateFruit(Vector2D(pos.x + (rand() % xRange) - xDeductRange, pos.y), true);
	CreateFruit(Vector2D(pos.x + (rand() % xRange) - xDeductRange, pos.y), true);
	CreateFruit(Vector2D(pos.x + (rand() % xRange) - xDeductRange, pos.y), true);
	CreateFruit(Vector2D(pos.x + (rand() % xRange) - xDeductRange, pos.y), true);
	CreateFruit(Vector2D(pos.x + (rand() % xRange) - xDeductRange, pos.y), true);
	CreateFruit(Vector2D(pos.x + (rand() % xRange) - xDeductRange, pos.y), true);

	mTriggeredChestSpawns = true;
}

//--------------------------------------------------------------------------------------------------