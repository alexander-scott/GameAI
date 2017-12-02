#include "CharacterAI.h"   

//-----------------------------------constructor-------------------------   
//   
//-----------------------------------------------------------------------   
CharacterAI::CharacterAI(SDL_Renderer* renderer, string imagePath, LevelMap* map, Vector2D startPosition) : CharacterBub(renderer, imagePath, map, startPosition)
{
	//initialize its memory   
	m_MemoryMap.Init(CParams::WindowWidth,
		CParams::WindowHeight);
}

//-----------------------------Reset()------------------------------------   
//   
//  Resets the sweepers position, fitness and rotation   
//   
//------------------------------------------------------------------------   
void CharacterAI::Reset()
{
	//and the fitness   
	m_dFitness = 0;

	//reset its memory   
	m_MemoryMap.Reset();

}

//-------------------------------Update()--------------------------------   
//   
//  First we take sensor readings and feed these into the sweepers brain.   
//   
//  The inputs are:   
//     
//  The readings from the minesweepers sensors   
//   
//  We receive two outputs from the brain.. lTrack & rTrack.   
//  So given a force for each track we calculate the resultant rotation    
//  and acceleration and apply to current velocity vector.   
//   
//-----------------------------------------------------------------------   
bool CharacterAI::Update(vector<SPoint> &objects)
{

	////this will store all the inputs for the NN   
	//vector<double> inputs;

	////grab sensor readings   
	//TestSensors(objects);


	////input sensors into net   
	//for (int sr = 0; sr<m_vecdSensors.size(); ++sr)
	//{
	//	inputs.push_back(m_vecdSensors[sr]);

	//	inputs.push_back(m_vecFeelers[sr]);
	//}

	//inputs.push_back(m_bCollided);

	////update the brain and get feedback   
	//vector<double> output = m_pItsBrain->Update(inputs, CNeuralNet::active);

	////make sure there were no errors in calculating the    
	////output   
	//if (output.size() < CParams::iNumOutputs)
	//{
	//	return false;
	//}

	////assign the outputs to the sweepers left & right tracks   
	//m_lTrack = output[0];
	//m_rTrack = output[1];

	////calculate steering forces   
	//double RotForce = m_lTrack - m_rTrack;

	////clamp rotation   
	//Clamp(RotForce, -CParams::dMaxTurnRate, CParams::dMaxTurnRate);

	//m_dRotation += RotForce;

	////update Look At    
	//m_vLookAt.x = -sin(m_dRotation);
	//m_vLookAt.y = cos(m_dRotation);

	////if the sweepers haven't collided with an obstacle   
	////update their position   
	//if (!m_bCollided)
	//{
	//	m_dSpeed = m_lTrack + m_rTrack;

	//	//update position   
	//	m_vPosition += (m_vLookAt * m_dSpeed);

	//	//test range of x,y values - because of 'cheap' collision detection   
	//	//this can go into error when using < 4 sensors   
	//	TestRange();
	//}

	////update the memory map   
	//m_MemoryMap.Update(m_vPosition.x, m_vPosition.y);

	return true;
}

//------------------------- EndOfRunCalculations() -----------------------   
//   
//------------------------------------------------------------------------   
void CharacterAI::EndOfRunCalculations()
{
	m_dFitness += m_MemoryMap.NumCellsVisited();
}