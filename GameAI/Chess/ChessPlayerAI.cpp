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

	_maxBestMove = nullptr;
	_minBestMove = nullptr;

	MiniMax(*mChessBoard, 0, _maxBestMove);

	bool gameStillActive = MakeAMove(_maxBestMove);
	currentTime = GetTickCount();
	cout << " - [AI Time taken: " << std::setprecision(10) << (currentTime-startTime)/1000.0f << " seconds]";
	return gameStillActive;

	//-----------------------------------------------------------

}

//--------------------------------------------------------------------------------------------------

int ChessPlayerAI::MiniMax(Board board, int depth, Move* bestMove)
{
	int score = Maximise(board, depth, _maxBestMove, MaxInt);
	return score;
}

//--------------------------------------------------------------------------------------------------

int ChessPlayerAI::Maximise(Board board, int depth, Move* bestMove, int parentLow)
{
	//TODO: Code Maximise function.

	if (depth >= *mDepthToSearch) //if depth limit is reached
	{
		return ScoreTheBoard(board);
	}
	else 
	{
		vector<Move>* moves = new vector<Move>;
		GetAllMoveOptions(board, (mTeamColour == COLOUR_WHITE ? COLOUR_WHITE : COLOUR_BLACK), moves);

		int currentScore = -MaxInt;

		vector<Move>::iterator it;
		for (it = moves->begin(); it != moves->end(); it++) 
		{
			Board newBoard = Board(board);
			newBoard.currentLayout[it->from_X][it->from_Y].hasMoved = true;
			newBoard.currentLayout[it->to_X][it->to_Y] = mChessBoard->currentLayout[it->from_X][it->from_Y];
			newBoard.currentLayout[it->from_X][it->from_Y] = BoardPiece();

			int newScore = Minimise(newBoard, depth + 1, &(*it), parentLow);

			if (currentScore < newScore)
			{
				currentScore = newScore;
				_maxBestMove = new Move((*it));
				_maxBestMove->score = currentScore;
			}

			//currentScore = max(currentScore, Minimise(newBoard, depth + 1, &(*it), parentLow));

			/*if (bestMove == NULL || bestMove->score > currentScore)
			{
				bestMove = new Move((*it));
				bestMove->score = currentScore;
			}*/

			/*if (parentLow <= bestMove->score)
			{
			break;
			}	*/
		}

		//for (int i = 0; i < moves->size(); i++)
		//{
		//	Board newBoard = Board(board);
		//	newBoard.currentLayout[moves->at(i).from_X][moves->at(i).from_Y].hasMoved = true;
		//	newBoard.currentLayout[moves->at(i).to_X][moves->at(i).to_Y] = mChessBoard->currentLayout[moves->at(i).from_X][moves->at(i).from_Y];
		//	newBoard.currentLayout[moves->at(i).from_X][moves->at(i).from_Y] = BoardPiece();

		//	currentScore = max(currentScore, Minimise(newBoard, depth + 1, &moves->at(i), parentLow));

		//	if (bestMove == NULL || bestMove->score > currentScore)
		//	{
		//		bestMove = new Move(moves->at(i));
		//		bestMove->score = currentScore;
		//	}

		//	/*if (parentLow <= bestMove->score)
		//	{
		//		break;
		//	}	*/
		//}

		return currentScore;
	}
}

//--------------------------------------------------------------------------------------------------

int ChessPlayerAI::Minimise(Board board, int depth, Move* bestMove, int parentHigh)
{
	//TODO: Code Minimise function.

	if (depth >= *mDepthToSearch) {//if depth limit is reached
		return ScoreTheBoard(board);
	}
	else
	{
		vector<Move>* moves = new vector<Move>;
		GetAllMoveOptions(board, (mTeamColour == COLOUR_WHITE ? COLOUR_BLACK : COLOUR_WHITE), moves);

		int currentScore = MaxInt;

		vector<Move>::iterator it;
		for (it = moves->begin(); it != moves->end(); it++)
		{
			Board newBoard = Board(board);
			newBoard.currentLayout[it->from_X][it->from_Y].hasMoved = true;
			newBoard.currentLayout[it->to_X][it->to_Y] = mChessBoard->currentLayout[it->from_X][it->from_Y];
			newBoard.currentLayout[it->from_X][it->from_Y] = BoardPiece();

			int newScore = Maximise(newBoard, depth + 1, &(*it), parentHigh);

			if (currentScore > newScore)
			{
				currentScore = newScore;
				_minBestMove = new Move((*it));
				_minBestMove->score = currentScore;
			}

			/*currentScore = min(currentScore, Maximise(newBoard, depth + 1, &(*it), parentHigh));

			if (bestMove == NULL || bestMove->score < currentScore)
			{
				bestMove = new Move((*it));
				bestMove->score = currentScore;
			}*/
		}

		//for (int i = 0; i < moves->size(); i++)
		//{
		//	Board newBoard = Board(board);
		//	newBoard.currentLayout[moves->at(i).from_X][moves->at(i).from_Y].hasMoved = true;
		//	newBoard.currentLayout[moves->at(i).to_X][moves->at(i).to_Y] = mChessBoard->currentLayout[moves->at(i).from_X][moves->at(i).from_Y];
		//	newBoard.currentLayout[moves->at(i).from_X][moves->at(i).from_Y] = BoardPiece();

		//	currentScore = min(currentScore, Maximise(newBoard, depth + 1, &moves->at(i), parentHigh));

		//	if (bestMove == NULL || bestMove->score < currentScore)
		//	{
		//		bestMove = new Move(moves->at(i));
		//		bestMove->score = currentScore;
		//	}

		//	/*if (parentHigh >= bestMove->score)
		//	{
		//		break;
		//	}*/
		//}

		return currentScore;
	}
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

	for (int i = 0; i < moves->size(); i++)
	{

	}
}

//--------------------------------------------------------------------------------------------------

int ChessPlayerAI::ScoreTheBoard(Board boardToScore)
{
	//TODO

	return (1 + rand() % static_cast<int>(1000 + 1));

	int score = 0;

	for (int x = 0; x < kBoardDimensions; x++)
	{
		for (int y = 0; y < kBoardDimensions; y++)
		{
			//Check for pieces.
			BoardPiece currentPiece = boardToScore.currentLayout[x][y];
			if (currentPiece.colour == mTeamColour && currentPiece.piece != PIECE_NONE)
			{
				switch (currentPiece.piece)
				{
				case PIECE_PAWN:
					score += 100;
					break;

				case PIECE_KNIGHT:
					score += 300;
					break;

				case PIECE_BISHOP:
					score += 325;
					break;

				case PIECE_ROOK:
					score += 500;
					break;

				case PIECE_QUEEN:
					score += 900;
					break;

				case PIECE_KING:
					score += 1000000;
					break;
				}
			}
			if (currentPiece.colour != mTeamColour && currentPiece.piece != PIECE_NONE)
			{
				switch (currentPiece.piece)
				{
				case PIECE_PAWN:
					score -= 100;
					break;

				case PIECE_KNIGHT:
					score -= 300;
					break;

				case PIECE_BISHOP:
					score -= 325;
					break;

				case PIECE_ROOK:
					score -= 500;
					break;

				case PIECE_QUEEN:
					score -= 900;
					break;

				case PIECE_KING:
					score -= 1000000;
					break;
				}
			}
		}
	}

	return score;
}

//--------------------------------------------------------------------------------------------------
