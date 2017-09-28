//------------------------------------------------------------------------
//  Author: Paul Roberts 2016
//------------------------------------------------------------------------

#include "ChessPlayer.h"
#include <iostream>
#include <SDL.h>
#include "ChessConstants.h"
#include "../Texture2D.h"
#include "ChessMoveManager.h"

using namespace::std;

//--------------------------------------------------------------------------------------------------

ChessPlayer::ChessPlayer(SDL_Renderer* renderer, COLOUR colour, Board* board, vector<Vector2D>* highlights, Vector2D* selectedPiecePosition, Move* lastMove)
{
	//Spritesheet for drawing the pawn promotion popup.
	mSelectAPieceTexture		= new Texture2D(renderer);
	mSelectAPieceTexture->LoadFromFile(kChessSelectAPiecePath);
	mPawnPromotionDrawPosition.x = (int)(kChessScreenWidth*0.24f);
	mPawnPromotionDrawPosition.y = (int)(kChessScreenHeight*0.25f);
	mPawnPromotionDrawPosition.w = (int)mSelectAPieceTexture->GetWidth();
	mPawnPromotionDrawPosition.h = (int)(mSelectAPieceTexture->GetHeight()*0.5f);

	//Start values.
	mChessBoard				= board;
	mTeamColour				= colour;
	mHighlightPositions		= highlights;
	mCurrentMove			= SELECT_A_PIECE; 
	mSelectedPiecePosition	= selectedPiecePosition;
	mLastMove				= lastMove;
	mInCheck				= false;

	//Set the number of pieces we start with.
	//This will be used to reduce checking entire board for pieces.
	mNumberOfLivingPieces	= kTotalNumberOfStartingPieces;
}

//--------------------------------------------------------------------------------------------------

ChessPlayer::~ChessPlayer()
{
	mChessBoard = NULL;

	mHighlightPositions = NULL;

	delete mSelectAPieceTexture;
	mSelectAPieceTexture = NULL;

	delete mSelectedPiecePosition;
	mSelectedPiecePosition = NULL;
}

//--------------------------------------------------------------------------------------------------

GAMESTATE ChessPlayer::PreTurn()
{
	//Check whether this player is in CHECK, CHECKMATE or STALEMATE.
	mInCheck = CheckForCheck( *mChessBoard, mTeamColour );

	//Remove any highlight position as we have yet to select a piece.
	mSelectedPiecePosition->x = -1;
	mSelectedPiecePosition->y = -1;

	//If we are in CHECK, can we actually make a move to get us out of it?
	if( mInCheck )
	{
		if(CheckForCheckmate( *mChessBoard, mTeamColour ))
			return GAMESTATE_CHECKMATE;
		else 
			return GAMESTATE_CHECK;
	}
	else
	{
		if(CheckForStalemate( *mChessBoard, mTeamColour ))
			return GAMESTATE_STALEMATE;
		else
			return GAMESTATE_NORMAL;
	}

	//Return normal if none of the above conditions have been met.
	return GAMESTATE_NORMAL;
}

//--------------------------------------------------------------------------------------------------

bool ChessPlayer::MakeAMove(Vector2D boardPosition)
{
	//Ensure the position passed in is within the board dimensions.
	if( boardPosition.x<= kBoardDimensions && boardPosition.y <= kBoardDimensions )
	{
		if( mCurrentMove == SELECT_A_PIECE )
		{
			//If the piece selected is of the same colour as the current player - Deal with it.
			if( mChessBoard->currentLayout[(int)boardPosition.x][(int)boardPosition.y].colour == mTeamColour )
			{
				//Valid position so store it and get possible moves.
				*mSelectedPiecePosition = boardPosition;
				BoardPiece boardPiece = mChessBoard->currentLayout[(int)boardPosition.x][(int)boardPosition.y];

				//Clear all highlighted moves before repopulating in GetMoveOptions() function.
				if(boardPiece.colour == mTeamColour)
					mHighlightPositions->clear();

				
				vector<Move> moves;
				GetMoveOptions(*mSelectedPiecePosition, boardPiece, *mChessBoard, &moves);

				//Is this a valid move?
				for(unsigned int i = 0; i < moves.size(); i++)
				{
					Board boardToTest = *mChessBoard;
					boardToTest.currentLayout[moves[i].to_X][moves[i].to_Y] = boardToTest.currentLayout[moves[i].from_X][moves[i].from_Y];
					boardToTest.currentLayout[moves[i].from_X][moves[i].from_Y] = BoardPiece();

					if(!CheckForCheck(boardToTest, mTeamColour))
						mHighlightPositions->push_back(Vector2D(moves[i].to_X, moves[i].to_Y)); 
				}

				//Change move type.
				mCurrentMove = SELECT_A_POSITION;
			}
			else
			{
				//TODO: Play a negative sound.
			}
		}
		else if( mCurrentMove == SELECT_A_POSITION )
		{
			//Check if this position is actually another piece of the current players colour.
			//If so, switch the turn type and recall this function.
			if( mChessBoard->currentLayout[(int)boardPosition.x][(int)boardPosition.y].colour == mTeamColour )
			{
				mCurrentMove = SELECT_A_PIECE;
				MakeAMove(boardPosition);
			}
			else
			{
				bool validPosition = false;

				//Check if this is a position within the highlighted options.
				for( unsigned int i = 0; i < mHighlightPositions->size(); i++ )
				{
					Vector2D highlightPos = mHighlightPositions->at(i);
					if( (int)(boardPosition.x) == (int)highlightPos.x && (int)(boardPosition.y) == (int)highlightPos.y )
					{
						//But does this move leave us in check?
						/*Board tempBoard = *mChessBoard; 
						tempBoard.currentLayout[(int)boardPosition.x][(int)boardPosition.y] = tempBoard.currentLayout[(int)mSelectedPiecePosition->x][(int)mSelectedPiecePosition->y];
						tempBoard.currentLayout[(int)mSelectedPiecePosition->x][(int)mSelectedPiecePosition->y] = BoardPiece();
						if( !CheckForCheck(tempBoard, mTeamColour) )*/
						{	
							validPosition = true;
							break;
						}
					}
				}

				//If we selected a valid position from the highlighted options, then move the piece.
				if( validPosition == true )
				{
					//If this was an en'passant move the taken piece will not be in the square we moved to.
					if( mChessBoard->currentLayout[(int)mSelectedPiecePosition->x][(int)mSelectedPiecePosition->y].piece == PIECE_PAWN )
					{
						//If the pawn is on its start position and it double jumps, then en'passant may be available for opponent.
						if( (mSelectedPiecePosition->y == 1 && boardPosition.y == 3) || 
							(mSelectedPiecePosition->y == 6 && boardPosition.y == 4) )
						{
							mChessBoard->currentLayout[(int)mSelectedPiecePosition->x][(int)mSelectedPiecePosition->y].canEnPassant = true;
						}
					}

					//En'Passant removal of enemy pawn.
					//If our pawn moved into an empty position to the left or right, then must be En'Passant.
					if( mChessBoard->currentLayout[(int)mSelectedPiecePosition->x][(int)mSelectedPiecePosition->y].piece == PIECE_PAWN &&
						mChessBoard->currentLayout[(int)boardPosition.x][(int)boardPosition.y].piece == PIECE_NONE )
					{
						int pawnDirectionOpposite = mTeamColour == COLOUR_WHITE ? -1 : 1;

						if( ((int)mSelectedPiecePosition->x < (int)boardPosition.x) ||
							((int)mSelectedPiecePosition->x > (int)boardPosition.x) )
						{
							mChessBoard->currentLayout[(int)boardPosition.x][(int)boardPosition.y+pawnDirectionOpposite] = BoardPiece();
						}
					}

					//CASTLING - Move the rook.
					if( mChessBoard->currentLayout[(int)mSelectedPiecePosition->x][(int)mSelectedPiecePosition->y].piece == PIECE_KING )
					{
						//Are we moving 2 spaces??? This indicates CASTLING.
						if( (int)boardPosition.x-(int)mSelectedPiecePosition->x == 2 )
						{
							//Moving 2 spaces to the right - Move the ROOK on the right into its new position.
							mChessBoard->currentLayout[(int)mSelectedPiecePosition->x+3][(int)mSelectedPiecePosition->y].hasMoved = true;
							mChessBoard->currentLayout[(int)mSelectedPiecePosition->x+1][(int)mSelectedPiecePosition->y] = mChessBoard->currentLayout[(int)mSelectedPiecePosition->x+3][(int)mSelectedPiecePosition->y];
							mChessBoard->currentLayout[(int)mSelectedPiecePosition->x+3][(int)mSelectedPiecePosition->y] = BoardPiece();
						}
						else if( (int)boardPosition.x-(int)mSelectedPiecePosition->x == -2 )
						{
							//Moving 2 spaces to the left - Move the ROOK on the left into its new position.
							//Move the piece into new position.
							mChessBoard->currentLayout[(int)mSelectedPiecePosition->x-4][(int)mSelectedPiecePosition->y].hasMoved = true;
							mChessBoard->currentLayout[(int)mSelectedPiecePosition->x-1][(int)mSelectedPiecePosition->y] = mChessBoard->currentLayout[(int)mSelectedPiecePosition->x-4][(int)mSelectedPiecePosition->y];
							mChessBoard->currentLayout[(int)mSelectedPiecePosition->x-4][(int)mSelectedPiecePosition->y] = BoardPiece();
						}
					}

					//Move the piece into new position.
					mChessBoard->currentLayout[(int)mSelectedPiecePosition->x][(int)mSelectedPiecePosition->y].hasMoved = true;
					mChessBoard->currentLayout[(int)boardPosition.x][(int)boardPosition.y] = mChessBoard->currentLayout[(int)mSelectedPiecePosition->x][(int)mSelectedPiecePosition->y];
					mChessBoard->currentLayout[(int)mSelectedPiecePosition->x][(int)mSelectedPiecePosition->y] = BoardPiece();
					
					//Store the last move to output at start of turn.
					mLastMove->from_X = (int)mSelectedPiecePosition->x;
					mLastMove->from_Y = (int)mSelectedPiecePosition->y;
					mLastMove->to_X = (int)boardPosition.x;
					mLastMove->to_Y = (int)boardPosition.y;

					//Record the move.
					MoveManager::Instance()->StoreMove(*mSelectedPiecePosition, boardPosition);

					//Piece is in a new position.
					mSelectedPiecePosition->x = boardPosition.x;
					mSelectedPiecePosition->y = boardPosition.y;

					//Check if we need to promote a pawn.
					if( mChessBoard->currentLayout[(int)mSelectedPiecePosition->x][(int)mSelectedPiecePosition->y].piece == PIECE_PAWN &&
						((int)mSelectedPiecePosition->y == 0 || (int)mSelectedPiecePosition->y == 7) )
					{
						//Time to promote.
						mCurrentMove = PAWN_PROMOTION;
					}
					else
					{
						//Turn finished.
						return true;
					}
				}
			}
		}
		else if( mCurrentMove == PAWN_PROMOTION )
		{
			//Convert board position from board coordinates back into screen coordinates.
			boardPosition *= kChessPieceDimensions;

			if( boardPosition.x > mPawnPromotionDrawPosition.x &&
				boardPosition.x < mPawnPromotionDrawPosition.x + mPawnPromotionDrawPosition.w &&
				boardPosition.y > mPawnPromotionDrawPosition.y &&
				boardPosition.y < mPawnPromotionDrawPosition.y + mPawnPromotionDrawPosition.h )
			{
				double relativeXPosition = boardPosition.x - mPawnPromotionDrawPosition.x;
				
				//Get the new PIECE type, by getting a 0-1 value using its relative x position, and then multiplying by 4 pieces.
				//Add 1 to move the scale from KNIGHT to QUEEN.
				PIECE newPieceType  = (PIECE)((int)((relativeXPosition/mPawnPromotionDrawPosition.w)*4)+1); 

				//Change the PAWN into the selected piece.
				mChessBoard->currentLayout[(int)mSelectedPiecePosition->x][(int)mSelectedPiecePosition->y].piece = newPieceType;

				//Turn finished.
				return true;
			}
		}
	}

	//Not finished turn yet.
	return false;
}

//--------------------------------------------------------------------------------------------------

void ChessPlayer::GetNumberOfLivingPieces()
{
	mNumberOfLivingPieces = 0;

	for(int x = 0; x < kBoardDimensions; x++)
	{
		for(int y = 0; y < kBoardDimensions; y++)
		{
			//Check for pieces.
			BoardPiece currentPiece = mChessBoard->currentLayout[x][y];
			if(currentPiece.colour == mTeamColour && currentPiece.piece != PIECE_NONE)
				mNumberOfLivingPieces++;
		}
	}
}

//--------------------------------------------------------------------------------------------------

void ChessPlayer::GetAllMoveOptions(Board boardToTest, COLOUR teamColour, vector<Move>* moves)
{
	int numberOfPiecesFound = 0;

	//Remove any previously stored move options.
	moves->clear();

	//Go through the board and get the moves for all pieces of our colour.
	for(int x = 0; x < kBoardDimensions; x++)
	{
		for(int y = 0; y < kBoardDimensions; y++)
		{
			//Check for pieces.
			BoardPiece currentPiece = boardToTest.currentLayout[x][y];
			if(currentPiece.colour == teamColour && currentPiece.piece != PIECE_NONE)
			{
				numberOfPiecesFound++;

				//All pieces move differently.
				Vector2D piecePosition = Vector2D(x,y);

				switch(currentPiece.piece)
				{
					case PIECE_PAWN:
						GetPawnMoveOptions(piecePosition, currentPiece, boardToTest, moves);
					break;
	
					case PIECE_KNIGHT:
						GetKnightMoveOptions(piecePosition, currentPiece, boardToTest, moves);
					break;

					case PIECE_BISHOP:
						GetDiagonalMoveOptions(piecePosition, currentPiece, boardToTest, moves);
					break;

					case PIECE_ROOK:
						GetHorizontalAndVerticalMoveOptions(piecePosition, currentPiece, boardToTest, moves);
					break;

					case PIECE_QUEEN:
						GetHorizontalAndVerticalMoveOptions(piecePosition, currentPiece, boardToTest, moves);
						GetDiagonalMoveOptions(piecePosition, currentPiece, boardToTest, moves);
					break;

					case PIECE_KING:
						GetKingMoveOptions(piecePosition, currentPiece, boardToTest, moves);
					break;

					case PIECE_NONE:
					break;

					default:
					break;
				}

				//Early exit - No point searching when we have already found all our pieces.
				if(numberOfPiecesFound == mNumberOfLivingPieces)
					return;
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------

void ChessPlayer::GetMoveOptions(Vector2D piecePosition, BoardPiece boardPiece, Board boardToTest, vector<Move>* moves)
{
	//All pieces move differently.
	switch(boardPiece.piece)
	{
		case PIECE_PAWN:
			GetPawnMoveOptions(piecePosition, boardPiece, boardToTest, moves);
		break;
	
		case PIECE_KNIGHT:
			GetKnightMoveOptions(piecePosition, boardPiece, boardToTest, moves);
		break;

		case PIECE_BISHOP:
			GetDiagonalMoveOptions(piecePosition, boardPiece, boardToTest, moves);
		break;

		case PIECE_ROOK:
			GetHorizontalAndVerticalMoveOptions(piecePosition, boardPiece, boardToTest, moves);
		break;

		case PIECE_QUEEN:
			GetHorizontalAndVerticalMoveOptions(piecePosition, boardPiece, boardToTest, moves);
			GetDiagonalMoveOptions(piecePosition, boardPiece, boardToTest, moves);
		break;

		case PIECE_KING:
			GetKingMoveOptions(piecePosition, boardPiece, boardToTest, moves);
		break;

		case PIECE_NONE:
		break;

		default:
		break;
	}
}

//--------------------------------------------------------------------------------------------------

bool ChessPlayer::CheckMoveOptionValidityAndStoreMove(Move moveToCheck, COLOUR pieceColour, Board boardToTest, vector<Move>* moves)
{
	if(moveToCheck.to_X >= 0 && moveToCheck.to_X < kBoardDimensions && moveToCheck.to_Y >= 0 && moveToCheck.to_Y < kBoardDimensions)
	{
		//We check with colour passed in to enable the same functions to construct attacked spaces
		//as well as constructing the positions we can move to.
		if(boardToTest.currentLayout[moveToCheck.to_X][moveToCheck.to_Y].piece == PIECE_NONE)
		{
			//Will this leave us in check?
			boardToTest.currentLayout[moveToCheck.to_X][moveToCheck.to_Y] = boardToTest.currentLayout[moveToCheck.from_X][moveToCheck.from_Y];
			boardToTest.currentLayout[moveToCheck.from_X][moveToCheck.from_Y] = BoardPiece();
			//if(pieceColour == mTeamColour)
			//{
				if(!CheckForCheck(boardToTest, pieceColour))
					moves->push_back(moveToCheck);
		//	}
			//else
				//moves->push_back(moveToCheck);
		}
		else
		{
			//A piece so no more moves after this, but can we take it?
			if(boardToTest.currentLayout[moveToCheck.to_X][moveToCheck.to_Y].colour != pieceColour)
			{
				//Will this leave us in check?
				boardToTest.currentLayout[moveToCheck.to_X][moveToCheck.to_Y] = boardToTest.currentLayout[moveToCheck.from_X][moveToCheck.from_Y];
				boardToTest.currentLayout[moveToCheck.from_X][moveToCheck.from_Y] = BoardPiece();
				//if(pieceColour == mTeamColour)
				//{
					if(!CheckForCheck(boardToTest, pieceColour))
						moves->push_back(moveToCheck);
				//}
				//else
					//moves->push_back(moveToCheck);
			}

			//Hit a piece, so no more moves in this direction.
			return false;
		}

		return true;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------

void ChessPlayer::ClearEnPassant()
{
	for(int x = 0; x < kBoardDimensions; x++)
	{
		for(int y = 0; y < kBoardDimensions; y++)
		{
			//Clear opponents en'Passant, not ours. Ours needs to be available for the opponents turn.
			if(mChessBoard->currentLayout[x][y].piece == PIECE_PAWN && mChessBoard->currentLayout[x][y].colour != mTeamColour)
			{
				mChessBoard->currentLayout[x][y].canEnPassant = false;
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------

void ChessPlayer::GetPawnMoveOptions(Vector2D piecePosition, BoardPiece boardPiece, Board boardToTest, vector<Move>* moves)
{
	Board tempBoard = boardToTest;
	int pawnDirection = boardPiece.colour == COLOUR_WHITE ? -1 : 1;

	//Single step FORWARD.
	
	int xPos = (int)piecePosition.x;
	int yPos = (int)piecePosition.y+pawnDirection;
	if(yPos >= 0 && yPos < kBoardDimensions && tempBoard.currentLayout[xPos][yPos].piece == PIECE_NONE)
	{
		//Will this leave us in check? Only interested in check if its one of our moves.
		tempBoard.currentLayout[xPos][yPos] = tempBoard.currentLayout[(int)piecePosition.x][(int)piecePosition.y];
		tempBoard.currentLayout[(int)piecePosition.x][(int)piecePosition.y] = BoardPiece();
		//if(boardPiece.colour == mTeamColour)
		//{
			if(!CheckForCheck(tempBoard, boardPiece.colour))
				moves->push_back(Move(piecePosition, Vector2D(piecePosition.x, piecePosition.y+pawnDirection)));
		//}
		//else
			//moves->push_back(Move(piecePosition, Vector2D(piecePosition.x, piecePosition.y+pawnDirection)));
	}

	//Double step FORWARD.
	if(piecePosition.y == 1 || piecePosition.y == 6)
	{
		//Rest the board.
		tempBoard = boardToTest;			
			
		int yPos2 = (int)piecePosition.y+pawnDirection*2;
		if(tempBoard.currentLayout[xPos][yPos].piece == PIECE_NONE && tempBoard.currentLayout[xPos][yPos2].piece == PIECE_NONE)
		{
			//Will this leave us in check? Only interested in check if its one of our moves.
			tempBoard.currentLayout[xPos][yPos2] = tempBoard.currentLayout[(int)piecePosition.x][(int)piecePosition.y];
			tempBoard.currentLayout[(int)piecePosition.x][(int)piecePosition.y] = BoardPiece();
			//if(boardPiece.colour == mTeamColour)
			//{
				if(!CheckForCheck(tempBoard, boardPiece.colour))
				{
					moves->push_back(Move(piecePosition, Vector2D(piecePosition.x, yPos2)));


				}
			//}
			//else
				//moves->push_back(Move(piecePosition, Vector2D(piecePosition.x, yPos2)));
		}
	}

	//En'Passant move.
	if( (piecePosition.y == 3 && boardPiece.colour == COLOUR_WHITE) || 
		(piecePosition.y == 4 && boardPiece.colour == COLOUR_BLACK) )
	{
		//Test the board.
		tempBoard = boardToTest;			
			
		//Pawn beside us, can we en'passant.
		xPos = (int)piecePosition.x-1;
		yPos = (int)piecePosition.y;
		BoardPiece leftPiece = tempBoard.currentLayout[xPos][yPos];
		if(leftPiece.piece == PIECE_PAWN && leftPiece.canEnPassant == true)
		{
			//Will this leave us in check? Only interested in check if its one of our moves.
			tempBoard.currentLayout[xPos][yPos] = tempBoard.currentLayout[(int)piecePosition.x][(int)piecePosition.y];
			tempBoard.currentLayout[(int)piecePosition.x][(int)piecePosition.y] = BoardPiece();
			//if(boardPiece.colour == mTeamColour)
			//{
				if(!CheckForCheck(tempBoard, boardPiece.colour))
					moves->push_back(Move(piecePosition, Vector2D(piecePosition.x-1, piecePosition.y+pawnDirection)));
			//}
			//else
				//moves->push_back(Move(piecePosition, Vector2D(piecePosition.x-1, piecePosition.y+pawnDirection)));
		}

		//Rest the board.
		tempBoard = boardToTest;			
			
		xPos = (int)piecePosition.x+1;
		BoardPiece rightPiece = tempBoard.currentLayout[xPos][yPos];
		if(rightPiece.piece == PIECE_PAWN && rightPiece.canEnPassant == true)
		{
			//Will this leave us in check? Only interested in check if its one of our moves.
			tempBoard.currentLayout[xPos][yPos] = tempBoard.currentLayout[(int)piecePosition.x][(int)piecePosition.y];
			tempBoard.currentLayout[(int)piecePosition.x][(int)piecePosition.y] = BoardPiece();
			//if(boardPiece.colour == mTeamColour)
			//{
				if(!CheckForCheck(tempBoard, boardPiece.colour))
					moves->push_back(Move(piecePosition, Vector2D(piecePosition.x+1, piecePosition.y+pawnDirection)));
			//}
			//else
				//moves->push_back(Move(piecePosition, Vector2D(piecePosition.x+1, piecePosition.y+pawnDirection)));
		}
	}

	//Take a piece move.
	if(piecePosition.y > 0 && piecePosition.y < kBoardDimensions-1)
	{
		//Ahead of selected pawn to the LEFT.
		if(piecePosition.x > 0)
		{
			//Rest the board.
			tempBoard = boardToTest;			
			
			int xPos = (int)piecePosition.x-1;
			int yPos = (int)piecePosition.y+pawnDirection;
			BoardPiece aheadLeftPiece = tempBoard.currentLayout[xPos][yPos];
			if(aheadLeftPiece.piece != PIECE_NONE && aheadLeftPiece.colour != boardPiece.colour)
			{
				//Will this leave us in check? Only interested in check if its one of our moves.
				tempBoard.currentLayout[xPos][yPos] = tempBoard.currentLayout[(int)piecePosition.x][(int)piecePosition.y];
				tempBoard.currentLayout[(int)piecePosition.x][(int)piecePosition.y] = BoardPiece();
				//if(boardPiece.colour == mTeamColour)
				//{
					if(!CheckForCheck(tempBoard, boardPiece.colour))
						moves->push_back(Move(piecePosition, Vector2D(xPos, piecePosition.y+pawnDirection)));
				//}
				//else
					//moves->push_back(Move(piecePosition, Vector2D(xPos, piecePosition.y+pawnDirection)));
			}
		}

		//Ahead of selected pawn to the RIGHT.
		if((int)piecePosition.x < kBoardDimensions-1) 
		{
			//Rest the board.
			tempBoard = boardToTest;			
			
			int xPos = (int)piecePosition.x+1;
			int yPos = (int)piecePosition.y+pawnDirection;
			BoardPiece aheadRightPiece = tempBoard.currentLayout[xPos][yPos];
			if(aheadRightPiece.piece != PIECE_NONE && aheadRightPiece.colour != boardPiece.colour)
			{
				//Will this leave us in check? Only interested in check if its one of our moves.
				tempBoard.currentLayout[xPos][yPos] = tempBoard.currentLayout[(int)piecePosition.x][(int)piecePosition.y];
				tempBoard.currentLayout[(int)piecePosition.x][(int)piecePosition.y] = BoardPiece();
				//if(boardPiece.colour == mTeamColour)
				//{
					if(!CheckForCheck(tempBoard, boardPiece.colour))
						moves->push_back(Move(piecePosition, Vector2D(xPos, piecePosition.y+pawnDirection)));
				//}
				//else
					//moves->push_back(Move(piecePosition, Vector2D(xPos, piecePosition.y+pawnDirection)));
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------

void ChessPlayer::GetHorizontalAndVerticalMoveOptions(Vector2D piecePosition, BoardPiece boardPiece, Board boardToTest, vector<Move>* moves)
{
	Move move;

	//Vertical DOWN the board.
	for(int yPos = (int)piecePosition.y+1; yPos < kBoardDimensions; yPos++)
	{
		//Keep checking moves until one is invalid.
		move = Move(piecePosition, Vector2D(piecePosition.x, yPos));
		if(CheckMoveOptionValidityAndStoreMove(move, boardPiece.colour, boardToTest, moves) == false)
			break;
	}

	//Vertical UP the board.
	for(int yPos = (int)piecePosition.y-1; yPos >= 0; yPos--)
	{
		//Keep checking moves until one is invalid.
		move = Move(piecePosition, Vector2D(piecePosition.x, yPos));
		if(CheckMoveOptionValidityAndStoreMove(move, boardPiece.colour, boardToTest, moves) == false)
			break;
	}

	//Horizontal LEFT of the board.
	for(int xPos = (int)piecePosition.x-1; xPos >= 0; xPos--)
	{
		//Keep checking moves until one is invalid.
		move = Move(piecePosition, Vector2D(xPos, piecePosition.y));
		if(CheckMoveOptionValidityAndStoreMove(move, boardPiece.colour, boardToTest, moves) == false)
			break;
	}

	//Horizontal RIGHT of the board.
	for(int xPos = (int)piecePosition.x+1; xPos < kBoardDimensions; xPos++)
	{
		//Keep checking moves until one is invalid.
		move = Move(piecePosition, Vector2D(xPos, piecePosition.y));
		if(CheckMoveOptionValidityAndStoreMove(move, boardPiece.colour, boardToTest, moves) == false)
			break;
	}
}

//--------------------------------------------------------------------------------------------------

void ChessPlayer::GetDiagonalMoveOptions(Vector2D piecePosition, BoardPiece boardPiece, Board boardToTest, vector<Move>* moves)
{
	Move move;

	//ABOVE & LEFT
	for(int yPos = (int)piecePosition.y-1, xPos = (int)piecePosition.x-1; yPos >= 0 && xPos >= 0; yPos--, xPos--)
	{
		//Keep checking moves until one is invalid.
		move = Move(piecePosition, Vector2D(xPos, yPos));
		if(CheckMoveOptionValidityAndStoreMove(move, boardPiece.colour, boardToTest, moves) == false)
			break;
	}

	//ABOVE & RIGHT
	for(int yPos = (int)piecePosition.y-1, xPos = (int)piecePosition.x+1; yPos >= 0 && xPos < kBoardDimensions; yPos--, xPos++)
	{
		//Keep checking moves until one is invalid.
		move = Move(piecePosition, Vector2D(xPos, yPos));
		if(CheckMoveOptionValidityAndStoreMove(move, boardPiece.colour, boardToTest, moves) == false)
			break;
	}

	//BELOW & LEFT
	for(int yPos = (int)piecePosition.y+1, xPos = (int)piecePosition.x-1; yPos < kBoardDimensions && xPos >= 0; yPos++, xPos--)
	{
		//Keep checking moves until one is invalid.
		move = Move(piecePosition, Vector2D(xPos, yPos));
		if(CheckMoveOptionValidityAndStoreMove(move, boardPiece.colour, boardToTest, moves) == false)
			break;
	}

	//BELOW & RIGHT
	for(int yPos = (int)piecePosition.y+1, xPos = (int)piecePosition.x+1; yPos < kBoardDimensions && xPos < kBoardDimensions; yPos++, xPos++)
	{
		//Keep checking moves until one is invalid.
		move = Move(piecePosition, Vector2D(xPos, yPos));
		if(CheckMoveOptionValidityAndStoreMove(move, boardPiece.colour, boardToTest, moves) == false)
			break;
	}
}

//--------------------------------------------------------------------------------------------------

void ChessPlayer::GetKnightMoveOptions(Vector2D piecePosition, BoardPiece boardPiece, Board boardToTest, vector<Move>* moves)
{
	Move move;

	//Moves to the RIGHT.
	move = Move(piecePosition, Vector2D(piecePosition.x+2, piecePosition.y+1));
	CheckMoveOptionValidityAndStoreMove(move, boardPiece.colour, boardToTest, moves);

	move.to_Y = (int)piecePosition.y-1;
	CheckMoveOptionValidityAndStoreMove(move, boardPiece.colour, boardToTest, moves);

	//Moves to the LEFT.
	move.to_X = (int)piecePosition.x-2;
	move.to_Y = (int)piecePosition.y+1;
	CheckMoveOptionValidityAndStoreMove(move, boardPiece.colour, boardToTest, moves);

	move.to_Y = (int)piecePosition.y-1;
	CheckMoveOptionValidityAndStoreMove(move, boardPiece.colour, boardToTest, moves);

	//Moves ABOVE.
	move.to_X = (int)piecePosition.x+1;
	move.to_Y = (int)piecePosition.y-2;
	CheckMoveOptionValidityAndStoreMove(move, boardPiece.colour, boardToTest, moves);

	move.to_X = (int)piecePosition.x-1;
	CheckMoveOptionValidityAndStoreMove(move, boardPiece.colour, boardToTest, moves);

	//Moves BELOW.
	move.to_X = (int)piecePosition.x+1;
	move.to_Y = (int)piecePosition.y+2;
	CheckMoveOptionValidityAndStoreMove(move, boardPiece.colour, boardToTest, moves);

	move.to_X = (int)piecePosition.x-1;
	CheckMoveOptionValidityAndStoreMove(move, boardPiece.colour, boardToTest, moves);
}

//--------------------------------------------------------------------------------------------------

void ChessPlayer::GetKingMoveOptions(Vector2D piecePosition, BoardPiece boardPiece, Board boardToTest, vector<Move>* moves)
{
	Move move;

	//Start at position top left of king and move across and down.
	for(int yPos = (int)piecePosition.y-1; yPos <= (int)piecePosition.y+1; yPos++)
	{
		for(int xPos = (int)piecePosition.x-1; xPos <= (int)piecePosition.x+1; xPos++)
		{
			if( (yPos >= 0 && yPos < kBoardDimensions) && (xPos >= 0 && xPos < kBoardDimensions) )
			{
				//Check if move is valid and store it. We dont care about the return value as we are only
				// checking one move in each direction.
				move = Move(piecePosition, Vector2D(xPos, yPos));
				CheckMoveOptionValidityAndStoreMove(move, boardPiece.colour, boardToTest, moves);
			}
		}
	}

	if( boardPiece.colour == mTeamColour )
	{
		COLOUR opponentColour = mTeamColour == COLOUR_WHITE ? COLOUR_BLACK : COLOUR_WHITE;

		//TODO: This breaks everything as it is recursive.
		//Compile all the moves available to our opponent.
		vector<Move> allOpponentMoves;
		GetAllMoveOptions(boardToTest, opponentColour, &allOpponentMoves);

		//Can CASTLE if not in CHECK.
		if( !mInCheck )
		{
			//CASTLE to the right.
			BoardPiece king		 = boardToTest.currentLayout[(int)piecePosition.x][(int)piecePosition.y];
			BoardPiece rightRook = boardToTest.currentLayout[(int)piecePosition.x+3][(int)piecePosition.y];

			if( !king.hasMoved && (rightRook.piece == PIECE_ROOK && !rightRook.hasMoved) )
			{
				if( boardToTest.currentLayout[(int)piecePosition.x+1][(int)piecePosition.y].piece == PIECE_NONE &&
					boardToTest.currentLayout[(int)piecePosition.x+2][(int)piecePosition.y].piece == PIECE_NONE)
				{
					//Cannot CASTLE through a CHECK position.
					bool canCastle = true;
					for(unsigned int i = 0; i < allOpponentMoves.size(); i++)
					{
						if( (allOpponentMoves[i].to_X == (int)piecePosition.x+2 && allOpponentMoves[i].to_Y == (int)piecePosition.y) ||
							(allOpponentMoves[i].to_X == (int)piecePosition.x+1 && allOpponentMoves[i].to_Y == (int)piecePosition.y) )
						{
							canCastle = false;
						}
					}

					//Check if the final position is valid.
					if( canCastle )
						CheckMoveOptionValidityAndStoreMove(Move(piecePosition, Vector2D(piecePosition.x+2, piecePosition.y)), boardPiece.colour, boardToTest, moves);
				}
			}

			//CASTLE to the left.
			BoardPiece leftRook = boardToTest.currentLayout[(int)piecePosition.x-4][(int)piecePosition.y];

			if( !king.hasMoved && (leftRook.piece == PIECE_ROOK && !leftRook.hasMoved) )
			{
				if( boardToTest.currentLayout[(int)piecePosition.x-1][(int)piecePosition.y].piece == PIECE_NONE &&
					boardToTest.currentLayout[(int)piecePosition.x-2][(int)piecePosition.y].piece == PIECE_NONE &&
					boardToTest.currentLayout[(int)piecePosition.x-3][(int)piecePosition.y].piece == PIECE_NONE )
				{
					//Cannot CASTLE through a CHECK position.
					bool canCastle = true;
					for(unsigned int i = 0; i < allOpponentMoves.size(); i++)
					{
						if( (allOpponentMoves[i].to_X == (int)piecePosition.x-1 && allOpponentMoves[i].to_Y == (int)piecePosition.y) ||
							(allOpponentMoves[i].to_X == (int)piecePosition.x-2 && allOpponentMoves[i].to_Y == (int)piecePosition.y) ||
							(allOpponentMoves[i].to_X == (int)piecePosition.x-3 && allOpponentMoves[i].to_Y == (int)piecePosition.y) )
						{
							canCastle = false;
						}
					}

					//Check if the final position is valid.
					if( canCastle )
						CheckMoveOptionValidityAndStoreMove(Move(piecePosition, Vector2D(piecePosition.x-2, piecePosition.y)), boardPiece.colour, boardToTest, moves);
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------

bool ChessPlayer::CheckForCheck(Board boardToTest, COLOUR teamColour)
{
	Vector2D ourKingPosition;
	//COLOUR opponentColour = teamColour == COLOUR_WHITE ? COLOUR_BLACK : COLOUR_WHITE;

	//Go through the board and find our KING's position.
	for(int xAxis = 0; xAxis < kBoardDimensions; xAxis++)
	{
		for(int yAxis = 0; yAxis < kBoardDimensions; yAxis++)
		{
			BoardPiece currentPiece = boardToTest.currentLayout[xAxis][yAxis];
			if(currentPiece.colour == teamColour && currentPiece.piece == PIECE_KING)
			{
				//Store our KING's position whilst we go through the board.
				ourKingPosition = Vector2D(xAxis,yAxis);
				
				//Force double loop exit.
				xAxis = kBoardDimensions;
				yAxis = kBoardDimensions;
			}
		}
	}

	//Now we have our kings position lets check if it is under attack from anywhere.
	//Horizontal - Right
	int x = (int)ourKingPosition.x;
	while(++x < kBoardDimensions)
	{
		BoardPiece currentPiece = boardToTest.currentLayout[x][(int)ourKingPosition.y];
		if(currentPiece.piece != PIECE_NONE)
		{
			if( currentPiece.colour == teamColour)
				break;
			else if(currentPiece.piece == PIECE_QUEEN || currentPiece.piece == PIECE_ROOK)
				return true;
			else
				break;
		}
	}

	//Horizontal - Left
	x = (int)ourKingPosition.x;
	while(--x >= 0)
	{
		BoardPiece currentPiece = boardToTest.currentLayout[x][(int)ourKingPosition.y];
		if(currentPiece.piece != PIECE_NONE)
		{
			if( currentPiece.colour == teamColour)
				break;
			else if(currentPiece.piece == PIECE_QUEEN || currentPiece.piece == PIECE_ROOK)
				return true;
			else
				break;
		}
	}

	//Veritcal - Up
	int y = (int)ourKingPosition.y;
	while(--y >= 0)
	{
		BoardPiece currentPiece = boardToTest.currentLayout[(int)ourKingPosition.x][y];
		if(currentPiece.piece != PIECE_NONE)
		{
			if( currentPiece.colour == teamColour)
				break;
			else if(currentPiece.piece == PIECE_QUEEN || currentPiece.piece == PIECE_ROOK)
				return true;
			else
				break;
		}
	}

	//Veritcal - Down
	y = (int)ourKingPosition.y;
	while(++y < kBoardDimensions)
	{
		BoardPiece currentPiece = boardToTest.currentLayout[(int)ourKingPosition.x][y];
		if(currentPiece.piece != PIECE_NONE)
		{
			if( currentPiece.colour == teamColour)
				break;
			else if(currentPiece.piece == PIECE_QUEEN || currentPiece.piece == PIECE_ROOK)
				return true;
			else
				break;
		}
	}

	//Diagonal - Right Down
	x = (int)ourKingPosition.x;
	y = (int)ourKingPosition.y;
	while(++y < kBoardDimensions && ++x < kBoardDimensions)
	{
		BoardPiece currentPiece = boardToTest.currentLayout[x][y];
		if(currentPiece.piece != PIECE_NONE)
		{
			if( currentPiece.colour == teamColour)
				break;
			else if(currentPiece.piece == PIECE_QUEEN || currentPiece.piece == PIECE_BISHOP)
				return true;
			else
				break;
		}
	}

	//Diagonal - Right Up
	x = (int)ourKingPosition.x;
	y = (int)ourKingPosition.y;
	while(--y >= 0 && ++x < kBoardDimensions)
	{
		BoardPiece currentPiece = boardToTest.currentLayout[x][y];
		if(currentPiece.piece != PIECE_NONE)
		{
			if( currentPiece.colour == teamColour)
				break;
			else if(currentPiece.piece == PIECE_QUEEN || currentPiece.piece == PIECE_BISHOP)
				return true;
			else
				break;
		}
	}

	//Diagonal - Left Down
	x = (int)ourKingPosition.x;
	y = (int)ourKingPosition.y;
	while(++y < kBoardDimensions && --x >= 0)
	{
		BoardPiece currentPiece = boardToTest.currentLayout[x][y];
		if(currentPiece.piece != PIECE_NONE)
		{
			if( currentPiece.colour == teamColour)
				break;
			else if(currentPiece.piece == PIECE_QUEEN || currentPiece.piece == PIECE_BISHOP)
				return true;
			else
				break;
		}
	}

	//Diagonal - Left Up
	x = (int)ourKingPosition.x;
	y = (int)ourKingPosition.y;
	while(--y >= 0 && --x >= 0)
	{
		BoardPiece currentPiece = boardToTest.currentLayout[x][y];
		if(currentPiece.piece != PIECE_NONE)
		{
			if( currentPiece.colour == teamColour)
				break;
			else if(currentPiece.piece == PIECE_QUEEN || currentPiece.piece == PIECE_BISHOP)
				return true;
			else
				break;
		}
	}

	//Awkward Knight moves
	x = (int)ourKingPosition.x+2;
	y = (int)ourKingPosition.y+1;
	if(x < kBoardDimensions && y < kBoardDimensions)
	{
		BoardPiece currentPiece = boardToTest.currentLayout[x][y];
		if( currentPiece.colour != teamColour && currentPiece.piece == PIECE_KNIGHT)
			return true;
	}

	x = (int)ourKingPosition.x+2;
	y = (int)ourKingPosition.y-1;
	if(x < kBoardDimensions && y >= 0)
	{
		BoardPiece currentPiece = boardToTest.currentLayout[x][y];
		if( currentPiece.colour != teamColour && currentPiece.piece == PIECE_KNIGHT)
			return true;
	}

	x = (int)ourKingPosition.x+1;
	y = (int)ourKingPosition.y+2;
	if(x < kBoardDimensions && y < kBoardDimensions)
	{
		BoardPiece currentPiece = boardToTest.currentLayout[x][y];
		if( currentPiece.colour != teamColour && currentPiece.piece == PIECE_KNIGHT)
			return true;
	}

	x = (int)ourKingPosition.x-1;
	y = (int)ourKingPosition.y+2;
	if(x >= 0 && y < kBoardDimensions)
	{
		BoardPiece currentPiece = boardToTest.currentLayout[x][y];
		if( currentPiece.colour != teamColour && currentPiece.piece == PIECE_KNIGHT)
			return true;
	}

	x = (int)ourKingPosition.x-2;
	y = (int)ourKingPosition.y+1;
	if(x >= 0 && y < kBoardDimensions)
	{
		BoardPiece currentPiece = boardToTest.currentLayout[x][y];
		if( currentPiece.colour != teamColour && currentPiece.piece == PIECE_KNIGHT)
			return true;
	}

	x = (int)ourKingPosition.x-2;
	y = (int)ourKingPosition.y-1;
	if(x >= 0 && y >= 0)
	{
		BoardPiece currentPiece = boardToTest.currentLayout[x][y];
		if( currentPiece.colour != teamColour && currentPiece.piece == PIECE_KNIGHT)
			return true;
	}

	x = (int)ourKingPosition.x-1;
	y = (int)ourKingPosition.y-2;
	if(x >= 0 && y >= 0)
	{
		BoardPiece currentPiece = boardToTest.currentLayout[x][y];
		if( currentPiece.colour != teamColour && currentPiece.piece == PIECE_KNIGHT)
			return true;
	}

	x = (int)ourKingPosition.x+1;
	y = (int)ourKingPosition.y-2;
	if(x < kBoardDimensions && y >= 0)
	{
		BoardPiece currentPiece = boardToTest.currentLayout[x][y];
		if( currentPiece.colour != teamColour && currentPiece.piece == PIECE_KNIGHT)
			return true;
	}

	//Opponent King positions
	for(int yPos = (int)ourKingPosition.y-1; yPos < (int)ourKingPosition.y+2; yPos++)
	{
		for(int xPos = (int)ourKingPosition.x-1; xPos < (int)ourKingPosition.x+2; xPos++)
		{
			if((xPos >= 0 && xPos < kBoardDimensions) && (yPos >= 0 && yPos < kBoardDimensions))
			{
				BoardPiece currentPiece = boardToTest.currentLayout[xPos][yPos];
				//Must be the opponents king, as w will pass over our own in this embedded loop.
				if( currentPiece.colour != teamColour && currentPiece.piece == PIECE_KING)
					return true;
			}
		}
	}

	//Opponent Pawns
	int opponentPawnDirection = teamColour == COLOUR_WHITE ? -1 : 1;
	x = (int)ourKingPosition.x+1;
	y = (int)ourKingPosition.y+opponentPawnDirection;
	if(x < kBoardDimensions && y >= 0 && y < kBoardDimensions)
	{
		BoardPiece currentPiece = boardToTest.currentLayout[x][y];
		if( currentPiece.colour != teamColour && currentPiece.piece == PIECE_PAWN)
			return true;
	}
	
	x = (int)ourKingPosition.x-1;
	y = (int)ourKingPosition.y+opponentPawnDirection;
	if(x >= 0 && y >= 0 && y < kBoardDimensions)
	{
		BoardPiece currentPiece = boardToTest.currentLayout[x][y];
		if( currentPiece.colour != teamColour && currentPiece.piece == PIECE_PAWN)
			return true;
	}

	return false;
}
/*
bool ChessPlayer::CheckForCheck(Board boardToTest, COLOUR teamColour)
{
	Vector2D ourKingPosition;
	COLOUR opponentColour = teamColour == COLOUR_WHITE ? COLOUR_BLACK : COLOUR_WHITE;

	//Compile all the moves available to our opponent.
	vector<Move> allMoves;// = new vector<Vector2D>();
	GetAllMoveOptions(boardToTest, opponentColour, &allMoves);

	//Go through the board and find our KING's position.
	for(int x = 0; x < kBoardDimensions; x++)
	{
		for(int y = 0; y < kBoardDimensions; y++)
		{
			BoardPiece currentPiece = boardToTest.currentLayout[x][y];
			if(currentPiece.colour == teamColour && currentPiece.piece == PIECE_KING)
			{
				//Store our KING's position whilst we go through the board.
				ourKingPosition = Vector2D(x,y);
			}
		}
	}

	//Now we have a list of all enemy moves (in board positioning (8x8)), is our KING in one of them?
	for(unsigned int i = 0; i < allMoves.size(); i++)
	{
		if(allMoves[i].to_X == ourKingPosition.x && allMoves[i].to_Y == ourKingPosition.y)
			return true;
	}

	return false;
}*/
//--------------------------------------------------------------------------------------------------

bool ChessPlayer::CheckForCheckmate( Board boardToCheck, COLOUR teamColour )
{
	//If we are in CHECK, can we actually make a move to get us out of it?
	if( mInCheck )
	{
		vector<Move> moves;
		GetAllMoveOptions( boardToCheck, teamColour, &moves );

		//If there are no valid moves then this unfortunately is CHECKMATE.
		if( moves.empty() )
			return true;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------

bool ChessPlayer::CheckForStalemate( Board boardToCheck, COLOUR teamColour )
{
	//If we are not in CHECK, can we actually make a move? If not then we are in a STALEMATE.
	if( !mInCheck )
	{
		vector<Move> moves;
		GetAllMoveOptions( boardToCheck, teamColour, &moves );

		//If there are no valid moves then this unfortunately is STALEMATE.
		if( moves.empty() )
			return true;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------

bool ChessPlayer::TakeATurn( SDL_Event e )
{
	switch( e.type )
	{
		//Deal with mouse click input.
		case SDL_MOUSEBUTTONUP:
			switch( e.button.button )
			{
				case SDL_BUTTON_LEFT:
					GetNumberOfLivingPieces();
					return MakeAMove( Vector2D(e.button.x/kChessPieceDimensions, e.button.y/kChessPieceDimensions) );
				break;

				default:
				break;
			}
		break;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------

void ChessPlayer::EndTurn()
{
	//Change move type.
	mCurrentMove = SELECT_A_PIECE;

	//Remove highlights.
	mHighlightPositions->clear();

	//Make all current player's pawns unavailable for en'passant. Opponent had their chance.
	ClearEnPassant();
}

//--------------------------------------------------------------------------------------------------

void ChessPlayer::RenderPawnPromotion()
{
	int halfHeight = (int)(mSelectAPieceTexture->GetHeight()*0.5f);
	int width = (int)mSelectAPieceTexture->GetWidth();
	SDL_Rect portionOfSpritesheet = {0, (int)(mTeamColour*halfHeight), width, halfHeight};
	mSelectAPieceTexture->Render( portionOfSpritesheet, mPawnPromotionDrawPosition );
}

//--------------------------------------------------------------------------------------------------
