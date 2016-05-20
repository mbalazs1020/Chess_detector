#ifndef _GAME_STATE_H
#define _GAME_STATE_H

#include "PictureConsts.h"

using namespace std;
using namespace field;
using namespace chessboard;

namespace gamestate
{
	// Lépés eltárolására képes osztály
	class Step
	{
	private:
		BasicField Source;
		BasicField Destination;

	public:
		Step(){}
	};

	// Játékállás osztálya
	class GameState
	{
	private:
		// GUI képei:
		cv::Mat gameGUI = cv::imread(GUI_BASE);
		cv::Mat* pieceGUIPics[2][6][2];

		// Ebbe mentjük az új sakktáblát minden változásnál
		vector<BasicChessboard*> ChessboardVector;
		// Ebbe mentjük a lépéseket
		vector<Step*> StepVector;

		// GUI képek beolvasása
		void initPieceGUIPics(void);

		// GUI-ra kép rajzolása:
		void putChesspieceOnGUI( BasicField Field );

		// Bábuk inicializálása, elsõ tábla
		void initChessPieces(BasicChessboard* Chessboard);


	public:
		// Konstruktor
		GameState();

		// Tábla hozzáadása, lépés kielemzése
		bool storeChessboard(BasicChessboard* Chessboard);

		// Elkérjük a GUI-ként kirajzolandó képet:
		bool getGameGUI(cv::Mat& returnPic);

		// Destruktor:
		~GameState();
	};
}

#endif // _GAME_STATE_H