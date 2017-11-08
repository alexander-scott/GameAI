//------------------------------------------------------------------------
//  Author: Paul Roberts 2017
//------------------------------------------------------------------------

#pragma once

//--------------------------------------------------------------------

enum FACING
{
	FACING_LEFT = 0,
	FACING_RIGHT
};

//--------------------------------------------------------------------

enum BADDIE_TYPE
{
	BADDIE_NORMAL = 0,
	BADDIE_ANGRY
};

//--------------------------------------------------------------------

enum CHARACTER_STATE
{
	CHARACTERSTATE_NONE,
	CHARACTERSTATE_WALK,
	CHARACTERSTATE_JUMP,
	CHARACTERSTATE_PLAYER_DEATH
};

//--------------------------------------------------------------------

enum FRUIT_TYPE
{
	FRUIT_EGGPLANT,
	FRUIT_CHOCOLATE,
	FRUIT_EGG,
	FRUIT_BURGER,
	FRUIT_ICECREAM,
	FRUIT_MELON,
	FRUIT_CAKE,
	FRUIT_APPLE,
	FRUIT_FLOWER,
	FRUIT_CORN,

	FRUIT_MAX
};


//--------------------------------------------------------------------
//TODO: Remove for student version.

//These are the world knoweldge items that the network accepts.
enum Perception
{
	Perception_GroundLeft,
	Perception_GroundCentre,
	Perception_GroundRight,
	Perception_CharacterX,
	Perception_CharacterY,
	Percention_DistanceToTop,
	//Perception_DistanceToNearestEnemy,
	//Perception_DistanceToNearestPickup

	Perception_Max
};
