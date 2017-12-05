#include "CCharacter.h"

//-----------------------------------constructor-------------------------
//
//-----------------------------------------------------------------------
CCharacter::CCharacter(SDL_Renderer* renderer, string imagePath, LevelMap* map, Vector2D startPosition) : CharacterBub(renderer, imagePath, map, startPosition)
{
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
	inputs.push_back(percentDone);

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

	////assign the outputs to the sweepers left & right tracks
	//m_lTrack = output[0];
	//m_rTrack = output[1];

	////calculate steering forces
	//double RotForce = m_lTrack - m_rTrack;

	////clamp rotation
	//Clamp(RotForce, -CParams::dMaxTurnRate, CParams::dMaxTurnRate);

	//m_dRotation += RotForce;

	//m_dSpeed = (m_lTrack + m_rTrack);

	////update Look At 
	//m_vLookAt.x = -sin(m_dRotation);
	//m_vLookAt.y = cos(m_dRotation);

	////update position
	//m_vPosition += (m_vLookAt * m_dSpeed);

	////wrap around window limits
	//if (m_vPosition.x > CParams::WindowWidth) m_vPosition.x = 0;
	//if (m_vPosition.x < 0) m_vPosition.x = CParams::WindowWidth;
	//if (m_vPosition.y > CParams::WindowHeight) m_vPosition.y = 0;
	//if (m_vPosition.y < 0) m_vPosition.y = CParams::WindowHeight;

	return true;
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


