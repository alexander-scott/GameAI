//------------------------------------------------------------------------
//  Author: Paul Roberts 2017
//------------------------------------------------------------------------

#ifndef _COLLISIONS_H
#define _COLLISIONS_H

#include "Commons.h"

//--------------------------------------------------------------------------------------------------

class GameObject;

//--------------------------------------------------------------------------------------------------
class Collisions
{

	//--------------------------------------------------------------------------------------------------
public:
	~Collisions();

	static Collisions* Instance();

	bool PointInBox(Vector2D point, Rect2D rect2);

	bool Circle(Vector2D circle1_Pos, float circle1_Radius, Vector2D circle2_Pos, float circle2_Radius);
	bool Box(Rect2D rect1, Rect2D rect2);
	bool TriangleCollision(Vector2D A, Vector2D B, Vector2D C, Vector2D P);
	bool LineIntersect(Vector2D Aa1, Vector2D a2, Vector2D b1, Vector2D b2);
	bool TriangleRectangleCollision(Vector2D A, Vector2D B, Vector2D C, Rect2D R);

	//--------------------------------------------------------------------------------------------------
private:
	Collisions();

	//--------------------------------------------------------------------------------------------------
private:
	static Collisions* mInstance;

};

//--------------------------------------------------------------------------------------------------
#endif //_COLLISIONS_H