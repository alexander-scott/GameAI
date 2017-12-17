#include "CCharacter.h"

//-----------------------------------constructor-------------------------
//
//-----------------------------------------------------------------------
CCharacter::CCharacter(SDL_Renderer* renderer, string imagePath, LevelMap* map, Vector2D startPosition) : CharacterBub(renderer, imagePath, map, startPosition)
{
	m_previousInput = 0;
	m_dFitness = 0;
	m_spawnRainbow = false;
	m_startPosition = Vector2D(startPosition.x, startPosition.y + 30);
}

//-------------------------------------------Reset()--------------------
//
//	Resets the sweepers position, fitness and rotation
//
//----------------------------------------------------------------------
void CCharacter::Reset()
{
	//and the fitness
	m_dFitness = 0;

	return;
}

//-------------------------------Update()--------------------------------
//
//	First we take sensor readings and feed these into the sweepers brain.
//
//	The inputs are:
//	
//	Distance between yPosToComplete and current character central position. Scaled down.
//	A vector to the closest fruit. Normalised.
//  A vector away from the closest enemy. Normalised.
//
//	We receive four outputs from the brain: MoveLeft, MoveRight, Jump, SpawnRainbow. Highest value is the move we make.
//
//-----------------------------------------------------------------------
bool CCharacter::UpdateNN(int yPositionToComplete, vector<Character*> enemies, vector<CharacterFruit*> fruit)
{
	// This will store all the inputs for the NN
	vector<double> inputs;

	// Get closest fruit and closest enemy
	CharacterFruit* closestFruit = GetClosestFruit(fruit);
	Character* closestEnemy = GetClosestEnemy(enemies);

	// turns into a percentage of the screen height
	double toTheNearestFruit = (GetCentralPosition() - closestFruit->GetCentralPosition()).Length();
	toTheNearestFruit /= kRainbowIslandsScreenHeight;
	toTheNearestFruit = 1 - toTheNearestFruit;

	//go left
	if (closestFruit->GetCentralPosition().x < GetCentralPosition().x)
		inputs.push_back(1);
	else
		inputs.push_back(0);

	//go right
	if (closestFruit->GetCentralPosition().x > GetCentralPosition().x)
		inputs.push_back(1);
	else
		inputs.push_back(0);

	// turns into a percentage of the screen height
	double toTheNearestEnemy = (GetCentralPosition() - closestEnemy->GetCentralPosition()).Length();
	toTheNearestEnemy /= kRainbowIslandsScreenHeight;

	inputs.push_back(1 - (GetCentralPosition().y / kRainbowIslandsScreenHeight));

	inputs.push_back(toTheNearestFruit);
	inputs.push_back(toTheNearestEnemy);

	vector<double> surroundings = GetSurroundings(closestEnemy->GetCentralPosition(), mRainbows);
	inputs.insert(inputs.end(), surroundings.begin(), surroundings.end());

	inputs.push_back(OnARainbow());
	vector<double> inputsBackup = inputs;

	//update the brain and get feedback
	vector<double> output = m_ItsBrain.Update(inputs);

	//make sure there were no errors in calculating the 
	//output
	if (output.size() < kNumOutputs)
	{
		return false;
	}

	if (output[0] > 0.80)
		VirtualJoypad::Instance()->LeftArrow = true;
	else
		VirtualJoypad::Instance()->LeftArrow = false;
	if (output[1] > 0.85)
		VirtualJoypad::Instance()->DownArrow = true;
	else
		VirtualJoypad::Instance()->DownArrow = false;
	if (output[2] > 0.80)
		VirtualJoypad::Instance()->RightArrow = true;
	else
		VirtualJoypad::Instance()->RightArrow = false;
	if (output[3] > 0.80)
		VirtualJoypad::Instance()->UpArrow = true;
	else
		VirtualJoypad::Instance()->UpArrow = false;

	//if (output[0] > output[1] && output[0] > output[2] && output[0] > output[3]) // Move Left
	//{
	//	m_previousInput = 0;
	//	mMovingLeft = true;
	//	mMovingRight = false;
	//	if (mOnGround)
	//		SetState(CHARACTERSTATE_WALK);
	//}
	//else if (output[1] > output[0] && output[1] > output[2] && output[1] > output[3]) // Move right
	//{
	//	m_previousInput = 1;
	//	mMovingRight = true;
	//	mMovingLeft = false;
	//	if (mOnGround)
	//		SetState(CHARACTERSTATE_WALK);
	//}
	//else if (output[2] > output[0] && output[2] > output[1] && output[2] > output[3]) // Jump
	//{
	//	m_previousInput = 2;
	//	if (!mJumping)
	//	{
	//		SetState(CHARACTERSTATE_JUMP);
	//		Jump();
	//	}
	//}
	//else if (output[3] > output[0] && output[3] > output[1] && output[3] > output[2]) // Spawn rainbow
	//{
	//	m_previousInput = 3;
	//	m_spawnRainbow = true;
	//}

	return true;
}

//----------------------GetClosestObject()---------------------------------
//
//	returns the vector from the sweeper to the closest mine
//
//-----------------------------------------------------------------------
CharacterFruit* CCharacter::GetClosestFruit(vector<CharacterFruit*> &objects)
{
	double				closest_so_far = 99999;
	CharacterFruit*		closestObject;

	//cycle through mines to find closest
	for (int i = 0; i < objects.size(); i++)
	{
		double len_to_object = Vec2DLength(objects[i]->GetCentralPosition() - GetCentralPosition());

		if (len_to_object < closest_so_far)
		{
			closest_so_far = len_to_object;
			closestObject = objects[i];
		}
	}

	return closestObject;
}

Character* CCharacter::GetClosestEnemy(vector<Character*>& objects)
{
	double			closest_so_far = 99999;
	Character*		closestObject;

	//cycle through mines to find closest
	for (int i = 0; i < objects.size(); i++)
	{
		double len_to_object = Vec2DLength(objects[i]->GetCentralPosition() - GetCentralPosition());

		if (len_to_object < closest_so_far)
		{
			closest_so_far = len_to_object;
			closestObject = objects[i];
		}
	}

	return closestObject;
}

vector<double> CCharacter::GetSurroundings(Vector2D enemyLocation, vector<CharacterRainbow*> _mRainbows)
{
	surroundingPositions.clear();
	surroundings.clear();

	int centralXPosition = (int)GetCentralPosition().x / TILE_WIDTH;
	int centralYPosition = (int)GetCentralPosition().y / TILE_HEIGHT;
	int footPosition = (int)(mPosition.y + mTexture->GetHeight()) / TILE_HEIGHT;

	int startHeight = (int)GetCentralPosition().y - (mSingleSpriteHeight * 2);
	int startWidth = (int)GetCentralPosition().x - (mSingleSpriteWidth * 3);

	int endHeight = (int)GetCentralPosition().y + (mSingleSpriteHeight * 3);
	int endWidth = (int)GetCentralPosition().x + (mSingleSpriteWidth * 4);

	for (int height = startHeight; height < endHeight; height += mSingleSpriteHeight)
	{
		for (int width = startWidth; width < endWidth; width += mSingleSpriteWidth)
		{
			surroundingPositions.push_back(Vector2D(width, height));
			if (height < startHeight + (mSingleSpriteHeight * 2))
			{
				if (width == 0 || width <= 0 ||
					width >= kRainbowIslandsScreenWidth || width + mSingleSpriteWidth <= 0 ||
					enemyLocation.Distance(Vector2D(width, height)) < 20)
					surroundings.push_back(1);
				else
					surroundings.push_back(0);

			}
			else
			{
				bool rainbowFound = false;
				for each (CharacterRainbow* var in _mRainbows)
				{
					Vector2D rainbowPosition = Vector2D(var->GetCentralPosition().x, var->GetCentralPosition().y + 12);
					//cout << var->GetCentralPosition().x << "," << var->GetCentralPosition().y << "		" << width << "," << height << "		= " << abs(var->GetCentralPosition().Distance(Vector2D(width, height))) << endl;
					if (abs(rainbowPosition.Distance(Vector2D(width, height))) < 28)
					{
						surroundings.push_back(1);
						rainbowFound = true;
						break;
					}
				}

				if (rainbowFound)
					continue;
				else 
				{
					if (width == 0 || width <= 0 ||
						width >= kRainbowIslandsScreenWidth || width + mSingleSpriteWidth <= 0 ||
						enemyLocation.Distance(Vector2D(width, height)) < 20)
						surroundings.push_back(1);
					else
						surroundings.push_back(mCurrentLevelMap->GetCollisionTileAt(height / TILE_HEIGHT, width / TILE_WIDTH));
				}
			}
		}
	}

	if (surroundings.size() != 35)
		cout << "ERROR OCCURED: INCORRECT SURROUNDINGS" << endl;
	if (surroundingPositions.size() != 35)
		cout << "ERROR OCCURED: INCORRECT SURROUNDINGS SIZE" << endl;
	return surroundings;
}


