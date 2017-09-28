//------------------------------------------------------------------------
//  Author: Paul Roberts 2016
//------------------------------------------------------------------------

#ifndef CHESSCOMMONS_H
#define CHESSCOMMONS_H

#include "ChessConstants.h"
#include "../Commons.h"

//--------------------------------------------------------------------------------------------------

enum MOVETYPE
{
	SELECT_A_PIECE = 0,
	SELECT_A_POSITION = 1,
	PAWN_PROMOTION
};

//--------------------------------------------------------------------------------------------------

enum PIECE
{
	PIECE_PAWN,
	PIECE_KNIGHT,
	PIECE_BISHOP,
	PIECE_ROOK,
	PIECE_QUEEN,
	PIECE_KING,

	PIECE_NONE
};

//--------------------------------------------------------------------------------------------------

enum COLOUR
{
	COLOUR_WHITE,
	COLOUR_BLACK,

	COLOUR_NONE
};

//--------------------------------------------------------------------------------------------------

struct BoardPiece
{
	PIECE   piece;
	COLOUR  colour;
	bool	canEnPassant;
	bool	hasMoved;			//Required for Castling move.

	BoardPiece()
	{
		piece		 = PIECE_NONE;
		colour		 = COLOUR_NONE;
		canEnPassant = false;
		hasMoved	 = false;
	};

	BoardPiece(PIECE type, COLOUR col)
	{
		piece		 = type;
		colour		 = col;
		canEnPassant = false;
		hasMoved	 = false;
	};
};

//--------------------------------------------------------------------------------------------------

struct Board
{
	int			score;
	BoardPiece	currentLayout[8][8];

	Board()
	{
		//Top row - White
		currentLayout[0][0] = BoardPiece(PIECE_ROOK, COLOUR_BLACK);
		currentLayout[1][0] = BoardPiece(PIECE_KNIGHT, COLOUR_BLACK);
		currentLayout[2][0] = BoardPiece(PIECE_BISHOP, COLOUR_BLACK);
		currentLayout[3][0] = BoardPiece(PIECE_QUEEN, COLOUR_BLACK);
		currentLayout[4][0] = BoardPiece(PIECE_KING, COLOUR_BLACK);
		currentLayout[5][0] = BoardPiece(PIECE_BISHOP, COLOUR_BLACK);
		currentLayout[6][0] = BoardPiece(PIECE_KNIGHT, COLOUR_BLACK);
		currentLayout[7][0] = BoardPiece(PIECE_ROOK, COLOUR_BLACK);

		//Second row - White Pawns
		for(int x = 0; x < kBoardDimensions; x++)
			currentLayout[x][1] = BoardPiece(PIECE_PAWN, COLOUR_BLACK);
		
		//Bottom row - Black
		currentLayout[0][7] = BoardPiece(PIECE_ROOK, COLOUR_WHITE);
		currentLayout[1][7] = BoardPiece(PIECE_KNIGHT, COLOUR_WHITE);
		currentLayout[2][7] = BoardPiece(PIECE_BISHOP, COLOUR_WHITE);
		currentLayout[3][7] = BoardPiece(PIECE_QUEEN, COLOUR_WHITE);
		currentLayout[4][7] = BoardPiece(PIECE_KING, COLOUR_WHITE);
		currentLayout[5][7] = BoardPiece(PIECE_BISHOP, COLOUR_WHITE);
		currentLayout[6][7] = BoardPiece(PIECE_KNIGHT, COLOUR_WHITE);
		currentLayout[7][7] = BoardPiece(PIECE_ROOK, COLOUR_WHITE);

		//Second row - Black Pawns
		for(int x = 0; x < kBoardDimensions; x++)
			currentLayout[x][6] = BoardPiece(PIECE_PAWN, COLOUR_WHITE);

	};

	Board(BoardPiece setup[8][8])
	{
		for(int x = 0; x < kBoardDimensions; x++)
		{
			for(int y = 0; y < kBoardDimensions; y++)
			{
				currentLayout[x][y] = setup[x][y];
			}
		}
	};
};

//--------------------------------------------------------------------------------------------------

struct Move
{
	int from_X;
	int from_Y;
	int to_X;
	int to_Y;
	int score;	//Required only for ordering moves.

	Move()
	{
	}

	Move(int fromX, int fromY, int toX, int toY)
	{
		from_X = fromX;
		from_Y = fromY;
		to_X   = toX;
		to_Y   = toY;
		score  = 0;
	};

	Move(Vector2D fromPosition, Vector2D toPosition)
	{
		from_X = (int)fromPosition.x;
		from_Y = (int)fromPosition.y;
		to_X   = (int)toPosition.x;
		to_Y   = (int)toPosition.y;
		score  = 0;
	};
};

//--------------------------------------------------------------------------------------------------

enum GAMESTATE
{
	GAMESTATE_NORMAL    = 0,	//Numbers used for preTurnText
	GAMESTATE_CHECK     = 2,
	GAMESTATE_CHECKMATE = 3,
	GAMESTATE_STALEMATE = 4
};

//--------------------------------------------------------------------------------------------------

enum TURNSTATE
{
	TURNSTATE_PRE,
	TURNSTATE_PLAY,
	TURNSTATE_POST
};

//--------------------------------------------------------------------------------------------------

struct ChessMove
{
	Move   theMove;
	string startPosition;
	string endPosition;
};

//--------------------------------------------------------------------------------------------------

#endif //CHESSCOMMONS_H