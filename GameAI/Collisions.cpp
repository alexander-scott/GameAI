//------------------------------------------------------------------------
//  Author: Paul Roberts 2017
//------------------------------------------------------------------------

#include "Collisions.h"
#include "Commons.h"
#include <iostream>

//Initialise the instance to null.
Collisions* Collisions::mInstance = NULL;

//--------------------------------------------------------------------------------------------------

Collisions::Collisions()
{
}

//--------------------------------------------------------------------------------------------------

Collisions::~Collisions()
{
	mInstance = NULL;
}

//--------------------------------------------------------------------------------------------------

Collisions* Collisions::Instance()
{
	if (!mInstance)
	{
		mInstance = new Collisions;
	}

	return mInstance;
}

//--------------------------------------------------------------------------------------------------

bool Collisions::PointInBox(Vector2D point, Rect2D rect2)
{
	if (point.x > rect2.x &&
		point.x < (rect2.x + rect2.width) &&
		point.y > rect2.y &&
		point.y < (rect2.y + rect2.height))
	{
		return true;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------

bool Collisions::Circle(Vector2D circle1_Pos, float circle1_Radius, Vector2D circle2_Pos, float circle2_Radius)
{
	Vector2D vec = Vector2D((circle1_Pos.x - circle2_Pos.x), (circle1_Pos.y - circle2_Pos.y));
	double distance = sqrt((vec.x*vec.x) + (vec.y*vec.y));

	double combinedDistance = (circle1_Radius + circle2_Radius);
	return distance < combinedDistance;
}

//--------------------------------------------------------------------------------------------------

bool Collisions::Box(Rect2D rect1, Rect2D rect2)
{
	if (rect1.x + (rect1.width / 2) > rect2.x &&
		rect1.x + (rect1.width / 2) < rect2.x + rect2.width &&
		rect1.y + (rect1.height / 2) > rect2.y &&
		rect1.y + (rect1.height / 2) < rect2.y + rect2.height)
	{
		return true;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------

bool Collisions::TriangleCollision(Vector2D A, Vector2D B, Vector2D C, Vector2D P)
{
	//http://www.blackpawn.com/texts/pointinpoly/

	//Compute Vectors
	Vector2D v0 = C - A;
	Vector2D v1 = B - A;
	Vector2D v2 = P - A;

	//Compute dot product
	float dot00 = v0.Dot(v0);
	float dot01 = v0.Dot(v1);
	float dot02 = v0.Dot(v2);
	float dot11 = v1.Dot(v1);
	float dot12 = v1.Dot(v2);

	// Compute barycentric coordinates
	float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	// Check if point is in triangle
	return (u >= 0) && (v >= 0) && (u + v < 1);
}

bool Collisions::TriangleRectangleCollision(Vector2D A, Vector2D B, Vector2D C, Rect2D R)
{
	Vector2D a = A;
	Vector2D b = B;

	//Do line interescts on each of the lines.
	for (int i = 0; i < 3; i++)
	{
		//Get the relevant line from the triangle.
		switch (i)
		{
		case 0:
			a = A;
			b = B;
			break;

		case 1:
			a = B;
			b = C;
			break;

		case 2:
			a = C;
			b = A;
			break;

		default:
			break;
		}

		//Does this line interesct with any of the Rectangle lines?
		if (LineIntersect(a, b, Vector2D(R.x, R.y), Vector2D(R.x + R.width, R.y)))
			return true;
		if (LineIntersect(a, b, Vector2D(R.x + R.width, R.y), Vector2D(R.x + R.width, R.y + R.height)))
			return true;
		if (LineIntersect(a, b, Vector2D(R.x + R.width, R.y + R.height), Vector2D(R.x, R.y + R.height)))
			return true;
		if (LineIntersect(a, b, Vector2D(R.x, R.y + R.height), Vector2D(R.x, R.y)))
			return true;
	}

	//Non of the conditions were met.
	return false;
}

// a1 is line1 start, a2 is line1 end, b1 is line2 start, b2 is line2 end
bool Collisions::LineIntersect(Vector2D a1, Vector2D a2, Vector2D b1, Vector2D b2)
{
	Vector2D intersection = Vector2D();

	Vector2D b = a2 - a1;
	Vector2D d = b2 - b1;
	float bDotDPerp = b.x * d.y - b.y * d.x;

	// if b dot d == 0, it means the lines are parallel so have infinite intersection points
	if (bDotDPerp == 0)
		return false;

	Vector2D c = b1 - a1;
	float t = (c.x * d.y - c.y * d.x) / bDotDPerp;
	if (t < 0 || t > 1)
		return false;

	float u = (c.x * b.y - c.y * b.x) / bDotDPerp;
	if (u < 0 || u > 1)
		return false;

	intersection = a1 + (b*t);

	return true;
}

//--------------------------------------------------------------------------------------------------