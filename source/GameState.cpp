// Alap dolgok:
#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// OpenCV inkl�dok:
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

// Saj�t oszt�lyaim inkl�d:
#include "Field.h"
#include "Chessboard.h"

// Saj�t header:
#include "GameState.h"

namespace gamestate
{
	
	/* GameState f�ggv�nyei */
	// GUI k�pek beolvas�sa
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

	// GUI-ra k�p rajzol�sa:
	void GameState::putChesspieceOnGUI(BasicField Field)
	{
		// A GUI k�pen ezen sz�mok szerint fogom meg�rteni magam:
		int field_multiplyer = 50; // 50 k�pkock�nk�nt vannak a mez�k
		int xNum = (int) Field.HorizontalId - 1; // V�zszintesen mennyit l�pek
		int yNum = (int) Field.VerticalId - 1; // F�gg�legesen mennyit l�pek (vissza)
		int pieceColor = (Field.ChesspieceColor == BasicField::black) ? 0 : 1;
		int fieldColor = (Field.FieldColor == BasicField::black) ? 0 : 1;
		int piece_type = 0;

		if (Field.ChesspieceType == BasicField::king) piece_type = 5;
		else if (Field.ChesspieceType == BasicField::queen) piece_type = 4;
		else if (Field.ChesspieceType == BasicField::rook) piece_type = 3;
		else if (Field.ChesspieceType == BasicField::bishop) piece_type = 2;
		else if (Field.ChesspieceType == BasicField::knight) piece_type = 1;

		// Rajzoljunk a k�pre, ha van mit
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
		// T�bl�k mem�riafelszabad�t�sa:
		for (vector<BasicChessboard*>::iterator cbIt = ChessboardVector.begin();
			cbIt < ChessboardVector.end();
			cbIt++)
		{
			delete *cbIt;
		}

		// L�p�sek mem�riafelszabad�t�sa:
		for (vector<Step*>::iterator stIt = StepVector.begin();
			stIt < StepVector.end();
			stIt++)
		{
			delete *stIt;
		}

		// B�bu k�pek eldob�sa:
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

	// Els� sakkt�bl�n b�buk inicializ�l�sa
	void GameState::initChessPieces(BasicChessboard* Chessboard)
	{
		// Itt nincs mit tenni, felsoroljuk egyenk�nt, hogy melyik mez�n milyen b�bu lesz.
		// Vil�gos tisztek:
		Chessboard->Fields[0][0].ChesspieceType = BasicField::rook;
		Chessboard->Fields[1][0].ChesspieceType = BasicField::knight;
		Chessboard->Fields[2][0].ChesspieceType = BasicField::bishop;
		Chessboard->Fields[3][0].ChesspieceType = BasicField::queen;
		Chessboard->Fields[4][0].ChesspieceType = BasicField::king;
		Chessboard->Fields[5][0].ChesspieceType = BasicField::bishop;
		Chessboard->Fields[6][0].ChesspieceType = BasicField::knight;
		Chessboard->Fields[7][0].ChesspieceType = BasicField::rook;
		// Vil�gos parasztok:
		for (int Idx = 0; Idx < 8; Idx++)
		{
			Chessboard->Fields[Idx][1].ChesspieceType = BasicField::pawn;
		}

		// S�t�t tisztek:
		Chessboard->Fields[0][7].ChesspieceType = BasicField::rook;
		Chessboard->Fields[1][7].ChesspieceType = BasicField::knight;
		Chessboard->Fields[2][7].ChesspieceType = BasicField::bishop;
		Chessboard->Fields[3][7].ChesspieceType = BasicField::queen;
		Chessboard->Fields[4][7].ChesspieceType = BasicField::king;
		Chessboard->Fields[5][7].ChesspieceType = BasicField::bishop;
		Chessboard->Fields[6][7].ChesspieceType = BasicField::knight;
		Chessboard->Fields[7][7].ChesspieceType = BasicField::rook;
		// S�t�t parasztok:
		for (int Idx = 0; Idx < 8; Idx++)
		{
			Chessboard->Fields[Idx][6].ChesspieceType = BasicField::pawn;
		}

	}

	// Sakkt�bla t�rol�sa
	bool GameState::storeChessboard(BasicChessboard* Chessboard)
	{
		// Ha az els� t�bla j�tt be
		if (ChessboardVector.empty())
		{
			// B�buk inicializ�l�sa
			initChessPieces(Chessboard);
			// Tegy�k el a t�bl�t
			ChessboardVector.push_back(Chessboard);
		}
		else // TODO ez el�g er�sen refaktorra szorul, mert kurva nagy
		{
			// Hiteless�g teszt, b�buk sz�m�nak ellen�rz�se
			int pieceCountLast = 0;
			int pieceCountNew = 0;

			// Utols� t�bla
			BasicChessboard* lastCb = ChessboardVector.at(ChessboardVector.size() - 1);

			// B�bukat megsz�molom a r�gi t�bl�n
			for (int idx1 = 0; idx1 < 8; idx1++)
			{
				for (int idx2 = 0; idx2 < 8; idx2++)
				{
					if (lastCb->Fields[idx1][idx2].ChesspieceOn == true) pieceCountLast++;
				}
			}
			// B�bukat megsz�molom az �j t�bl�n
			for (int idx1 = 0; idx1 < 8; idx1++)
			{
				for (int idx2 = 0; idx2 < 8; idx2++)
				{
					if (Chessboard->Fields[idx1][idx2].ChesspieceOn == true) pieceCountNew++;
				}
			}
			// Csak egy b�buval lehet kevesebb
			if ((pieceCountLast - pieceCountNew) > 1)
				return false;



			// Ezekbe n�zem meg, hogy mi v�ltozott
			bool difference = false;
			int sourceFound = 0;
			int destinationFound = 0;
			BasicField Source;
			BasicField Destination;

			// N�zem minden mez�n a k�l�nbs�geket az el�z�h�z k�pest
			for (int idx1 = 0; idx1 < 8; idx1++)
			{
				for (int idx2 = 0; idx2 < 8; idx2++)
				{
					// Azokat n�zz�k, ahol az el�z� t�bl�n volt b�bu
					if (lastCb->Fields[idx1][idx2].ChesspieceOn == true)
					{
						// Ahol m�r nincs b�bu, onnan ell�pt�nk
						if (Chessboard->Fields[idx1][idx2].ChesspieceOn == false)
						{
							Source = lastCb->Fields[idx1][idx2];
							difference = true;
							sourceFound++;
						}
						// Ahol b�bu sz�ne v�ltozott, oda l�pt�nk
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
					// Ahol az el�z� t�bl�n nem volt b�bu
					else
					{
						// Ha most van, akkor oda l�pt�nk
						if (Chessboard->Fields[idx1][idx2].ChesspieceOn == true)
						{
							Destination = Chessboard->Fields[idx1][idx2];
							difference = true;
							destinationFound++;
						}
					}
				}
			}

			// Ha nincs k�l�nbs�g, hazak�ld�m
			if (difference == false) return false;

			// Mindkett�b�l csak egy lehet
			if (sourceFound != 1) return false;
			if (destinationFound != 1) return false;

			// TODO a step vektorba is menteni

			// Ha van k�l�nbs�g, akkor �tm�solom a b�bukat okosan
			for (int idx1 = 0; idx1 < 8; idx1++)
			{
				for (int idx2 = 0; idx2 < 8; idx2++)
				{
					// Ha a forr�s mez�n vagyok
					if ((Source.HorizontalId == lastCb->Fields[idx1][idx2].HorizontalId) &&
						(Source.VerticalId == lastCb->Fields[idx1][idx2].VerticalId))
					{
						// Nem csin�lok semmit, hiszen nem kell b�bu adatot �tm�solni
					}
					// Ha az �rkez� mez�n vagyok
					else if ((Destination.HorizontalId == lastCb->Fields[idx1][idx2].HorizontalId) &&
						(Destination.VerticalId == lastCb->Fields[idx1][idx2].VerticalId))
					{
						// �tm�solom a Source b�but�pus�t az �j sakkt�bla kijel�lt index� mez�j�re
						Chessboard->Fields[idx1][idx2].ChesspieceType = Source.ChesspieceType;
					}
					else
					{
						// Az �sszes t�bbi mez�n �tm�solom a b�bu t�pus�t
						Chessboard->Fields[idx1][idx2].ChesspieceType = lastCb->Fields[idx1][idx2].ChesspieceType;
					}
				}
			}

			// Elmentem az �j sakkt�bl�t
			ChessboardVector.push_back(Chessboard);
		}

		// Ilyenkor a GUI k�pet szedj�k be �jra, hogy tiszta legyen:
		gameGUI = cv::imread(GUI_BASE);

		// R�rajzolom az �j �ll�st
		for (int idx1 = 0; idx1 < 8; idx1++)
		{
			for (int idx2 = 0; idx2 < 8; idx2++)
			{
				putChesspieceOnGUI(Chessboard->Fields[idx1][idx2]);
			}
		}

		return true;
	}

	// Elk�rj�k a GUI-k�nt kirajzoland� k�pet:
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

}