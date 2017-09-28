//------------------------------------------------------------------------
//  Author: Mat Buckland
//------------------------------------------------------------------------

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <limits>

#define DEBUG_LINES_VISIBLE 1

//Useful constants.
const int     MaxInt    = (std::numeric_limits<int>::max)();
const double  MaxDouble = (std::numeric_limits<double>::max)();
const double  MinDouble = (std::numeric_limits<double>::min)();
const float   MaxFloat  = (std::numeric_limits<float>::max)();
const float   MinFloat  = (std::numeric_limits<float>::min)();

//Angle manipulation.
const double   Pi							= 3.14159;
const double   TwoPi						= Pi * 2;
const double   HalfPi						= Pi / 2;
const double   QuarterPi					= Pi / 4;

inline double DegsToRads(double degrees) {return TwoPi * (degrees/360.0f);}
inline double RadsToDegs(double radians) {return radians * (180.0f/Pi);}

const size_t UPDATES_PER_SECOND = 60;
const size_t MS_PER_UPDATE = 1000 / UPDATES_PER_SECOND;

#endif //CONSTANTS_H