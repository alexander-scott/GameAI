//------------------------------------------------------------------------
//  Author: Paul Roberts 2017
//------------------------------------------------------------------------

#include "VirtualJoypad.h"

//Initialise the instance to null.
VirtualJoypad* VirtualJoypad::mInstance = NULL;

//--------------------------------------------------------------------------------------------------

VirtualJoypad::VirtualJoypad()
{
	LeftArrow = false;
	RightArrow = false;
	UpArrow = false;
	DownArrow = false;
}

//--------------------------------------------------------------------------------------------------

VirtualJoypad::~VirtualJoypad()
{
	mInstance = NULL;
}

//--------------------------------------------------------------------------------------------------

VirtualJoypad* VirtualJoypad::Instance()
{
	if(!mInstance)
	{
		mInstance = new VirtualJoypad;
	}

	return mInstance;
}

//--------------------------------------------------------------------------------------------------

void VirtualJoypad::SetJoypadState(SDL_Event e)
{
	switch (e.type)
	{
		//Deal with keyboard input.
		case SDL_KEYUP:
			switch (e.key.keysym.sym)
			{
				case SDLK_LEFT:
					LeftArrow = false;
				break;

				case SDLK_RIGHT:
					RightArrow = false;
				break;

				case SDLK_UP:
					UpArrow = false;
				break;

				case SDLK_DOWN:
					DownArrow = false;
				break;

				default:
				break;
			}
		break;

		case SDL_KEYDOWN:
			switch (e.key.keysym.sym)
			{
				case SDLK_LEFT:
					LeftArrow = true;
				break;

				case SDLK_RIGHT:
					RightArrow = true;
				break;

				case SDLK_UP:
					UpArrow = true;
				break;

				case SDLK_DOWN:
					DownArrow = true;
				break;

				default:
				break;
			}
		break;

		default:
		break;
	}
}

//--------------------------------------------------------------------------------------------------