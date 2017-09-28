//------------------------------------------------------------------------
//  Author: Paul Roberts 2016
//------------------------------------------------------------------------

#include "GameScreen_Conway.h"
#include <stdlib.h>
#include "../Texture2D.h"
#include "ConwayConstants.h"
#include "../TinyXML/tinyxml.h"

//--------------------------------------------------------------------------------------------------

GameScreen_Conway::GameScreen_Conway(SDL_Renderer* renderer) : GameScreen(renderer)
{
	mMap = new int*[kConwayScreenWidth/kConwayTileDimensions];
	for(int i =0; i < kConwayScreenWidth/kConwayTileDimensions; i++)
		mMap[i] = new int[kConwayScreenHeight/kConwayTileDimensions];

	//Get all required textures.
	mWhiteTile = new Texture2D(renderer);
	mWhiteTile->LoadFromFile("Images/Conway/WhiteTile.png");
	mBlackTile = new Texture2D(renderer);
	mBlackTile->LoadFromFile("Images/Conway/BlackTile.png");

	//Start map has 50% active cells.
	CreateRandomMap(75);

	//Start game in frozen state.
	mPause = true;
}

//--------------------------------------------------------------------------------------------------

GameScreen_Conway::~GameScreen_Conway()
{
	delete mWhiteTile;
	mWhiteTile = NULL;

	delete mBlackTile;
	mBlackTile = NULL;

	for(int i =0; i < kConwayScreenWidth/kConwayTileDimensions; i++)
	{
		delete mMap[i];
		mMap[i] = NULL;
	}
	delete mMap;
	mMap = NULL;
}

//--------------------------------------------------------------------------------------------------

void GameScreen_Conway::Render()
{
	for(int x = 0; x < kConwayScreenWidth/kConwayTileDimensions; x++)
	{
		for(int y = 0; y < kConwayScreenHeight/kConwayTileDimensions; y++)
		{
			switch(mMap[x][y])
			{
				case 0:
					mBlackTile->Render(Vector2D(x*kConwayTileDimensions,y*kConwayTileDimensions));
				break;
				
				case 1:
					mWhiteTile->Render(Vector2D(x*kConwayTileDimensions,y*kConwayTileDimensions));
				break;
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------

void GameScreen_Conway::Update(size_t deltaTime, SDL_Event e)
{
	//Additional input outside of player.
	switch(e.type)
	{
		//Deal with mouse click input.
		case SDL_KEYUP:
			switch(e.key.keysym.sym)
			{
				case SDLK_SPACE:
					mPause = !mPause;
				break;

				case SDLK_r:
					mPause = true;
					CreateRandomMap(75);
				break;

				case SDLK_l:
					mPause = true;
					LoadMap("Conway/ConwaySeed.xml");
				break;
			}
		break;
	
		default:
		break;
	}
	
	if(!mPause)
		UpdateMap();
}

//--------------------------------------------------------------------------------------------------

void GameScreen_Conway::UpdateMap()
{
	//Rules
	//1. Any living cell with less than 2 live neighbour dies.
	//2. Any living cell with 2 or 3 live cells lives on to next generation.
	//3. Any live cell with more than 3 live neighbours dies.
	//4. Any dead cell with exactly 3 live neighbours becomes a living cell.

	//TODO: Code the above rules.
	DetermineState();
}

//--------------------------------------------------------------------------------------------------

void GameScreen_Conway::CreateRandomMap(int percentageInactive)
{
	int randomNumber = 0;

	for(int x = 0; x < kConwayScreenWidth/kConwayTileDimensions; x++)
	{
		for(int y = 0; y < kConwayScreenHeight/kConwayTileDimensions; y++)
		{
			randomNumber = (rand() % 100)+1;

			if(randomNumber > percentageInactive)
				mMap[x][y] = 1;
			else
				mMap[x][y] = 0;
		}
	}
}

//--------------------------------------------------------------------------------------------------

void GameScreen_Conway::LoadMap(std::string path)
{
	//Get the whole xml document.
	TiXmlDocument doc;
	if(!doc.LoadFile(path))
	{
		cerr << doc.ErrorDesc() << endl;
	}

	//Now get the root element.
	TiXmlElement* root = doc.FirstChildElement();
	if(!root)
	{
		cerr << "Failed to load file: No root element." << endl;
		doc.Clear();
	}
	else
	{
		//Jump to the first 'objectgroup' element.
		for(TiXmlElement* groupElement = root->FirstChildElement("objectgroup"); groupElement != NULL; groupElement = groupElement->NextSiblingElement())
		{
			string name = groupElement->Attribute("name");
			if(name == "Seed")
			{
				int x = 0;
				int y = 0;

				//Jump to the first 'object' element - within 'objectgroup'
				for(TiXmlElement* objectElement = groupElement->FirstChildElement("object"); objectElement != NULL; objectElement = objectElement->NextSiblingElement())
				{
					string name = objectElement->Attribute("name");
					if(name == "TileTypes")
					{
						//Reset x position of map to 0 at start of each element.
						x = 0;

						//Split up the comma delimited connections.
						stringstream ss(objectElement->Attribute("value"));
						int i;
						while(ss >> i)
						{
							mMap[x][y] = i;

							if(ss.peek() == ',')
								ss.ignore();

							//Increment the x position.
							x++;
						}

						//Move down to the next y position in the map.
						y++;
					}
				}
			}
		}
	}
}

void GameScreen_Conway::DetermineState()
{
	int ** mapTwo;
	mapTwo = new int*[(kConwayScreenWidth / kConwayTileDimensions) + 1];
	for (int i = 0; i < (kConwayScreenWidth / kConwayTileDimensions) + 1; i++)
		mapTwo[i] = new int[(kConwayScreenWidth / kConwayTileDimensions) + 1];

	for (int a = 0; a < kConwayScreenWidth / kConwayTileDimensions; a++)
	{
		for (int b = 0; b < kConwayScreenWidth / kConwayTileDimensions; b++)
		{
			mapTwo[a][b] = mMap[a][b];
		}
	}

	for (int a = 1; a < kConwayScreenWidth / kConwayTileDimensions; a++)
	{
		for (int b = 1; b < kConwayScreenWidth / kConwayTileDimensions; b++)
		{
			int alive = 0;
			for (int c = -1; c < 2; c++)
			{
				for (int d = -1; d < 2; d++)
				{
					if (!(c == 0 && d == 0))
					{
						if (mapTwo[a + c][b + d])
						{
							++alive;
						}
					}
				}
			}
			if (alive < 2)
			{
				mMap[a][b] = 0;
			}
			else if (alive == 3)
			{
				mMap[a][b] = 1;
			}
			else if (alive > 3)
			{
				mMap[a][b] = 0;
			}
		}
	}
}