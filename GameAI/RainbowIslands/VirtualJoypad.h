//------------------------------------------------------------------------
//  Author: Paul Roberts 2017
//------------------------------------------------------------------------

#ifndef _VIRTUALJOYPAD_H
#define _VIRTUALJOYPAD_H

#include <SDL.h>

//--------------------------------------------------------------------------------------------------
class VirtualJoypad
{
	
//--------------------------------------------------------------------------------------------------
public:
	~VirtualJoypad();

	static VirtualJoypad* Instance();

	void SetJoypadState(SDL_Event e);

//--------------------------------------------------------------------------------------------------
private:
	VirtualJoypad();

//--------------------------------------------------------------------------------------------------
private:
	static VirtualJoypad* mInstance;

public:
	bool LeftArrow;
	bool RightArrow;
	bool UpArrow;
	bool DownArrow;
	bool ForceRestartLevel;
};

//--------------------------------------------------------------------------------------------------
#endif //_VIRTUALJOYPAD_H