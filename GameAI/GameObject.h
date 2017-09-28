//------------------------------------------------------------------------
//  Author: Paul Roberts 2017
//------------------------------------------------------------------------

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Commons.h"
#include <SDL.h>
#include <vector>

//---------------------------------------------------------------

class Texture2D;

class GameObject
{
//---------------------------------------------------------------
public:
	GameObject(SDL_Renderer* renderer, Vector2D startPosition, string imagePath);
	~GameObject();

	virtual void				Update(size_t deltaTime, SDL_Event e);
	virtual void				Render();

	virtual void				UpdateAdjustedBoundingBox();
	virtual vector<Vector2D>	GetAdjustedBoundingBox()											{return mAdjustedBoundingBox;}
	Vector2D					GetPosition()														{return mPosition;}
	Vector2D					GetCentralPosition();

	void						SetCollisionRadius(double newCollisionRadius)						{mCollisionRadius = newCollisionRadius; mCollisionRadiusSq = newCollisionRadius*newCollisionRadius;}
	double						GetCollisionRadius()												{return mCollisionRadius;}
	double						GetCollisionRadiusSq()												{return mCollisionRadiusSq;}

//---------------------------------------------------------------
protected:
	void						DrawDebugCircle(Vector2D centrePoint, double radius, int red, int green, int blue);
	void						DrawDebugLine(Vector2D startPoint, Vector2D endPoint, int red, int green, int blue);
	void						DrawCollisionBox();

//---------------------------------------------------------------
protected:
	SDL_Renderer*	 mRenderer;
	Texture2D*		 mTexture;
	Vector2D		 mPosition;
	double			 mRotationAngle;

	vector<Vector2D> mAdjustedBoundingBox;

	double			 mCollisionRadius;
	double			 mCollisionRadiusSq;
};

//---------------------------------------------------------------

#endif //GAMEOBJECT_H