//------------------------------------------------------------------------
//  Author: Paul Roberts 2016
//------------------------------------------------------------------------

#ifndef LUNAR_CONSTANTS_H
#define LUNAR_CONSTANTS_H

//Screen dimensions.
const int	 kLunarScreenWidth			 = 500;		//In pixels.
const int	 kLunarScreenHeight			 = 500;

const float  kThrust					 = 0.2f;   //DO NOT CHANGE

const float  kGravity					 = 0.05f;  //DO NOT CHANGE
const float  kLandingSpeed				 = 300.0f; //DO NOT CHANGE

const int	 kNumberOfAILanders			 = 100;
//const int	 kHalfNumberOfAILanders		 = 50;
const int	 kNumberOfChromosomeElements = 100;

const int	 kCrossoverRate				 = 3000; //This is in a range of ints up to 10000 making it 30%
const int	 kMutationRate				 = 5;	 //This is in a range of ints up to 10000 making it 0.05%

const float	 kLandingBonus				 = 1.0f; //Todo: Change these values
const double kDistWeight				 = 1.0;  //Todo: Change these values
const double kRotWeight					 = 1.0;  //Todo: Change these values
const double kAirTimeWeight				 = 1.0;  //Todo: Change these values

const double kThrustDuration			 = 0.1; //DO NOT CHANGE

#endif //LUNAR_CONSTANTS_H