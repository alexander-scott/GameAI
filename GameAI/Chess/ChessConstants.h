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

#endif //CHESSCONSTANTS_H