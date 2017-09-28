//------------------------------------------------------------------------
//  Author: Paul Roberts 2016
//------------------------------------------------------------------------

#include "GameScreen_Chess.h"
#include <iostream>
#include <time.h>
#include "../Texture2D.h"
#include "ChessConstants.h"
#include "ChessMoveManager.h"
#include "ChessPlayerAI.h"

using namespace::std;

//--------------------------------------------------------------------------------------------------
//          Board structure
// |-------------------------------|
// |0,0|1,0|2,0|3,0|4,0|5,0|6,0|7,0|
// |-------------------------------|
// |0,1|1,1|2,1|3,1|4,1|5,1|6,1|7,1|
// |-------------------------------|
// |0,2|1,2|2,2|3,2|4,2|5,2|6,2|7,2|
// |-------------------------------|
// |0,3|1,3|2,3|3,3|4,3|5,3|6,3|7,3|
// |-------------------------------|
// |0,4|1,4|2,4|3,4|4,4|5,4|6,4|7,4|
// |-------------------------------|
// |0,5|1,5|2,5|3,5|4,5|5,5|6,5|7,5|
// |-------------------------------|
// |0,6|1,6|2,6|3,6|4,6|5,6|6,6|7,6|
// |-------------------------------|
// |0,7|1,7|2,7|3,7|4,7|5,7|6,7|7,7|
// |-------------------------------|
//--------------------------------------------------------------------------------------------------

GameScreen_Chess::GameScreen_Chess(SDL_Renderer* renderer) : GameScreen(renderer)
{
	srand(static_cast <unsigned> (time(0)));

	//Set up board.
	mBackgroundTexture	= new Texture2D(renderer);
	mBackgroundTexture->LoadFromFile(kChessBoardPath);
	mChessBoard			= new Board();

	//Load in the spritesheets.
	mWhitePiecesSpritesheet = new Texture2D(renderer);
	mWhitePiecesSpritesheet->LoadFromFile(kChessWhitePiecesPath);
	mBlackPiecesSpritesheet = new Texture2D(renderer);
	mBlackPiecesSpritesheet->LoadFromFile(kChessBlackPiecesPath);
	mSelectedPieceSpritesheet = new Texture2D(renderer);
	mSelectedPieceSpritesheet->LoadFromFile(kSelectedChessPiecePath);
	mSquareHighlightSpritesheet = new Texture2D(renderer);
	mSquareHighlightSpritesheet->LoadFromFile(kChessSquareRedHighlightPath);
	mPreviousMoveHighlightSpritesheet = new Texture2D(renderer);
	mPreviousMoveHighlightSpritesheet->LoadFromFile(kChessSquareBlueHighlightPath);
	mGameStateSpritesheet = new Texture2D(renderer);
	mGameStateSpritesheet->LoadFromFile(kChessGameStatesPath);

	//Start values.
	mLastMove				= new Move(Vector2D(999,999), Vector2D(999,999));
	mSearchDepth			= new int;
	*mSearchDepth			= kSearchDepth;
	mSelectedPiecePosition  = Vector2D();
	mPlayers[COLOUR_WHITE]	= new ChessPlayer(renderer, COLOUR_WHITE, mChessBoard, &mHighlightPositions, &mSelectedPiecePosition, mLastMove); //HUMAN PLAYER
	mPlayers[COLOUR_BLACK]	= new ChessPlayerAI(renderer, COLOUR_BLACK, mChessBoard, &mHighlightPositions, &mSelectedPiecePosition, mLastMove, mSearchDepth); //AI PLAYER
	mAIPlayerPlaying		= false;
	//mPlayers[COLOUR_WHITE]	= new ChessPlayerAI(renderer, COLOUR_WHITE, mChessBoard, &mHighlightPositions, &mSelectedPiecePosition, mLastMove, mSearchDepth); //AI PLAYER
	//mAIPlayerPlaying		= true;
	mPlayerTurn				= COLOUR_WHITE;
	mTurnState				= TURNSTATE_PRE;
	mHighlightsOn			= false;
}

//--------------------------------------------------------------------------------------------------

GameScreen_Chess::~GameScreen_Chess()
{
	delete mPlayers[COLOUR_WHITE];
	mPlayers[COLOUR_WHITE] = NULL;

	delete mPlayers[COLOUR_BLACK];
	mPlayers[COLOUR_BLACK] = NULL;

	delete mLastMove;
	mLastMove = NULL;

	delete mSearchDepth;
	mSearchDepth = NULL;
}

//--------------------------------------------------------------------------------------------------

void GameScreen_Chess::Render()
{
	RenderBoard();

	RenderPreTurnText();

	if(mPlayers[mPlayerTurn]->GetMoveType() == PAWN_PROMOTION)
		mPlayers[mPlayerTurn]->RenderPawnPromotion();
}

//--------------------------------------------------------------------------------------------------

void GameScreen_Chess::Update(size_t deltaTime, SDL_Event e)
{
	//Additional input outside of player.
	switch(e.type)
	{
		//Deal with mouse click input.
		case SDL_KEYUP:
			switch(e.key.keysym.sym)
			{
				case SDLK_h:
					mHighlightsOn = !mHighlightsOn;
				break;

				case SDLK_UP:
					*mSearchDepth += 1;
					cout << endl << "Search Depth = " << *mSearchDepth << endl;
				break;

				case SDLK_DOWN:
					*mSearchDepth = max(1, *mSearchDepth-1);
					cout << endl << "Search Depth = " << *mSearchDepth << endl;
				break;
			}
		break;

		default:
		break;
	}
	
	//Game progression.
	switch(mTurnState)
	{
		case TURNSTATE_PRE:
			//Lets find out what state the player finds itself in.
			mGameState			= mPlayers[mPlayerTurn]->PreTurn();
			mTurnState			= TURNSTATE_PLAY;
			mPreTurnTextTimer	= kPreTurnTextTime;

			//Output via MoveManager.
			MoveManager::Instance()->OutputGameState(mGameState, mPlayerTurn);
		break;
	
		case TURNSTATE_PLAY:
			if(mGameState == GAMESTATE_NORMAL || mGameState == GAMESTATE_CHECK)
			{
				//Tick down the time the text is on screen.
				mPreTurnTextTimer -= deltaTime;

				//Take the turn, and when it returns true a turn has been taken.
				if(mPlayers[mPlayerTurn]->TakeATurn(e))
					mTurnState = TURNSTATE_POST;
			}
			else if(mGameState == GAMESTATE_CHECKMATE)
			{
				//SDL_Quit();
			}
			else if(mGameState == GAMESTATE_STALEMATE)
			{
				//SDL_Quit();
			}
		break;

		case TURNSTATE_POST:
			//Let the player clean up after itself.
			mPlayers[mPlayerTurn]->EndTurn();

			//Change players.
			mPlayerTurn = mPlayerTurn == COLOUR_WHITE ? COLOUR_BLACK : COLOUR_WHITE;

			mTurnState = TURNSTATE_PRE;
		break;
	}
}

//--------------------------------------------------------------------------------------------------

void GameScreen_Chess::RenderBoard()
{
	//Draw the black and white board.
	mBackgroundTexture->Render(Vector2D());

	//Draw each piece.
	for(int x = 0; x < kBoardDimensions; x++)
	{
		for(int y = 0; y < kBoardDimensions; y++)
		{
			BoardPiece piece = mChessBoard->currentLayout[x][y];
			RenderPiece(piece, Vector2D(x*kChessPieceDimensions, y*kChessPieceDimensions));
		}
	}

	//Draw the highlight over the selected piece.
	if(mSelectedPiecePosition.x != -1 && mSelectedPiecePosition.y != -1)
	{
		SDL_Rect destRect = {(int)mSelectedPiecePosition.x*kChessPieceDimensions, (int)mSelectedPiecePosition.y*kChessPieceDimensions, kChessPieceDimensions, kChessPieceDimensions};
		SDL_Rect portionOfSpritesheet = {0, 0, kChessPieceDimensions, kChessPieceDimensions};
		portionOfSpritesheet.y = mChessBoard->currentLayout[(int)mSelectedPiecePosition.x][(int)mSelectedPiecePosition.y].piece * kChessPieceDimensions;
		mSelectedPieceSpritesheet->Render(portionOfSpritesheet, destRect);
	}

	//Draw highlights.
	RenderHighlights();
}

//--------------------------------------------------------------------------------------------------

void GameScreen_Chess::RenderPiece(BoardPiece boardPiece, Vector2D position)
{
	Texture2D* spritesheet;
	switch(boardPiece.colour)
	{
		case COLOUR_WHITE:
			spritesheet = mWhitePiecesSpritesheet;
		break;

		case COLOUR_BLACK:
			spritesheet = mBlackPiecesSpritesheet;
		break;
		
		case COLOUR_NONE:
			return;
		break;

		default:
			return;
		break;
	};

	SDL_Rect destRect = {(int)position.x, (int)position.y, kChessPieceDimensions, kChessPieceDimensions};
	SDL_Rect portionOfSpritesheet = {0, 0, kChessPieceDimensions, kChessPieceDimensions};
	portionOfSpritesheet.y = boardPiece.piece * kChessPieceDimensions;

	spritesheet->Render(portionOfSpritesheet, destRect);

}

//--------------------------------------------------------------------------------------------------

void GameScreen_Chess::RenderHighlights()
{
	//Always highlight previous move, if there is one.
	if(MoveManager::Instance()->HasRecordedMoves())
	{
		SDL_Rect destRectStart = {(int)MoveManager::Instance()->GetLastMove().from_X*kChessPieceDimensions, (int)MoveManager::Instance()->GetLastMove().from_Y*kChessPieceDimensions, kChessPieceDimensions, kChessPieceDimensions};
		SDL_Rect destRectEnd   = {(int)MoveManager::Instance()->GetLastMove().to_X*kChessPieceDimensions, (int)MoveManager::Instance()->GetLastMove().to_Y*kChessPieceDimensions, kChessPieceDimensions, kChessPieceDimensions};
		SDL_Rect portionOfSpritesheet = {0, 0, kChessPieceDimensions, kChessPieceDimensions};
		
		mPreviousMoveHighlightSpritesheet->Render(portionOfSpritesheet, destRectStart);
		mPreviousMoveHighlightSpritesheet->Render(portionOfSpritesheet, destRectEnd);
	}

	if( mHighlightsOn )
	{
		if( mPlayers[mPlayerTurn]->GetMoveType() == SELECT_A_POSITION )
		{
			for( unsigned int i = 0; i < mHighlightPositions.size(); i++ )
			{
				SDL_Rect destRect = {(int)mHighlightPositions[i].x*kChessPieceDimensions, (int)mHighlightPositions[i].y*kChessPieceDimensions, kChessPieceDimensions, kChessPieceDimensions};
				SDL_Rect portionOfSpritesheet = {0, 0, kChessPieceDimensions, kChessPieceDimensions};
		
				mSquareHighlightSpritesheet->Render(portionOfSpritesheet, destRect);
			}
		}	
	}
}

//--------------------------------------------------------------------------------------------------

void GameScreen_Chess::RenderPreTurnText()
{
	if(mPreTurnTextTimer > 0.0f)
	{
		//Render text.
		SDL_Rect destRect = {(int)(kChessScreenWidth*0.2f), (int)(kChessScreenHeight*0.25f), kPreTurnTextWidth, kPreTurnTextHeight};
		SDL_Rect portionOfSpritesheet = {0, mGameState*kPreTurnTextHeight, kPreTurnTextWidth, kPreTurnTextHeight};

		//If AI turn, lets modify the text to inform the human player why the game seems to have frozen.
		//AI is always BLACK at the moment.
		if(mAIPlayerPlaying && mPlayerTurn == COLOUR_BLACK && mGameState == GAMESTATE_NORMAL)
			portionOfSpritesheet.y = kPreTurnTextHeight;

		mGameStateSpritesheet->Render(portionOfSpritesheet, destRect);

		//Render last move in highlighted squares.
		SDL_Rect portionOfHighlightSpritesheet = {0, 0, kChessPieceDimensions, kChessPieceDimensions};
		
		SDL_Rect destRect_FromPosition = {mLastMove->from_X*kChessPieceDimensions, mLastMove->from_Y*kChessPieceDimensions, kChessPieceDimensions, kChessPieceDimensions};
		mSquareHighlightSpritesheet->Render(portionOfHighlightSpritesheet, destRect_FromPosition);
		SDL_Rect destRect_ToPosition = {mLastMove->to_X*kChessPieceDimensions, mLastMove->to_Y*kChessPieceDimensions, kChessPieceDimensions, kChessPieceDimensions};
		mSquareHighlightSpritesheet->Render(portionOfHighlightSpritesheet, destRect_ToPosition);
	}
}

//--------------------------------------------------------------------------------------------------