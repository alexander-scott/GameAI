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
	ForceRestartLevel = false;
	DebugView = false;
	mDebugPressed = false;
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

				case SDLK_d:
					mDebugPressed = false;
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

				case SDLK_r:
					ForceRestartLevel = true;
				break;

				case SDLK_d:
					if (!mDebugPressed)
					{
						DebugView = !DebugView;
						mDebugPressed = true;
					}
						
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