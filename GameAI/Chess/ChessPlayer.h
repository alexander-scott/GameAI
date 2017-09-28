//------------------------------------------------------------------------
//  Author: Paul Roberts 2016
//------------------------------------------------------------------------

#ifndef _CHESSPLAYER_H
#define _CHESSPLAYER_H

#include "ChessCommons.h"
#include "../Commons.h"
#include <SDL.h>
#include <vector>
using namespace::std;

class Texture2D;

class ChessPlayer
{
//--------------------------------------------------------------------------------------------------
public:
	ChessPlayer(SDL_Renderer* renderer, COLOUR colour, Board* board, vector<Vector2D>* highlights, Vector2D* selectedPiecePosition, Move* lastMove);
	~ChessPlayer();

	COLOUR	 GetColour()			{return mTeamColour;}
	MOVETYPE GetMoveType()			{return mCurrentMove;}

//--------------------------------------------------------------------------------------------------	
public:
	virtual GAMESTATE	PreTurn();
	virtual bool		TakeATurn(SDL_Event e);
	virtual void		EndTurn();

	void				RenderPawnPromotion();

//--------------------------------------------------------------------------------------------------
protected:
	virtual bool MakeAMove(Vector2D boardPosition);

	void GetNumberOfLivingPieces();
	void GetMoveOptions(Vector2D piecePosition, BoardPiece boardPiece, Board boardToTest, vector<Move>* moves);
	void GetAllMoveOptions(Board boardToTest, COLOUR teamColour, vector<Move>* moves);
	bool CheckMoveOptionValidityAndStoreMove(Move moveToCheck, COLOUR pieceColour, Board boardToTest, vector<Move>* moves);

	void GetPawnMoveOptions(Vector2D piecePosition, BoardPiece boardPiece, Board boardToTest, vector<Move>* moves);
	void GetHorizontalAndVerticalMoveOptions(Vector2D piecePosition, BoardPiece boardPiece, Board boardToTest, vector<Move>* moves);
	void GetDiagonalMoveOptions(Vector2D piecePosition, BoardPiece boardPiece, Board boardToTest, vector<Move>* moves);
	void GetKnightMoveOptions(Vector2D piecePosition, BoardPiece boardPiece, Board boardToTest, vector<Move>* moves);
	void GetKingMoveOptions(Vector2D piecePosition, BoardPiece boardPiece, Board boardToTest, vector<Move>* moves);

	void ClearEnPassant();
	bool CheckForCheck(Board boardToTest, COLOUR teamColour); 
	bool CheckForCheckmate(Board boardToTest, COLOUR teamColour);
	bool CheckForStalemate(Board boardToCheck, COLOUR teamColour);

//--------------------------------------------------------------------------------------------------
protected:
	Texture2D*		  mSelectAPieceTexture;
	SDL_Rect		  mPawnPromotionDrawPosition;

	Board*			  mChessBoard;
	COLOUR			  mTeamColour;
	vector<Vector2D>* mHighlightPositions;		//Screen positioning, NOT board positioning.
	MOVETYPE		  mCurrentMove;
	Vector2D*		  mSelectedPiecePosition;

	int				  mNumberOfLivingPieces;

	bool			  mInCheck;

	Move*			  mLastMove;
};


#endif //_CHESSPLAYER_H