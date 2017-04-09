#ifndef _GAME_STATE_H
#define _GAME_STATE_H

#include "PictureConsts.h"

using namespace chess_message;
using namespace std;
using namespace field;
using namespace chessboard;

namespace gamestate
{
	// Lépés eltárolására képes osztály
	class Move
	{
	private:
		BasicField Source;
		BasicField Destination;

	public:
		Move(){}
		Move(BasicField Source, BasicField Destination) : Source(Source), Destination(Destination) {}

		BasicField getSource(){ return Source; }
		BasicField getDest(){ return Destination; }
	};

	// Háromból kettõ lépéselfogadás osztálya
	class TwoOutOfThreeFilter
	{
	private:
		unsigned int numberOfReceivedMoves = 0; // Ennyi elem jött be eddig

		// A beérkezett lépések elrakva
		Move moves[3];

		// A beérkezett táblák elrakva
		BasicChessboard cb[3];

		// Az elfogadott tábla
		BasicChessboard* acceptedBoard;
		Move             acceptedMove;

	public:
		TwoOutOfThreeFilter(){}

		// Átveszem, és megmondom, hogy jó-e már
		bool check2oo3(BasicChessboard cb, BasicField source, BasicField destination); 

		// Az elfogadott tábla átvétele
		BasicChessboard* getAcceptedBoard(void);

		// Az elfogadott lépés átvétele
		Move getAcceptedMove(void);
	};

	// Játékállás osztálya
	class GameState
	{
	private:
		// Adok neki egy referenciát a TCP kapcsolattartóhoz, hogy tudjon küldeni
		ChessMessage* myJavaCoreReference = NULL;
		bool isJavaCoreSet = false;

		// GUI képei:
		cv::Mat gameGUI = cv::imread(GUI_BASE);
		cv::Mat* pieceGUIPics[2][6][2];

		// Ebbe mentjük az új sakktáblát minden változásnál
		vector<BasicChessboard*> ChessboardVector;

		// Saját háromból kettõ szûrõ a lépéselfogadásra
		TwoOutOfThreeFilter filter;

		// GUI képek beolvasása
		void initPieceGUIPics(void);

		// GUI-ra kép rajzolása:
		void putChesspieceOnGUI( BasicField Field );

		// Bábuk inicializálása, elsõ tábla
		void initChessPieces(BasicChessboard* Chessboard); 


	public:
		// Konstruktor
		GameState();

		// Lépés kielemzése, tábla elmentése, ha történt új lépés
		bool storeChessboard(BasicChessboard* Chessboard);

		// Elkérjük a GUI-ként kirajzolandó képet:
		bool getGameGUI(cv::Mat& returnPic);

		// Beállítok referenciát a gamestate-re
		void setJavaCoreReference(ChessMessage* pChMess);

		// Destruktor:
		~GameState();
	};
}

#endif // _GAME_STATE_H