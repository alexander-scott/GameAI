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

const int kPawnScore		= 10;
const int kKnightScore		= 80;
const int kBishopScore		= 80;
const int kRookScore		= 150;
const int kQueenScore		= 200;
const int kKingScore		= 1000000;

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

	mMaxBestMove = nullptr;

	MiniMax(*mChessBoard, 0, mMaxBestMove);

	bool gameStillActive = MakeAMove(mMaxBestMove);
	currentTime = GetTickCount();
	cout << " - [AI Time taken: " << std::setprecision(10) << (currentTime-startTime)/1000.0f << " seconds]";
	return gameStillActive;

	//-----------------------------------------------------------

}

//--------------------------------------------------------------------------------------------------

int ChessPlayerAI::MiniMax(Board board, int depth, Move* bestMove)
{
	int score = Maximise(board, depth, mMaxBestMove, 10000000);
	return score;
}

//--------------------------------------------------------------------------------------------------

// Find best score for AI
int ChessPlayerAI::Maximise(Board board, int depth, Move* bestMove, int beta)
{
	// If the depth limit is reached, don't go any deeper and return the score of the board
	if (depth >= *mDepthToSearch) 
		return ScoreTheBoard(board);

	// Get all the available move options for the current board
	vector<Move>* moves = new vector<Move>;
	GetAllMoveOptions(board, (mTeamColour == COLOUR_WHITE ? COLOUR_WHITE : COLOUR_BLACK), moves);

	int alpha = -10000000; // Set alpha to low num
	Move* bestChildMove = nullptr;

	// Iterate through every available move
	vector<Move>::iterator it;
	for (it = moves->begin(); it != moves->end(); it++) 
	{
		// Create a copy of the board and then perform the current move
		Board newBoard = Board(board);
		newBoard.currentLayout[it->from_X][it->from_Y].hasMoved = true;
		newBoard.currentLayout[it->to_X][it->to_Y] = mChessBoard->currentLayout[it->from_X][it->from_Y];
		newBoard.currentLayout[it->from_X][it->from_Y] = BoardPiece();

		// Go down a layer and find the minimum score available for the enemy (PLAYER). Set it to alpha if it is greater than a previous alpha.
		alpha = max(alpha, Minimise(newBoard, depth + 1, bestChildMove, alpha));

		// If the new alpha is greater than the previous best score (or hasn't been set yet)
		if (bestChildMove == nullptr || alpha > bestChildMove->score)
		{
			if (bestChildMove != nullptr) // Free memory
				delete bestChildMove;

			bestChildMove = new Move((*it));
			bestChildMove->score = alpha;
		}

		// If a move is found to be worse than the best score currently stored, return out of the tree early.
		if (alpha > beta)
		{
			delete moves;
			return alpha;
		}
	}

	delete moves;

	// If we're on the top layer, return the best move available
	if (depth == 0)
	{
		mMaxBestMove = bestChildMove;
	}

	return alpha;
}

//--------------------------------------------------------------------------------------------------

// Find the best score for player
int ChessPlayerAI::Minimise(Board board, int depth, Move* bestMove, int alpha)
{
	if (depth >= *mDepthToSearch) 
		return ScoreTheBoard(board);

	vector<Move>* moves = new vector<Move>;
	GetAllMoveOptions(board, (mTeamColour == COLOUR_WHITE ? COLOUR_BLACK : COLOUR_WHITE), moves);

	int beta = 10000000;
	Move* bestChildMove = nullptr;

	vector<Move>::iterator it;
	for (it = moves->begin(); it != moves->end(); it++)
	{
		Board newBoard = Board(board);
		newBoard.currentLayout[it->from_X][it->from_Y].hasMoved = true;
		newBoard.currentLayout[it->to_X][it->to_Y] = mChessBoard->currentLayout[it->from_X][it->from_Y];
		newBoard.currentLayout[it->from_X][it->from_Y] = BoardPiece();

		beta = min(beta, Maximise(newBoard, depth + 1, &(*it), beta));

		if (bestChildMove == nullptr || beta < bestChildMove->score)
		{
			if (bestChildMove != nullptr)
				delete bestChildMove;
				
			bestChildMove = new Move((*it));
			bestChildMove->score = beta;
		}

		if (beta < alpha)
		{
			delete moves;
			return beta;
		}
	}

	delete moves;

	return beta;
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
	float numQueensPlayer = 0, numQueensAI = 0, numRooksPlayer = 0, numRooksAI = 0, numBishopsPlayer = 0, 
		numBishopsAI = 0, numKnightsPlayer = 0, numKnightsAI = 0, numPawnsPlayer = 0, numPawnsAI = 0;

	float score = 0;

	for (int x = 0; x < kBoardDimensions; x++)
	{
		for (int y = 0; y < kBoardDimensions; y++)
		{
			//Check for pieces.
			BoardPiece currentPiece = boardToScore.currentLayout[x][y];

			// AI
			if (currentPiece.colour == mTeamColour && currentPiece.piece != PIECE_NONE)
			{
				switch (currentPiece.piece)
				{
				case PIECE_PAWN:
					score += pawnScoreTableAI[x][y];
					numPawnsAI++;
					break;

				case PIECE_KNIGHT:
					score += knightScoreTableAI[x][y];
					numKnightsAI++;
					break;

				case PIECE_BISHOP:
					score += bishopScoreTableAI[x][y];
					numBishopsAI++;
					break;

				case PIECE_ROOK:
					score += rookScoreTableAI[x][y];
					numRooksAI++;
					break;

				case PIECE_QUEEN:
					score += queenScoreTableAI[x][y];
					numQueensAI++;
					break;

				case PIECE_KING:
					score += kKingScore;
					score += kingMiddleGameScoreTableAI[x][y];
					break;
				}
			}

			// PLAYER
			if (currentPiece.colour != mTeamColour && currentPiece.piece != PIECE_NONE)
			{
				switch (currentPiece.piece)
				{
				case PIECE_PAWN:
					score -= pawnScoreTablePlayer[x][y];
					numPawnsPlayer++;
					break;

				case PIECE_KNIGHT:
					score -= knightScoreTableAI[x][y];
					numKnightsPlayer++;
					break;

				case PIECE_BISHOP:
					score -= bishopScoreTablePlayer[x][y];
					numBishopsPlayer++;
					break;

				case PIECE_ROOK:
					score -= rookScoreTablePlayer[x][y];
					numRooksPlayer++;
					break;

				case PIECE_QUEEN:
					score -= queenScoreTablePlayer[x][y];
					numQueensPlayer++;
					break;

				case PIECE_KING:
					score -= kKingScore;
					score -= kingMiddleGameScoreTablePlayer[x][y];
					break;
				}
			}
		}
	}

	score += (kQueenScore * (numQueensAI - numQueensPlayer)) +
			(kRookScore * (numRooksAI - numRooksPlayer)) +
			(kBishopScore *(numBishopsAI - numBishopsPlayer)) +
			(kKnightScore * (numKnightsAI - numKnightsPlayer)) +
			(kPawnScore * (numPawnsAI - numPawnsPlayer));

	return score;
}

//--------------------------------------------------------------------------------------------------
