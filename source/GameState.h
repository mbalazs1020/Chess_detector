#ifndef _GAME_STATE_H
#define _GAME_STATE_H

#include "PictureConsts.h"

using namespace std;
using namespace field;
using namespace chessboard;

namespace gamestate
{
	// L�p�s elt�rol�s�ra k�pes oszt�ly
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

	// H�romb�l kett� l�p�selfogad�s oszt�lya
	class TwoOutOfThreeFilter
	{
	private:
		unsigned int numberOfReceivedMoves = 0; // Ennyi elem j�tt be eddig

		// A be�rkezett l�p�sek elrakva
		Move moves[3];

		// A be�rkezett t�bl�k elrakva
		BasicChessboard cb[3];

		// Az elfogadott t�bla
		BasicChessboard* acceptedBoard;

	public:
		TwoOutOfThreeFilter(){}

		// �tveszem, �s megmondom, hogy j�-e m�r
		bool check2oo3(BasicChessboard cb, BasicField source, BasicField destination); 

		// Az elfogadott t�bla �tv�tele
		BasicChessboard* getAcceptedBoard(void);
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

		// Saj�t h�romb�l kett� sz�r� a l�p�selfogad�sra
		TwoOutOfThreeFilter filter;

		// GUI k�pek beolvas�sa
		void initPieceGUIPics(void);

		// GUI-ra k�p rajzol�sa:
		void putChesspieceOnGUI( BasicField Field );

		// B�buk inicializ�l�sa, els� t�bla
		void initChessPieces(BasicChessboard* Chessboard); 


	public:
		// Konstruktor
		GameState();

		// L�p�s kielemz�se, t�bla elment�se, ha t�rt�nt �j l�p�s
		bool storeChessboard(BasicChessboard* Chessboard);

		// Elk�rj�k a GUI-k�nt kirajzoland� k�pet:
		bool getGameGUI(cv::Mat& returnPic);

		// Destruktor:
		~GameState();
	};
}

#endif // _GAME_STATE_H