//------------------------------------------------------------------------
//  Author: Paul Roberts 2017
//------------------------------------------------------------------------

#ifndef LUNARLANDER_H
#define LUNARLANDER_H

#include "../GameObject.h"
#include "../Texture2D.h"
#include "../Commons.h"
#include <SDL.h>

//---------------------------------------------------------------

class LunarLander : public GameObject
{
//---------------------------------------------------------------
public:
	LunarLander(SDL_Renderer* renderer, Vector2D startPosition, Rect2D platform);
	~LunarLander();

	virtual void		Update(size_t deltaTime, SDL_Event e);
	virtual void		UpdateAdjustedBoundingBox();

	void				Render();
	void				Reset();

	Vector2D			GetPosition()														{return mPosition;}
	Vector2D			GetStartPosition()													{return mStartPosition;}
	Vector2D			GetCentralPosition();
	double				GetSpeed()															{return mVelocity.Length();}

	void				TurnLeft(size_t deltaTime);
	void				TurnRight(size_t deltaTime);
	void				Thrust();

	bool				IsAlive()															{return mAlive;}
	bool				HasLanded()															{return mLanded;}

	double				GetRotationAngle()													{return mRotationAngle;}
	double				GetSurvivalTime()													{return mSurvivalTime;}

//---------------------------------------------------------------
protected:
	void RotateByRadian(double radian, size_t deltaTime);

//---------------------------------------------------------------
private:
	bool HasCollidedWithScreenEdge();
	bool HasCollidedWithPlatform();
	bool HasTouchedDownSafely();

//---------------------------------------------------------------
protected:
	Texture2D*		mAliveTexture;
	Texture2D*		mDeadTexture;
	Texture2D*		mLandedTexture;
	Texture2D*		mThrustTexture;

	bool			mAlive;
	bool			mLanded;

	bool			mThrusting;
	double			mThrustingTime;

	Vector2D		mStartPosition;
	Vector2D		mPosition;
	Vector2D		mVelocity;
	Vector2D		mHeading;
	double			mRotationAngle;
	double			mSurvivalTime;

	Rect2D			mLandingPlatform;
};

//---------------------------------------------------------------

#endif //LUNARLANDER_H