//------------------------------------------------------------------------
//  Author: Paul Roberts 2017
//------------------------------------------------------------------------

#ifndef RAINBOW_CONSTANTS_H
#define RAINBOW_CONSTANTS_H

#define RAD2DEG	57.2957795f

//Screen dimensions.
const int kRainbowIslandsScreenWidth	= 320;
const int kRainbowIslandsScreenHeight   = 624;// 320;

//Background map dimensions.
const int MAP_WIDTH						= 20;
const int MAP_HEIGHT					= 39;

const int TILE_WIDTH					= 16;
const int TILE_HEIGHT					= 16;

//Generic
const float MOVE_SPEED					= 0.16f;
const float GRAVITY						= 0.1f;
const float DEAD_TIME					= 0.5f;

//Characters
const float ANIMATION_DELAY				= 100.0f;
const float RAINBOW_DELAY				= 10.0f;
const float RAINBOW_TIME				= 2000.0f;

//Caterpillar
const float CALM_SPEED					= 0.05f;
const float ANGRY_SPEED					= 0.1f;

//Jumping.
const float JUMP_FORCE_INITIAL			= 0.26f;
const float JUMP_FORCE_DECREMENT		= 0.008f;
const float JUMP_FORCE_INITIAL_FRUIT	= 0.29f;

//Level Time.
const int LEVEL_TIME					= 60000;//60 seconds
const int TRIGGER_ANGRY_TIME			= 10000;//10 seconds

//Complete Level.
const int Y_POSITION_TO_COMPLETE		= 60;

//Background tile map sprite sheet.
const int kTileSpriteSheetWidth			= 5;
const int kTileSpriteSheetHeight		= 5;

//--------------------------------------------------------------------
//TODO: Remove for student version.
const int kNumberOfOutputs				 = 4; // Controls on joypad.
const int kNumberOfHiddenLayers			 = 1; // 
const int kNumberOfNeuronsPerHiddenLayer = 3; // 

#endif //RAINBOW_CONSTANTS_H