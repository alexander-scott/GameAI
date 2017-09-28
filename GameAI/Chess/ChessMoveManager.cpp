//------------------------------------------------------------------------
//  Author: Paul Roberts 2016
//------------------------------------------------------------------------

#include "ChessMoveManager.h"
#include "ChessCommons.h"
#include "../Commons.h"
#include <iostream>
#include <string>
using namespace std;

//Initialise the instance to null.
MoveManager* MoveManager::mInstance = NULL;

//--------------------------------------------------------------------------------------------------

MoveManager::MoveManager()
{
}

//--------------------------------------------------------------------------------------------------

MoveManager::~MoveManager()
{
	mInstance = NULL;
}

//--------------------------------------------------------------------------------------------------

MoveManager* MoveManager::Instance()
{
	if(!mInstance)
	{
		mInstance = new MoveManager;
	}

	return mInstance;
}

//--------------------------------------------------------------------------------------------------

void MoveManager::ClearRecordedMoves()
{
	mRecordedChessMoves.clear();
}

//--------------------------------------------------------------------------------------------------

void MoveManager::StoreMove(Vector2D fromBoardPosition, Vector2D toBoardPosition)
{
	StoreMove(Move(fromBoardPosition, toBoardPosition));
}

//--------------------------------------------------------------------------------------------------

void MoveManager::StoreMove(Move move)
{
	//Convert the positions passed in as moves and store in mRecordedChessMoves.
	ChessMove newMove;
	string fromMove = ConvertBoardPositionIntToLetter(move.from_X);
	fromMove += to_string(move.from_Y+1);

	string toMove = ConvertBoardPositionIntToLetter(move.to_X);
	toMove += to_string(move.to_Y+1);

	newMove.startPosition = fromMove;
	newMove.endPosition   = toMove;

	//We still want the original move stored.
	newMove.theMove = move;

	//Now do the recording.
	mRecordedChessMoves.push_back(newMove);

	OutputMove(newMove);
}

//--------------------------------------------------------------------------------------------------

bool MoveManager::HasRecordedMoves()
{
	return mRecordedChessMoves.size() > 0;
}

//--------------------------------------------------------------------------------------------------

Move MoveManager::GetLastMove()
{
	ChessMove chessMove = mRecordedChessMoves[mRecordedChessMoves.size()-1];
	return chessMove.theMove;
}

//--------------------------------------------------------------------------------------------------

string MoveManager::ConvertBoardPositionIntToLetter(int numericalValue)
{
	switch(numericalValue)
	{
		case 0:	return "A";	break;
		case 1:	return "B";	break;
		case 2:	return "C";	break;
		case 3:	return "D";	break;
		case 4:	return "E";	break;
		case 5:	return "F";	break;
		case 6:	return "G";	break;
		case 7:	return "H";	break;
		default: return "X"; break;
	}
}

//--------------------------------------------------------------------------------------------------

void MoveManager::OutputMove(ChessMove move)
{
	cout << endl << move.startPosition << " -> " << move.endPosition;
}

//--------------------------------------------------------------------------------------------------

void MoveManager::OutputGameState(GAMESTATE gameState, COLOUR playerTurn)
{
	switch(gameState)
	{
		case GAMESTATE_NORMAL:
		break;

		case GAMESTATE_CHECK:
			if(playerTurn == COLOUR_WHITE)
				cout << " (WHITE in Check)";
			else if(playerTurn == COLOUR_BLACK)
				cout << " (BLACK in Check)";
		break;

		case GAMESTATE_CHECKMATE:
			if(playerTurn == COLOUR_WHITE)
			{
				cout << " (WHITE in Checkmate)" << endl << "----------" << endl;
				cout << "BLACK Wins" << endl << "----------" << endl;	
			}
			else if(playerTurn == COLOUR_BLACK)
			{
				cout << " (BLACK in Checkmate)" << endl << "----------" << endl;
				cout << "WHITE Wins" << endl << "----------" << endl;
			}
		break;

		case GAMESTATE_STALEMATE:
			cout << "(Stalemate)" << endl << "----------" << endl;
		break;
	}
}

//--------------------------------------------------------------------------------------------------
