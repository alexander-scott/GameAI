//------------------------------------------------------------------------
//  Author: Paul Roberts 2016
//------------------------------------------------------------------------

#ifndef _CHESSPLAYERAI_H
#define _CHESSPLAYERAI_H

#include "ChessPlayer.h"
#include "ChessCommons.h"
#include "../Commons.h"
#include <SDL.h>

class ChessPlayerAI : public ChessPlayer
{
//--------------------------------------------------------------------------------------------------
public:
	ChessPlayerAI(SDL_Renderer* renderer, COLOUR colour, Board* board, vector<Vector2D>* highlights, Vector2D* selectedPiecePosition, Move* lastMove, int* searchDepth);
	~ChessPlayerAI();

	bool		TakeATurn(SDL_Event e);

//--------------------------------------------------------------------------------------------------
protected:
	int  MiniMax(Board board, int depth, Move* bestMove);
	int  Maximise(Board board, int depth, Move* bestMove, int alphaBeta);
	int  Minimise(Board board, int depth, Move* bestMove, int alphaBeta);
	bool MakeAMove(Move* move);

	void OrderMoves(Board board, vector<Move>* moves, bool highToLow);
	void CropMoves(vector<Move>* moves, unsigned int maxNumberOfMoves);

	int  ScoreTheBoard(Board boardToScore);

private:
	int* mDepthToSearch;
};


#endif //_CHESSPLAYERAI_H