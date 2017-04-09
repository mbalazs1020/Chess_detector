// Alap dolgok:
#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string>

// OpenCV inklúdok:
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

// Saját osztályaim inklúd:
#include "ChessMessage.h"
#include "Field.h"
#include "Chessboard.h"

// Saját header:
#include "GameState.h"

namespace gamestate
{
	
	/* GameState függvényei */
	// GUI képek beolvasása
	void GameState::initPieceGUIPics(void)
	{
		pieceGUIPics[0][0][0] = new Mat(cv::imread(black_pawn_on_black));
		pieceGUIPics[1][0][0] = new Mat(cv::imread(white_pawn_on_black));
		pieceGUIPics[0][0][1] = new Mat(cv::imread(black_pawn_on_white));
		pieceGUIPics[1][0][1] = new Mat(cv::imread(white_pawn_on_white));

		pieceGUIPics[0][1][0] = new Mat(cv::imread(black_knight_on_black));
		pieceGUIPics[1][1][0] = new Mat(cv::imread(white_knight_on_black));
		pieceGUIPics[0][1][1] = new Mat(cv::imread(black_knight_on_white));
		pieceGUIPics[1][1][1] = new Mat(cv::imread(white_knight_on_white));

		pieceGUIPics[0][2][0] = new Mat(cv::imread(black_bishop_on_black));
		pieceGUIPics[1][2][0] = new Mat(cv::imread(white_bishop_on_black));
		pieceGUIPics[0][2][1] = new Mat(cv::imread(black_bishop_on_white));
		pieceGUIPics[1][2][1] = new Mat(cv::imread(white_bishop_on_white));

		pieceGUIPics[0][3][0] = new Mat(cv::imread(black_rook_on_black));
		pieceGUIPics[1][3][0] = new Mat(cv::imread(white_rook_on_black));
		pieceGUIPics[0][3][1] = new Mat(cv::imread(black_rook_on_white));
		pieceGUIPics[1][3][1] = new Mat(cv::imread(white_rook_on_white));

		pieceGUIPics[0][4][0] = new Mat(cv::imread(black_queen_on_black));
		pieceGUIPics[1][4][0] = new Mat(cv::imread(white_queen_on_black));
		pieceGUIPics[0][4][1] = new Mat(cv::imread(black_queen_on_white));
		pieceGUIPics[1][4][1] = new Mat(cv::imread(white_queen_on_white));

		pieceGUIPics[0][5][0] = new Mat(cv::imread(black_king_on_black));
		pieceGUIPics[1][5][0] = new Mat(cv::imread(white_king_on_black));
		pieceGUIPics[0][5][1] = new Mat(cv::imread(black_king_on_white));
		pieceGUIPics[1][5][1] = new Mat(cv::imread(white_king_on_white));
	}

	// GUI-ra kép rajzolása:
	void GameState::putChesspieceOnGUI(BasicField Field)
	{
		// A GUI képen ezen számok szerint fogom megérteni magam:
		int field_multiplyer = 50; // 50 képkockánként vannak a mezõk
		int xNum = (int) Field.HorizontalId - 1; // Vízszintesen mennyit lépek
		int yNum = (int) Field.VerticalId - 1; // Függõlegesen mennyit lépek (vissza)
		int pieceColor = (Field.ChesspieceColor == BasicField::black) ? 0 : 1;
		int fieldColor = (Field.FieldColor == BasicField::black) ? 0 : 1;
		int piece_type = 0;

		if (Field.ChesspieceType == BasicField::king) piece_type = 5;
		else if (Field.ChesspieceType == BasicField::queen) piece_type = 4;
		else if (Field.ChesspieceType == BasicField::rook) piece_type = 3;
		else if (Field.ChesspieceType == BasicField::bishop) piece_type = 2;
		else if (Field.ChesspieceType == BasicField::knight) piece_type = 1;

		// Rajzoljunk a képre, ha van mit
		if (Field.ChesspieceOn == true)
		{
			pieceGUIPics[pieceColor]
				        [piece_type]
			            [fieldColor]->copyTo(gameGUI(cv::Rect( (xNum*field_multiplyer),
							                                   350 - (yNum * field_multiplyer),
				                                               pieceGUIPics[pieceColor][piece_type][fieldColor]->cols,
				                                               pieceGUIPics[pieceColor][piece_type][fieldColor]->rows)));

		}

	}

	// Konstruktor
	GameState::GameState()
	{
		initPieceGUIPics();
	}

	// Destruktor:
	GameState::~GameState()
	{
		// Táblák memóriafelszabadítása:
		for (vector<BasicChessboard*>::iterator cbIt = ChessboardVector.begin();
			cbIt < ChessboardVector.end();
			cbIt++)
		{
			delete *cbIt;
		}

		// Bábu képek eldobása:
		for (int idx1 = 0; idx1 < 2; idx1++)
		{
			for (int idx2 = 0; idx2 < 6; idx2++)
			{
				for (int idx3 = 0; idx3 < 2; idx3++)
				{
					delete pieceGUIPics[idx1][idx2][idx3];
				}
			}
		}
	}

	// Elsõ sakktáblán bábuk inicializálása
	void GameState::initChessPieces(BasicChessboard* Chessboard)
	{
		// Itt nincs mit tenni, felsoroljuk egyenként, hogy melyik mezõn milyen bábu lesz.
		// Világos tisztek:
		Chessboard->Fields[0][0].ChesspieceType = BasicField::rook;
		Chessboard->Fields[1][0].ChesspieceType = BasicField::knight;
		Chessboard->Fields[2][0].ChesspieceType = BasicField::bishop;
		Chessboard->Fields[3][0].ChesspieceType = BasicField::queen;
		Chessboard->Fields[4][0].ChesspieceType = BasicField::king;
		Chessboard->Fields[5][0].ChesspieceType = BasicField::bishop;
		Chessboard->Fields[6][0].ChesspieceType = BasicField::knight;
		Chessboard->Fields[7][0].ChesspieceType = BasicField::rook;
		// Világos parasztok:
		for (int Idx = 0; Idx < 8; Idx++)
		{
			Chessboard->Fields[Idx][1].ChesspieceType = BasicField::pawn;
		}

		// Sötét tisztek:
		Chessboard->Fields[0][7].ChesspieceType = BasicField::rook;
		Chessboard->Fields[1][7].ChesspieceType = BasicField::knight;
		Chessboard->Fields[2][7].ChesspieceType = BasicField::bishop;
		Chessboard->Fields[3][7].ChesspieceType = BasicField::queen;
		Chessboard->Fields[4][7].ChesspieceType = BasicField::king;
		Chessboard->Fields[5][7].ChesspieceType = BasicField::bishop;
		Chessboard->Fields[6][7].ChesspieceType = BasicField::knight;
		Chessboard->Fields[7][7].ChesspieceType = BasicField::rook;
		// Sötét parasztok:
		for (int Idx = 0; Idx < 8; Idx++)
		{
			Chessboard->Fields[Idx][6].ChesspieceType = BasicField::pawn;
		}

	}

	// Sakktábla tárolása
	bool GameState::storeChessboard(BasicChessboard* Chessboard)
	{
		// Ha az elsõ tábla jött be
		if (ChessboardVector.empty())
		{
			// Bábuk inicializálása
			initChessPieces(Chessboard);
			// Tegyük el a táblát
			ChessboardVector.push_back(Chessboard);

			// Ilyenkor a GUI képet szedjük be újra, hogy tiszta legyen:
			gameGUI = cv::imread(GUI_BASE);

			// Rárajzolom az új állást
			for (int idx1 = 0; idx1 < 8; idx1++)
			{
				for (int idx2 = 0; idx2 < 8; idx2++)
				{
					putChesspieceOnGUI(Chessboard->Fields[idx1][idx2]);
				}
			}
		}
		else // TODO ez elég erõsen refaktorra szorul, mert kurva nagy
		{
			// Hitelesség teszt, bábuk számának ellenõrzése
			int pieceCountLast = 0;
			int pieceCountNew = 0;

			// Utolsó táblát leveszem a vektorról
			BasicChessboard* lastCb = ChessboardVector.at(ChessboardVector.size() - 1);

			// Bábukat megszámolom a régi táblán
			for (int idx1 = 0; idx1 < 8; idx1++)
			{
				for (int idx2 = 0; idx2 < 8; idx2++)
				{
					if (lastCb->Fields[idx1][idx2].ChesspieceOn == true) pieceCountLast++;
				}
			}
			// Bábukat megszámolom az új táblán
			for (int idx1 = 0; idx1 < 8; idx1++)
			{
				for (int idx2 = 0; idx2 < 8; idx2++)
				{
					if (Chessboard->Fields[idx1][idx2].ChesspieceOn == true) pieceCountNew++;
				}
			}
			// Csak egy bábuval lehet kevesebb
			if ((pieceCountLast - pieceCountNew) > 1)
				return false;



			// Ezekbe nézem meg, hogy mi változott
			bool difference = false;
			int sourceFound = 0;
			int destinationFound = 0;
			BasicField Source;
			BasicField Destination;

			// Nézem minden mezõn a különbségeket az elõzõhöz képest
			for (int idx1 = 0; idx1 < 8; idx1++)
			{
				for (int idx2 = 0; idx2 < 8; idx2++)
				{
					// Azokat nézzük, ahol az elõzõ táblán volt bábu
					if (lastCb->Fields[idx1][idx2].ChesspieceOn == true)
					{
						// Ahol már nincs bábu, onnan elléptünk
						if (Chessboard->Fields[idx1][idx2].ChesspieceOn == false)
						{
							Source = lastCb->Fields[idx1][idx2];
							difference = true;
							sourceFound++;
						}
						// Ahol bábu színe változott, oda léptünk
						else if (lastCb->Fields[idx1][idx2].ChesspieceColor != Chessboard->Fields[idx1][idx2].ChesspieceColor)
						{
							// Ha van is valami rajta
							if (Chessboard->Fields[idx1][idx2].ChesspieceOn == true)
							{
								Destination = Chessboard->Fields[idx1][idx2];
								difference = true;
								destinationFound++;
							}
						}
					}
					// Ahol az elõzõ táblán nem volt bábu
					else
					{
						// Ha most van, akkor oda léptünk
						if (Chessboard->Fields[idx1][idx2].ChesspieceOn == true)
						{
							Destination = Chessboard->Fields[idx1][idx2];
							difference = true;
							destinationFound++;
						}
					}
				}
			}

			// Ha nincs különbség, hazaküldöm
			if (difference == false) return false;

			// Mindkettõbõl csak egy lehet
			if (sourceFound != 1) return false;
			if (destinationFound != 1) return false;

			// Háromból kettõ ellenõrzés a lépés elfogadára
			if ( !(this->filter.check2oo3( *Chessboard, Source, Destination ) ) )
				return false; // Ha nem lett jó, vagy még nincs meg a három, hazaküldöm

			// Elkérem az új helyes táblát a hárombólkettõ ellenõrzõtõl  <<<<<<<<<<<<<<<<<<<< ITT VAN A LÉNYEG, ITT FOGADTAM EL A LÉPÉST!!!!!!!!!!!!!!!!!!!!
			BasicChessboard* acceptedBoard = this->filter.getAcceptedBoard();

			// Elküldöm a TCP kapcsolaton, ha van
			if (isJavaCoreSet)
			{
				Move acceptedMove = this->filter.getAcceptedMove();
				String fieldCodeSource = acceptedMove.getSource().getFieldCodeString();
				char movStart[3] = { fieldCodeSource[0], fieldCodeSource[1], '\0' };
				String fieldCodeDest = acceptedMove.getDest().getFieldCodeString();
				char movEnd[3] = { fieldCodeDest[0], fieldCodeDest[1], '\0' };

				myJavaCoreReference->sendMoveToJavaChessCore(movStart, movEnd);  // <<<<<<<<<<<<<<<<<ITT KÜLDTEM EL
			}

			// Ha van különbség, akkor átmásolom a bábukat okosan
			for (int idx1 = 0; idx1 < 8; idx1++)
			{
				for (int idx2 = 0; idx2 < 8; idx2++)
				{
					// Ha a forrás mezõn vagyok
					if ((Source.HorizontalId == lastCb->Fields[idx1][idx2].HorizontalId) &&
						(Source.VerticalId == lastCb->Fields[idx1][idx2].VerticalId))
					{
						// Nem csinálok semmit, hiszen nem kell bábu adatot átmásolni
					}
					// Ha az érkezõ mezõn vagyok
					else if ((Destination.HorizontalId == lastCb->Fields[idx1][idx2].HorizontalId) &&
						(Destination.VerticalId == lastCb->Fields[idx1][idx2].VerticalId))
					{
						// Átmásolom a Source bábutípusát az új sakktábla kijelölt indexû mezõjére
						acceptedBoard->Fields[idx1][idx2].ChesspieceType = Source.ChesspieceType;
					}
					else
					{
						// Az összes többi mezõn átmásolom a bábu típusát
						acceptedBoard->Fields[idx1][idx2].ChesspieceType = lastCb->Fields[idx1][idx2].ChesspieceType;
					}
				}
			}

			// Elmentem az új sakktáblát
			ChessboardVector.push_back( new BasicChessboard( *acceptedBoard ) );

			// Ilyenkor a GUI képet szedjük be újra, hogy tiszta legyen:
			gameGUI = cv::imread(GUI_BASE);

			// Rárajzolom az új állást
			for (int idx1 = 0; idx1 < 8; idx1++)
			{
				for (int idx2 = 0; idx2 < 8; idx2++)
				{
					putChesspieceOnGUI(acceptedBoard->Fields[idx1][idx2]);
				}
			}

		}

		return true;
	}

	// Elkérjük a GUI-ként kirajzolandó képet:
	bool GameState::getGameGUI(cv::Mat& returnPic)
	{
		if (gameGUI.empty())
		{
			return false;
		}
		else
		{
			gameGUI.copyTo(returnPic);
			return true;
		}
	}


	// Átveszem, és megmondom, hogy jó-e már
	bool TwoOutOfThreeFilter::check2oo3(BasicChessboard cb, BasicField source, BasicField destination)
	{
		if ((numberOfReceivedMoves < 2)) // Elsõ vagy második input
		{
			this->cb[numberOfReceivedMoves] = cb; // Elteszem a dolgokat
			this->moves[numberOfReceivedMoves] = Move(source, destination);
			numberOfReceivedMoves++;
			return false;
		}
		else if (numberOfReceivedMoves == 2) // Harmadik input
		{
			this->cb[numberOfReceivedMoves] = cb; // Elteszem a dolgokat
			this->moves[numberOfReceivedMoves] = Move(source, destination);
			numberOfReceivedMoves = 0; // Mindenképp újra lesz kezdve, nullázom

			// Csekkolom, hogy legalább kettõ egyezik-e
			bool eq01 = (	(this->moves[0].getSource().HorizontalId == this->moves[1].getSource().HorizontalId) &&
							(this->moves[0].getSource().VerticalId   == this->moves[1].getSource().VerticalId) &&
							(this->moves[0].getDest().HorizontalId   == this->moves[1].getDest().HorizontalId ) &&
							(this->moves[0].getDest().VerticalId     == this->moves[1].getDest().VerticalId));
			bool eq02 =	(	(this->moves[0].getSource().HorizontalId == this->moves[2].getSource().HorizontalId) &&
							(this->moves[0].getSource().VerticalId   == this->moves[2].getSource().VerticalId) &&
							(this->moves[0].getDest().HorizontalId   == this->moves[2].getDest().HorizontalId) &&
							(this->moves[0].getDest().VerticalId     == this->moves[2].getDest().VerticalId));
			bool eq12 =	(	(this->moves[1].getSource().HorizontalId == this->moves[2].getSource().HorizontalId) &&
							(this->moves[1].getSource().VerticalId   == this->moves[2].getSource().VerticalId) &&
							(this->moves[1].getDest().HorizontalId   == this->moves[2].getDest().HorizontalId) &&
							(this->moves[1].getDest().VerticalId     == this->moves[2].getDest().VerticalId));

			if (!(eq01 || eq02 || eq12))
			{
				cout <<  "ERROR: 2oo3 filter didn't accept move!" << endl;
				return false; // Ha egyik se lett jó, akkor vége
			}

			// Ha valamelyik jó lett, beállítom a tábláját elfogadottnak
			if (eq01 || eq02)
			{
				delete acceptedBoard;
				acceptedBoard =	new BasicChessboard(this->cb[0]);
				acceptedMove = moves[0];
			}

			else if (eq12)
			{
				delete acceptedBoard;
				acceptedBoard = new BasicChessboard(this->cb[0]);
				acceptedMove = moves[1];
			}

			else
			{
				cout << "Hát ez meg hogy lehet?" << endl;
				return false;
			}

			return true;
		}
		else
		{
			cout << "VÉGZETES HIBA A HÁROMBÓLKETTÕ FILTERBEN" << endl;
			return false;
		}
	}

	// Az elfogadott tábla átvétele
	BasicChessboard* TwoOutOfThreeFilter::getAcceptedBoard(void)
	{
		return this->acceptedBoard;
	}

	// Az elfogadott lépés átvétele
	Move TwoOutOfThreeFilter::getAcceptedMove(void)
	{
		return acceptedMove;
	}

	// Beállítok referenciát a gamestate-re
	void GameState::setJavaCoreReference(ChessMessage* pChMess)
	{ 
		this->myJavaCoreReference = pChMess;
		isJavaCoreSet = true;
	}
	
}