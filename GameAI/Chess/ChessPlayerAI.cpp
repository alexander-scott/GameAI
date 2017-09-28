//------------------------------------------------------------------------
//  Author: Paul Roberts 2016
//------------------------------------------------------------------------

#include "ChessPlayerAI.h"
#include <iostream>		//cout
#include <SDL.h>
#include <iomanip>		//Precision
#include <algorithm>	//Sort
#include "ChessConstants.h"
#include "../Texture2D.h"
#include "ChessMoveManager.h"

using namespace::std;

//--------------------------------------------------------------------------------------------------

const int kPawnScore		= 1;
const int kKnightScore		= 1;
const int kBishopScore		= 1;
const int kRookScore		= 1;
const int kQueenScore		= 1;
const int kKingScore		= 1;

const int kCheckScore		= 1;
const int kCheckmateScore	= 1;
const int kStalemateScore	= 1;	//Tricky one because sometimes you want this, sometimes you don't.

const int kPieceWeight		= 1; //Scores as above.
const int kMoveWeight		= 1; //Number of moves available to pieces.
const int kPositionalWeight	= 1; //Whether in CHECK, CHECKMATE or STALEMATE.

//--------------------------------------------------------------------------------------------------

ChessPlayerAI::ChessPlayerAI(SDL_Renderer* renderer, COLOUR colour, Board* board, vector<Vector2D>* highlights, Vector2D* selectedPiecePosition, Move* lastMove, int* searchDepth)
	: ChessPlayer(renderer, colour, board, highlights, selectedPiecePosition, lastMove)
{
	mDepthToSearch = searchDepth;
}

//--------------------------------------------------------------------------------------------------

ChessPlayerAI::~ChessPlayerAI()
{
}

//--------------------------------------------------------------------------------------------------

bool ChessPlayerAI::MakeAMove(Move* move)
{	
	//If this was an en'passant move the taken piece will not be in the square we moved to.
	if( mChessBoard->currentLayout[move->from_X][move->from_Y].piece == PIECE_PAWN )
	{
		//If the pawn is on its start position and it double jumps, then en'passant may be available for opponent.
		if( (move->from_Y == 1 && move->to_Y == 3) || 
			(move->from_Y == 6 && move->to_Y == 4) )
		{
			mChessBoard->currentLayout[move->from_X][move->from_Y].canEnPassant = true;
		}
	}

	//En'Passant removal of enemy pawn.
	//If our pawn moved into an empty position to the left or right, then must be En'Passant.
	if( mChessBoard->currentLayout[move->from_X][move->from_Y].piece == PIECE_PAWN &&
		mChessBoard->currentLayout[move->to_X][move->to_Y].piece == PIECE_NONE )
	{
		int pawnDirectionOpposite = mTeamColour == COLOUR_WHITE ? -1 : 1;

		if( (move->from_X < move->to_X) ||
			(move->from_X > move->to_X) )
		{
			mChessBoard->currentLayout[move->to_X][move->to_Y+pawnDirectionOpposite] = BoardPiece();
		}
	}

	//CASTLING - Move the rook.
	if( mChessBoard->currentLayout[move->from_X][move->from_Y].piece == PIECE_KING )
	{
		//Are we moving 2 spaces??? This indicates CASTLING.
		if( move->to_X-move->from_X == 2 )
		{
			//Moving 2 spaces to the right - Move the ROOK on the right into its new position.
			mChessBoard->currentLayout[move->from_X+3][move->from_Y].hasMoved = true;
			mChessBoard->currentLayout[move->from_X+1][move->from_Y] = mChessBoard->currentLayout[move->from_X+3][move->from_Y];
			mChessBoard->currentLayout[move->from_X+3][move->from_Y] = BoardPiece();
		}
		else if( move->to_X-move->from_X == -2 )
		{
			//Moving 2 spaces to the left - Move the ROOK on the left into its new position.
			//Move the piece into new position.
			mChessBoard->currentLayout[move->from_X-4][move->from_Y].hasMoved = true;
			mChessBoard->currentLayout[move->from_X-1][move->from_Y] = mChessBoard->currentLayout[move->from_X-4][move->from_Y];
			mChessBoard->currentLayout[move->from_X-4][move->from_Y] = BoardPiece();
		}
	}

	//Move the piece into new position.
	mChessBoard->currentLayout[move->from_X][move->from_Y].hasMoved = true;
	mChessBoard->currentLayout[move->to_X][move->to_Y] = mChessBoard->currentLayout[move->from_X][move->from_Y];
	mChessBoard->currentLayout[move->from_X][move->from_Y] = BoardPiece();

	//Store the last move to output at start of turn.
	mLastMove->from_X = move->from_X;
	mLastMove->from_Y = move->from_Y;
	mLastMove->to_X = move->to_X;
	mLastMove->to_Y = move->to_Y;

	//Record the move.
	MoveManager::Instance()->StoreMove(*move);

	//Piece is in a new position.
	mSelectedPiecePosition->x = move->to_X;
	mSelectedPiecePosition->y = move->to_Y;

	//Check if we need to promote a pawn.
	if( mChessBoard->currentLayout[move->to_X][move->to_Y].piece == PIECE_PAWN && (move->to_Y == 0 || move->to_Y == 7) )
	{
		//Time to promote - Always QUEEN at the moment.
		PIECE newPieceType  = PIECE_QUEEN; 

		//Change the PAWN into the selected piece.
		mChessBoard->currentLayout[move->to_X][move->to_Y].piece = newPieceType;
	}

	//Not finished turn yet.
	return true;
}

//--------------------------------------------------------------------------------------------------

bool ChessPlayerAI::TakeATurn(SDL_Event e)
{
	//TODO: Code your own function - Remove this version after, it is only here to keep the game functioning for testing.

	DWORD startTime		 = GetTickCount();
	DWORD currentTime	 = 0;

	vector<Move> moves;
	GetAllMoveOptions(*mChessBoard, mTeamColour, &moves);
	Move* move = &moves.at(0);
	bool gameStillActive = MakeAMove(move);
	currentTime = GetTickCount();
	cout << " - [AI Time taken: " << std::setprecision(10) << (currentTime-startTime)/1000.0f << " seconds]";
	return gameStillActive;

	//-----------------------------------------------------------

}

//--------------------------------------------------------------------------------------------------

int ChessPlayerAI::MiniMax(Board board, int depth, Move* bestMove)
{
	return Maximise(board, depth, bestMove, MaxInt);
}

//--------------------------------------------------------------------------------------------------

int ChessPlayerAI::Maximise(Board board, int depth, Move* bestMove, int parentLow)
{
	//TODO: Code Maximise function.
	return 0;
}

//--------------------------------------------------------------------------------------------------

int ChessPlayerAI::Minimise(Board board, int depth, Move* bestMove, int parentHigh)
{
	//TODO: Code Minimise function.
	return 0;
}

//--------------------------------------------------------------------------------------------------

void ChessPlayerAI::OrderMoves(Board board, vector<Move>* moves, bool highToLow)
{
	//TODO
}

//--------------------------------------------------------------------------------------------------

void ChessPlayerAI::CropMoves(vector<Move>* moves, unsigned int maxNumberOfMoves)
{
	//TODO
}

//--------------------------------------------------------------------------------------------------

int ChessPlayerAI::ScoreTheBoard(Board boardToScore)
{
	//TODO
	return 0;
}

//--------------------------------------------------------------------------------------------------
