//------------------------------------------------------------------------
//  Author: Paul Roberts 2016
//------------------------------------------------------------------------

#ifndef CHESSCONSTANTS_H
#define CHESSCONSTANTS_H

#include <iostream>
using namespace::std;

//File paths.
const string kChessBoardPath				= "Images/Chess/ChessBoard.png";
const string kChessWhitePiecesPath			= "Images/Chess/WhitePieces.png";
const string kChessBlackPiecesPath			= "Images/Chess/BlackPieces.png";
const string kSelectedChessPiecePath		= "Images/Chess/SelectedPieceHighlight.png";
const string kChessSquareRedHighlightPath	= "Images/Chess/SquareHighlight.png";
const string kChessSquareBlueHighlightPath	= "Images/Chess/PreviousMoveHighlight.png";
const string kChessGameStatesPath			= "Images/Chess/GameState.png";
const string kChessSelectAPiecePath			= "Images/Chess/SelectAPiece.png";

//Screen dimensions.
const int kChessScreenWidth					= 416;		//In pixels.
const int kChessScreenHeight				= 416;

const int kChessPieceDimensions				= 52;		//Width & Height of image in pixels
const int kBoardDimensions					= 8;		//8x8 grid.

//Spritesheet / sprite animations.
const int   kNumberOfPieces					= 6;

//PreTurn text.
const float	kPreTurnTextTime				= 1.5f;
const int kPreTurnTextWidth					= 240;
const int kPreTurnTextHeight				= 52;

//Search depth in MiniMax Algorithm.
const unsigned int kSearchDepth				= 3;

//Cut the number of moves down per ply.
//This will be multiplied by current depth.
const unsigned int kMaxMovesPerPly			= 20;

//Number of pieces at start of game.
const unsigned int kTotalNumberOfStartingPieces = 16;

// Friendly score tables
const float pawnScoreTablePlayer[8][8] = {			{ 0,  0,  0,  0,  0,  0,  0,  0, },
												{ 50, 50, 50, 50, 50, 50, 50, 50, },
												{ 10, 10, 20, 30, 30, 20, 10, 10, } ,
												{ 5,  5, 10, 25, 25, 10,  5,  5, } ,
												{ 0,  0,  0, 20, 20,  0,  0,  0, } ,
												{ 5, -5,-10,  0,  0,-10, -5,  5, } ,
												{ 5, 10, 10,-20,-20, 10, 10,  5, } ,
												{ 0,  0,  0,  0,  0,  0,  0,  0 } };

const float knightScoreTablePlayer[8][8] = {			{ -50,-40,-30,-30,-30,-30,-40,-50, },
												{ -40,-20,  0,  0,  0,  0,-20,-40, },
												{ -30,  0, 10, 15, 15, 10,  0,-30, } ,
												{ -30,  5, 15, 20, 20, 15,  5,-30, } ,
												{ -30,  0, 15, 20, 20, 15,  0,-30, } ,
												{ -30,  5, 10, 15, 15, 10,  5,-30, } ,
												{ -40,-20,  0,  5,  5,  0,-20,-40, } ,
												{ -50,-40,-30,-30,-30,-30,-40,-50, } };

const float bishopScoreTablePlayer[8][8] = {			{ -20,-10,-10,-10,-10,-10,-10,-20, },
												{ -10,  0,  0,  0,  0,  0,  0,-10, },
												{ -10,  0,  5, 10, 10,  5,  0,-10, } ,
												{ -10,  5,  5, 10, 10,  5,  5,-10, } ,
												{ -10,  0, 10, 10, 10, 10,  0,-10, } ,
												{ -10, 10, 10, 10, 10, 10, 10,-10, } ,
												{ -10,  5,  0,  0,  0,  0,  5,-10, } ,
												{ -20,-10,-10,-10,-10,-10,-10,-20, } };

const float rookScoreTablePlayer[8][8] = {			{ 0,  0,  0,  0,  0,  0,  0,  0, },
												{ 5, 10, 10, 10, 10, 10, 10,  5, },
												{ -5,  0,  0,  0,  0,  0,  0, -5, } ,
												{ -5,  0,  0,  0,  0,  0,  0, -5, } ,
												{ -5,  0,  0,  0,  0,  0,  0, -5, } ,
												{ -5,  0,  0,  0,  0,  0,  0, -5, } ,
												{ -5,  0,  0,  0,  0,  0,  0, -5, } ,
												{ 0,  0,  0,  5,  5,  0,  0,  0 } };

const float queenScoreTablePlayer[8][8] = {			{ -20,-10,-10, -5, -5,-10,-10,-20, },
												{ -10,  0,  0,  0,  0,  0,  0,-10, },
												{ -10,  0,  5,  5,  5,  5,  0,-10, } ,
												{ -5,  0,  5,  5,  5,  5,  0, -5, } ,
												{ 0,  0,  5,  5,  5,  5,  0, -5, } ,
												{ -10,  5,  5,  5,  5,  5,  0,-10, } ,
												{ -10,  0,  5,  0,  0,  0,  0,-10, } ,
												{ -20,-10,-10, -5, -5,-10,-10,-20 } };

const float kingMiddleGameScoreTablePlayer[8][8] = {	{ -30,-40,-40,-50,-50,-40,-40,-30, },
												{ -30,-40,-40,-50,-50,-40,-40,-30, },
												{ -30,-40,-40,-50,-50,-40,-40,-30, } ,
												{ -30,-40,-40,-50,-50,-40,-40,-30, } ,
												{ -20,-30,-30,-40,-40,-30,-30,-20, } ,
												{ -10,-20,-20,-20,-20,-20,-20,-10, } ,
												{ 20, 20,  0,  0,  0,  0, 20, 20, } ,
												{ 20, 30, 10,  0,  0, 10, 30, 20 } };

const float kingEndGameScoreTablePlayer[8][8] = {		{ -50,-40,-30,-20,-20,-30,-40,-50, },
												{ -30,-20,-10,  0,  0,-10,-20,-30, },
												{ -30,-10, 20, 30, 30, 20,-10,-30, } ,
												{ -30,-10, 30, 40, 40, 30,-10,-30, } ,
												{ -30,-10, 30, 40, 40, 30,-10,-30, } ,
												{ -30,-10, 20, 30, 30, 20,-10,-30, } ,
												{ -30,-30,  0,  0,  0,  0,-30,-30, } ,
												{ -50,-30,-30,-30,-30,-30,-30,-50 } };

// Enemy score tables
const float pawnScoreTableAI[8][8] = {			{ 0,  0,  0,  0,  0,  0,  0,  0 },
													{ 5, 10, 10,-20,-20, 10, 10,  5, },
													{ 5, -5,-10,  0,  0,-10, -5,  5, } ,
													{ 0,  0,  0, 20, 20,  0,  0,  0, } ,
													{ 5,  5, 10, 25, 25, 10,  5,  5, } ,
													{ 10, 10, 20, 30, 30, 20, 10, 10, } ,
													{ 50, 50, 50, 50, 50, 50, 50, 50, } ,
													{ 0,  0,  0,  0,  0,  0,  0,  0, } };

const float knightScoreTableAI[8][8] = {			{ -50,-40,-30,-30,-30,-30,-40,-50, },
													{ -40,-20,  0,  5,  5,  0,-20,-40, },
													{ -30,  5, 10, 15, 15, 10,  5,-30, } ,
													{ -30,  0, 15, 20, 20, 15,  0,-30, } ,
													{ -30,  5, 15, 20, 20, 15,  5,-30, } ,
													{ -30,  0, 10, 15, 15, 10,  0,-30, } ,
													{ -40,-20,  0,  0,  0,  0,-20,-40, } ,
													{ -50,-40,-30,-30,-30,-30,-40,-50, } };

const float bishopScoreTableAI[8][8] = {			{ -20,-10,-10,-10,-10,-10,-10,-20, },
													{ -10,  5,  0,  0,  0,  0,  5,-10, },
													{ -10, 10, 10, 10, 10, 10, 10,-10, } ,
													{ -10,  0, 10, 10, 10, 10,  0,-10, } ,
													{ -10,  5,  5, 10, 10,  5,  5,-10, } ,
													{ -10,  0,  5, 10, 10,  5,  0,-10, } ,
													{ -10,  0,  0,  0,  0,  0,  0,-10, } ,
													{ -20,-10,-10,-10,-10,-10,-10,-20, } };

const float rookScoreTableAI[8][8] = {			{ 0,  0,  0,  5,  5,  0,  0,  0 },
													{ -5,  0,  0,  0,  0,  0,  0, -5, },
													{ -5,  0,  0,  0,  0,  0,  0, -5, } ,
													{ -5,  0,  0,  0,  0,  0,  0, -5, } ,
													{ -5,  0,  0,  0,  0,  0,  0, -5, } ,
													{ -5,  0,  0,  0,  0,  0,  0, -5, } ,
													{ 5, 10, 10, 10, 10, 10, 10,  5, } ,
													{ 0,  0,  0,  0,  0,  0,  0,  0, } };

const float queenScoreTableAI[8][8] = {			{ -20,-10,-10, -5, -5,-10,-10,-20 },
													{ -10,  0,  5,  0,  0,  0,  0,-10, },
													{ -10,  5,  5,  5,  5,  5,  0,-10, } ,
													{ 0,  0,  5,  5,  5,  5,  0, -5, } ,
													{ -5,  0,  5,  5,  5,  5,  0, -5, } ,
													{ -10,  0,  5,  5,  5,  5,  0,-10, } ,
													{ -10,  0,  0,  0,  0,  0,  0,-10, } ,
													{ -20,-10,-10, -5, -5,-10,-10,-20, } };

const float kingMiddleGameScoreTableAI[8][8] = { { 20, 30, 10,  0,  0, 10, 30, 20 },
													{ 20, 20,  0,  0,  0,  0, 20, 20, },
													{ -10,-20,-20,-20,-20,-20,-20,-10, } ,
													{ -20,-30,-30,-40,-40,-30,-30,-20, } ,
													{ -30,-40,-40,-50,-50,-40,-40,-30, } ,
													{ -30,-40,-40,-50,-50,-40,-40,-30, } ,
													{ -30,-40,-40,-50,-50,-40,-40,-30, } ,
													{ -30,-40,-40,-50,-50,-40,-40,-30, } };

const float kingEndGameScoreTableAI[8][8] = {	{ -50,-30,-30,-30,-30,-30,-30,-50 },
													{ -30,-30,  0,  0,  0,  0,-30,-30, },
													{ -30,-10, 20, 30, 30, 20,-10,-30, } ,
													{ -30,-10, 30, 40, 40, 30,-10,-30, } ,
													{ -30,-10, 30, 40, 40, 30,-10,-30, } ,
													{ -30,-10, 20, 30, 30, 20,-10,-30, } ,
													{ -30,-20,-10,  0,  0,-10,-20,-30, } ,
													{ -50,-40,-30,-20,-20,-30,-40,-50, } };

#endif //CHESSCONSTANTS_H