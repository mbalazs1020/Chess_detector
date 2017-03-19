#ifndef _CHESSBOARD_H_
#define _CHESSBOARD_H_

#include "PictureConsts.h"

// OpenCV �s standard n�vterek:
using namespace std;
using namespace cv;
using namespace field;


// Chessboard n�vt�r:
namespace chessboard
{
	// Chessboard oszt�ly k�pfeldolgoz�shoz
	class Chessboard
	{
	private:

		// Priv�t v�ltoz�im:
		Mat GreyPicture;
		Mat ThreshPicture;
		Mat Hpic;
		Mat Spic;
		Mat Vpic;
		Mat DisplayPicture;
		Size BoardSize{ 9, 9 };
		vector<Point2f> CornerPoints;

		bool Valid = false;

		// Priv�t f�ggv�nyeim:
		void transformCorners90(void);   // Sarkpont transzform�ci�k
		void transformCorners180(void);
		void transformCorners270(void);
		bool getCornerGoodCoordinates(void);

		// K�p feld�sz�t�se
		void writeInformationOnPicture(void);

		// Mez�k l�trehoz�sa
		void createFields(void);

		// Sakkt�bla valid�ci� teszt
		bool chessboardValidationTest(void);

	public:
		// Sakkt�bla mez�i l�that�ak
		Field Fields[8][8];

		// Konstruktor:
		Chessboard() {}
		Chessboard(Mat ActualPicture);

		// K�p visszaad�sa
		Mat getPicture() { return this->DisplayPicture; }
		// J�-e a sakkt�bla?
		bool isValid(void){ return Valid; }

		// Kalibr�ci�: threshold �rt�kek meg�llap�t�sa:
		void calibrateChessboard(void);

		// Orient�ci� ellen�rz�s:
		bool checkChessboardOrientation(void);
	};

	// Egyszer� sakkt�bla, amelyben a j�t�k �ll�s�t mentj�k
	class BasicChessboard
	{
	public:
		// Mez�i
		BasicField Fields[8][8];

		// Default konstruktor
		BasicChessboard(){}

		// L�trehoz�sa
		BasicChessboard(Chessboard Chessboard);

		// Copy konstruktor:
		BasicChessboard(const BasicChessboard& other);
	};

}

#endif // _CHESSBOARD_H_