#ifndef _CHESSBOARD_H_
#define _CHESSBOARD_H_

#include "PictureConsts.h"

// OpenCV és standard névterek:
using namespace std;
using namespace cv;
using namespace field;


// Chessboard névtér:
namespace chessboard
{
	// Chessboard osztály képfeldolgozáshoz
	class Chessboard
	{
	private:

		// Privát változóim:
		Mat GreyPicture;
		Mat ThreshPicture;
		Mat Hpic;
		Mat Spic;
		Mat Vpic;
		Mat DisplayPicture;
		Size BoardSize{ 9, 9 };
		vector<Point2f> CornerPoints;

		bool Valid = false;

		// Privát függvényeim:
		void transformCorners90(void);   // Sarkpont transzformációk
		void transformCorners180(void);
		void transformCorners270(void);
		bool getCornerGoodCoordinates(void);

		// Kép feldíszítése
		void writeInformationOnPicture(void);

		// Mezõk létrehozása
		void createFields(void);

		// Sakktábla validáció teszt
		bool chessboardValidationTest(void);

	public:
		// Sakktábla mezõi láthatóak
		Field Fields[8][8];

		// Konstruktor:
		Chessboard() {}
		Chessboard(Mat ActualPicture);

		// Kép visszaadása
		Mat getPicture() { return this->DisplayPicture; }
		// Jó-e a sakktábla?
		bool isValid(void){ return Valid; }

		// Kalibráció: threshold értékek megállapítása:
		void calibrateChessboard(void);

		// Orientáció ellenõrzés:
		bool checkChessboardOrientation(void);
	};

	// Egyszerû sakktábla, amelyben a játék állását mentjük
	class BasicChessboard
	{
	public:
		// Mezõi
		BasicField Fields[8][8];

		// Default konstruktor
		BasicChessboard(){}

		// Létrehozása
		BasicChessboard(Chessboard Chessboard);

		// Copy konstruktor:
		BasicChessboard(const BasicChessboard& other);
	};

}

#endif // _CHESSBOARD_H_