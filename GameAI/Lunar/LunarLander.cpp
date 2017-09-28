//------------------------------------------------------------------------
//  Author: Paul Roberts 2017
//------------------------------------------------------------------------

#include "LunarLander.h"
#include "../Texture2D.h"
#include "LunarConstants.h"
#include <SDL.h>
#include <iostream>
#include "../C2DMatrix.h"
#include "../Collisions.h"
#include "../GameObject.h"

using namespace::std;

//--------------------------------------------------------------------------------------------------

LunarLander::LunarLander(SDL_Renderer* renderer, Vector2D startPosition, Rect2D platform)
	: GameObject(renderer, startPosition, "Images/Lunar/Lander.png")
{
	mLandingPlatform = platform;

	mAliveTexture = new Texture2D(renderer);
	mAliveTexture->LoadFromFile("Images/Lunar/Lander.png");
	mDeadTexture = new Texture2D(renderer);
	mDeadTexture->LoadFromFile("Images/Lunar/LanderRed.png");
	mLandedTexture = new Texture2D(renderer);
	mLandedTexture->LoadFromFile("Images/Lunar/LanderGreen.png");
	mThrustTexture = new Texture2D(renderer);
	mThrustTexture->LoadFromFile("Images/Lunar/Thrust.png");

	mStartPosition = startPosition;
	Reset();
}

//--------------------------------------------------------------------------------------------------

LunarLander::~LunarLander()
{
	delete mAliveTexture;
	mAliveTexture = NULL;

	delete mDeadTexture;
	mDeadTexture = NULL;

	delete mLandedTexture;
	mLandedTexture = NULL;
}

//--------------------------------------------------------------------------------------------------

void LunarLander::Update(size_t deltaTime, SDL_Event e)
{
	if(mAlive && !mLanded)
	{
		mSurvivalTime   += deltaTime;
		mVelocity.y		+= kGravity;
		mPosition		+= mVelocity*deltaTime;

		//Visual effects for thrusting.
		if(mThrusting)
		{
			mThrustingTime += deltaTime;

			if(mThrustingTime > kThrustDuration)
				mThrusting = false;
		}

		//Has the lander hit the screen edges or the landing platform?
		if(HasCollidedWithScreenEdge())
			mAlive = false;
		else if(HasCollidedWithPlatform())
		{
			if(HasTouchedDownSafely())
				mLanded = true;
			else
				mAlive = false;
		}
	}

	UpdateAdjustedBoundingBox();
}

//--------------------------------------------------------------------------------------------------

void LunarLander::Render()
{
	if(mAlive)
	{
		if(mLanded)
			mLandedTexture->Render(mPosition, SDL_FLIP_NONE, mRotationAngle);
		else
		{
			mAliveTexture->Render(mPosition, SDL_FLIP_NONE, mRotationAngle);

			//Add the thrust image if thrusting.
			if(mThrusting)
				mThrustTexture->Render(mPosition, SDL_FLIP_NONE, mRotationAngle);
		}
	}
	else
		mDeadTexture->Render(mPosition, SDL_FLIP_NONE, mRotationAngle);

	DrawCollisionBox();
}

//--------------------------------------------------------------------------------------------------

Vector2D LunarLander::GetCentralPosition()
{
	Vector2D centre = Vector2D(mPosition.x+(mAliveTexture->GetWidth()*0.5f), mPosition.y+(mAliveTexture->GetHeight()*0.5f));
	return centre;
}

//--------------------------------------------------------------------------------------------------

void LunarLander::RotateByRadian(double radian, size_t deltaTime)
{
	double degrees = RadsToDegs(radian);
	mRotationAngle += degrees;
	if(mRotationAngle > 360.0f)
		mRotationAngle -= 360.0f;
	else if(mRotationAngle < 0.0f)
		mRotationAngle += 360.0f;
  
	//Usee a rotation matrix to rotate the player's heading
	C2DMatrix RotationMatrix;
  
	//Calculate the direction of rotation.
	RotationMatrix.Rotate(radian);	

	//Get the new fire direction.
	RotationMatrix.TransformVector2Ds(mHeading);
}

//--------------------------------------------------------------------------------------------------

void LunarLander::TurnLeft(size_t deltaTime)
{
	if(mAlive && !mLanded)
	{
		RotateByRadian(-0.2f, deltaTime);
	}
}

//--------------------------------------------------------------------------------------------------

void LunarLander::TurnRight(size_t deltaTime)
{
	if(mAlive && !mLanded)
	{
		RotateByRadian(0.2f, deltaTime);
	}
}

//--------------------------------------------------------------------------------------------------

void LunarLander::Thrust()
{
	if(mAlive && !mLanded)
	{
		//Add to the velocity the thrust value * new heading.
		mVelocity		+= mHeading*kThrust;

		//Visual effects.
		mThrusting		= true;
		mThrustingTime	= 0.0f;
	}
}

//--------------------------------------------------------------------------------------------------

void LunarLander::Reset()
{
	mPosition		 = mStartPosition;

	UpdateAdjustedBoundingBox();

	mLanded			 = false;
	mAlive			 = true;
	mVelocity		 = Vector2D();
	mHeading		 = Vector2D(0.0f, -1.0f);
	mRotationAngle	 = 0.0f;
	mSurvivalTime	 = 0.0;
	mThrusting		 = false;
	mThrustingTime	 = 0.0f;
}

//--------------------------------------------------------------------------------------------------

bool LunarLander::HasCollidedWithScreenEdge()
{
	if( mPosition.x < 0.0f ||
		mPosition.x + mAliveTexture->GetWidth() > kLunarScreenWidth ||
		mPosition.y < 0.0f ||
		mPosition.y + mAliveTexture->GetHeight() > kLunarScreenHeight )
	{
		return true;
	}
	else
	{
		return false;
	}
}

//--------------------------------------------------------------------------------------------------

bool LunarLander::HasCollidedWithPlatform()
{
	vector<Vector2D> rect = GetAdjustedBoundingBox();
	if (Collisions::Instance()->TriangleRectangleCollision(rect[1], rect[2], rect[3], mLandingPlatform) || 
		Collisions::Instance()->TriangleRectangleCollision(rect[0], rect[1], rect[3], mLandingPlatform))
		return true;

	//Failed the checks.
	return false;

}

//--------------------------------------------------------------------------------------------------

bool LunarLander::HasTouchedDownSafely()
{
	if( mVelocity.Length() < kLandingSpeed &&																												//Correct descent speed.
		(mRotationAngle == 0.0f || mRotationAngle == 360.0f) &&																								//No rotation.
		/*mPosition.y-mAliveTexture->GetHeight() < mLandingPlatform.y &&	*/																					//Above the platform.
		Collisions::Instance()->PointInBox(Vector2D(mPosition.x, mPosition.y+mAliveTexture->GetHeight()), mLandingPlatform) && 								//Both sides of the lander are on the platform.
		Collisions::Instance()->PointInBox(Vector2D(mPosition.x + mAliveTexture->GetWidth(), mPosition.y+mAliveTexture->GetHeight()), mLandingPlatform) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

//--------------------------------------------------------------------------------------------------

void LunarLander::UpdateAdjustedBoundingBox()
{
	//Return texture size - Override for more precise collision.
	//This assumes there is only one image on the texture.
	Vector2D center = GetCentralPosition();
	float    halfWidth = (float)mTexture->GetWidth()*0.5f;
	float    halfHeight = (float)mTexture->GetHeight()*0.5f;

	float minX = (float)center.x - halfWidth;
	float minY = (float)center.y - halfHeight;

	float maxX = (float)center.x + halfWidth;
	float maxY = (float)center.y + halfHeight;

	double botLeftXRotated = (minX - center.x) * cos(mRotationAngle * M_PI / 180) - (minY - center.y) * sin(mRotationAngle * M_PI / 180);
	double botLeftYRotated = (minX - center.x) * sin(mRotationAngle * M_PI / 180) + (minY - center.y) * cos(mRotationAngle * M_PI / 180);

	double botRightXRotated = (maxX - center.x) * cos(mRotationAngle * M_PI / 180) - (minY - center.y) * sin(mRotationAngle * M_PI / 180);
	double botRightYRotated = (maxX - center.x) * sin(mRotationAngle * M_PI / 180) + (minY - center.y) * cos(mRotationAngle * M_PI / 180);

	double topRightXRotated = (maxX - center.x) * cos(mRotationAngle * M_PI / 180) - (maxY - center.y) * sin(mRotationAngle * M_PI / 180);
	double topRightYRotated = (maxX - center.x) * sin(mRotationAngle * M_PI / 180) + (maxY - center.y) * cos(mRotationAngle * M_PI / 180);

	double topLeftXRotated = (minX - center.x) * cos(mRotationAngle * M_PI / 180) - (maxY - center.y) * sin(mRotationAngle * M_PI / 180);
	double topLeftYRotated = (minX - center.x) * sin(mRotationAngle * M_PI / 180) + (maxY - center.y) * cos(mRotationAngle * M_PI / 180);

	Vector2D rotatedRect[4];
	rotatedRect[0] = Vector2D(botLeftXRotated, botLeftYRotated) + center; //Bottom Left
	rotatedRect[1] = Vector2D(botRightXRotated, botRightYRotated) + center; //Bottom Right
	rotatedRect[2] = Vector2D(topRightXRotated, topRightYRotated) + center; //Top Right
	rotatedRect[3] = Vector2D(topLeftXRotated, topLeftYRotated) + center; //Top Left

	mAdjustedBoundingBox[0] = rotatedRect[0];
	mAdjustedBoundingBox[1] = rotatedRect[1];
	mAdjustedBoundingBox[2] = rotatedRect[2];
	mAdjustedBoundingBox[3] = rotatedRect[3];
}

//--------------------------------------------------------------------------------------------------