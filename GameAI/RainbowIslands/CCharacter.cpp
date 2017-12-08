#include "CCharacter.h"

//-----------------------------------constructor-------------------------
//
//-----------------------------------------------------------------------
CCharacter::CCharacter(SDL_Renderer* renderer, string imagePath, LevelMap* map, Vector2D startPosition) : CharacterBub(renderer, imagePath, map, startPosition)
{

	m_spawnRainbow = false;
	m_startPosition = startPosition;
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
bool CCharacter::Update(int yPositionToComplete, vector<Character*> enemies, vector<CharacterFruit*> fruit)
{
	//this will store all the inputs for the NN
	vector<double> inputs;

	//get vector to closest mine
	Vector2D vClosestFruit = GetClosestFruit(fruit);
	vClosestFruit.Normalize();

	Vector2D vClosestEnemy = GetClosestEnemy(enemies);
	vClosestEnemy.Normalize();

	double startDistanceFromTop = std::abs(yPositionToComplete - m_startPosition.y);
	double distanceFromTop = std::abs(yPositionToComplete - GetCentralPosition().y);

	double percentDone = (distanceFromTop / startDistanceFromTop);
	percentDone = 1 - percentDone;

	//add in distance to top
	//inputs.push_back(percentDone);

	//add in vector to closest fruit
	inputs.push_back(vClosestFruit.x);
	inputs.push_back(vClosestFruit.y);

	// Add in vector away from closest enemy
	inputs.push_back(vClosestEnemy.x);
	inputs.push_back(vClosestEnemy.y);

	//update the brain and get feedback
	vector<double> output = m_ItsBrain.Update(inputs);

	//make sure there were no errors in calculating the 
	//output
	if (output.size() < kNumOutputs)
	{
		return false;
	}

	if (output[0] > output[1] && output[0] > output[2] && output[0] > output[3]) // Move Left
	{
		mMovingLeft = true;
		mMovingRight = false;
		if (mOnGround)
			SetState(CHARACTERSTATE_WALK);
	}
	else if (output[1] > output[0] && output[1] > output[2] && output[1] > output[3]) // Move right
	{
		mMovingRight = true;
		mMovingLeft = false;
		if (mOnGround)
			SetState(CHARACTERSTATE_WALK);
	}
	else if (output[2] > output[0] && output[2] > output[1] && output[2] > output[3]) // Jump
	{
		if (!mJumping)
		{
			SetState(CHARACTERSTATE_JUMP);
			Jump();
		}
	}
	else if (output[3] > output[0] && output[3] > output[1] && output[3] > output[2]) // Spawn rainbow
	{
		m_spawnRainbow = true;
	}

	return true;
}

void CCharacter::Update(size_t deltaTime, SDL_Event e)
{
	//cout << deltaTime << endl;

	//Change frame?
	mFrameDelay -= deltaTime;
	if (mFrameDelay <= 0.0f)
	{
		//Reset frame delay count.
		mFrameDelay = ANIMATION_DELAY;

		//Move frame on.
		mCurrentFrame++;

		//Loop frame around if it goes beyond the number of frames.
		if (mCurrentFrame > mEndFrame)
		{
			if (IsInState(CHARACTERSTATE_PLAYER_DEATH))
				mAlive = false;
			else
				mCurrentFrame = mStartFrame;
		}
	}

	//Update if not in death state.
	if (!IsInState(CHARACTERSTATE_PLAYER_DEATH))
	{
		AlternateCharacterUpdate(deltaTime, e);

		if (mOnGround && IsInState(CHARACTERSTATE_JUMP))
		{
			if (mMovingLeft || mMovingRight)
				SetState(CHARACTERSTATE_WALK);
			else
				SetState(CHARACTERSTATE_NONE);
		}

		//Collision position variables.
		int centralXPosition = (int)(mPosition.x + (mTexture->GetWidth()*0.5f)) / TILE_WIDTH;
		int centralYPosition = (int)(mPosition.y + mTexture->GetHeight()*0.5f) / TILE_HEIGHT;
		int footPosition = (int)(mPosition.y + mTexture->GetHeight()) / TILE_HEIGHT;


		/*if (VirtualJoypad::Instance()->LeftArrow)
		{
			mMovingLeft = true;
			mMovingRight = false;
			if (mOnGround)
				SetState(CHARACTERSTATE_WALK);
		}
		else if (VirtualJoypad::Instance()->RightArrow)
		{
			mMovingRight = true;
			mMovingLeft = false;
			if (mOnGround)
				SetState(CHARACTERSTATE_WALK);
		}
		else
		{
			mMovingLeft = false;
			mMovingRight = false;
			if (mOnGround)
				SetState(CHARACTERSTATE_NONE);
		}

		if (VirtualJoypad::Instance()->UpArrow)
		{
			if (!mJumping)
			{
				SetState(CHARACTERSTATE_JUMP);
				Jump();
			}
		}*/
	}
}


//----------------------GetClosestObject()---------------------------------
//
//	returns the vector from the sweeper to the closest mine
//
//-----------------------------------------------------------------------
Vector2D CCharacter::GetClosestFruit(vector<CharacterFruit*> &objects)
{
	double			closest_so_far = 99999;

	Vector2D		vClosestObject(0, 0);

	//cycle through mines to find closest
	for (int i = 0; i < objects.size(); i++)
	{
		double len_to_object = Vec2DLength(objects[i]->GetCentralPosition() - GetCentralPosition());

		if (len_to_object < closest_so_far)
		{
			closest_so_far = len_to_object;

			vClosestObject = GetCentralPosition() - objects[i]->GetCentralPosition();
		}
	}

	return vClosestObject;
}

Vector2D CCharacter::GetClosestEnemy(vector<Character*>& objects)
{
	double			closest_so_far = 99999;

	Vector2D		vClosestObject(0, 0);

	//cycle through mines to find closest
	for (int i = 0; i < objects.size(); i++)
	{
		double len_to_object = Vec2DLength(objects[i]->GetCentralPosition() - GetCentralPosition());

		if (len_to_object < closest_so_far)
		{
			closest_so_far = len_to_object;

			vClosestObject = GetCentralPosition() - objects[i]->GetCentralPosition();
		}
	}

	return vClosestObject;
}


