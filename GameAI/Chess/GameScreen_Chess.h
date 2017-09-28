//------------------------------------------------------------------------
//  Author: Paul Roberts 2016
//------------------------------------------------------------------------

#ifndef _GAMESCREEN_CHESS_H
#define _GAMESCREEN_CHESS_H

#include "../GameScreen.h"
#include "ChessCommons.h"
#include "../Commons.h"
#include "ChessPlayer.h"
#include <SDL.h>
#include <vector>
using namespace::std;

class Texture2D;

class GameScreen_Chess : GameScreen
{
//--------------------------------------------------------------------------------------------------
public:
	GameScreen_Chess(SDL_Renderer* renderer);
	~GameScreen_Chess();

	void Render();
	void Update(size_t deltaTime, SDL_Event e);

//--------------------------------------------------------------------------------------------------
private:
	void RenderBoard();
	void RenderPiece(BoardPiece boardPiece, Vector2D position);
	void RenderHighlights();
	void RenderPreTurnText();

//--------------------------------------------------------------------------------------------------
private:
	Texture2D*		 mBackgroundTexture;
	Texture2D*		 mWhitePiecesSpritesheet;
	Texture2D*		 mBlackPiecesSpritesheet;
	Texture2D*		 mSelectedPieceSpritesheet;
	Texture2D*		 mSquareHighlightSpritesheet;
	Texture2D*		 mPreviousMoveHighlightSpritesheet;
	Texture2D*		 mGameStateSpritesheet;
	Board*			 mChessBoard;

	ChessPlayer*	 mPlayers[2];
	COLOUR			 mPlayerTurn;
	bool			 mAIPlayerPlaying;

	Vector2D		 mSelectedPiecePosition;
	vector<Vector2D> mHighlightPositions;
	bool			 mHighlightsOn;
	Move*			 mLastMove;

	TURNSTATE		 mTurnState;
	GAMESTATE		 mGameState;
	size_t			 mPreTurnTextTimer;

	int*			 mSearchDepth;
};


#endif //_GAMESCREEN_CHESS_H