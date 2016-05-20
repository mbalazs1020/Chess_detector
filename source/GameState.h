#ifndef _GAME_STATE_H
#define _GAME_STATE_H

#include "PictureConsts.h"

using namespace std;
using namespace field;
using namespace chessboard;

namespace gamestate
{
	// L�p�s elt�rol�s�ra k�pes oszt�ly
	class Step
	{
	private:
		BasicField Source;
		BasicField Destination;

	public:
		Step(){}
	};

	// J�t�k�ll�s oszt�lya
	class GameState
	{
	private:
		// GUI k�pei:
		cv::Mat gameGUI = cv::imread(GUI_BASE);
		cv::Mat* pieceGUIPics[2][6][2];

		// Ebbe mentj�k az �j sakkt�bl�t minden v�ltoz�sn�l
		vector<BasicChessboard*> ChessboardVector;
		// Ebbe mentj�k a l�p�seket
		vector<Step*> StepVector;

		// GUI k�pek beolvas�sa
		void initPieceGUIPics(void);

		// GUI-ra k�p rajzol�sa:
		void putChesspieceOnGUI( BasicField Field );

		// B�buk inicializ�l�sa, els� t�bla
		void initChessPieces(BasicChessboard* Chessboard);


	public:
		// Konstruktor
		GameState();

		// T�bla hozz�ad�sa, l�p�s kielemz�se
		bool storeChessboard(BasicChessboard* Chessboard);

		// Elk�rj�k a GUI-k�nt kirajzoland� k�pet:
		bool getGameGUI(cv::Mat& returnPic);

		// Destruktor:
		~GameState();
	};
}

#endif // _GAME_STATE_H