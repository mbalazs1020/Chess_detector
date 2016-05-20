#ifndef _FIELD_H_
#define _FIELD_H_

#include "PictureConsts.h"

// Használandó névterek
using namespace cv;
using namespace std;

namespace field
{
	// Egyszerû mezõ osztály (a rohadt nagy képeket nem tárolja)
	class BasicField
	{
	public:
		// Mezõ azonosítói:
		typedef enum
		{
			initID = -1, A = 1, B, C, D, E, F, G, H
		} HORIZONTAL_ID;
		typedef int VERTICAL_ID;

		// Szín:
		typedef enum { black, white, unknown_color } COLOR;

		// Bábu típusa
		typedef enum
		{
			unknown_piece = 0,
			pawn = 1,
			knight = 2,
			bishop = 3,
			rook = 5,
			queen = 9,
			king = 100
		} CHESSPIECE_TYPE;

		HORIZONTAL_ID HorizontalId = initID;
		VERTICAL_ID VerticalId = -1;
		COLOR FieldColor = unknown_color;
		bool ChesspieceOn = false;
		CHESSPIECE_TYPE ChesspieceType = unknown_piece;
		COLOR ChesspieceColor = unknown_color;

	protected:
		// Ennek a konstruktora legyen protected, hogy ne hozhasson létre bárki a semmibõl egy random sakktáblamezõt.
		BasicField(HORIZONTAL_ID HorizontalId, VERTICAL_ID VerticalId);

		string HorizIdtoString(void);       // HorizontalID visszaadása string-ként

	public:
		// Default konstruktor
		BasicField(){}

		// Mezõ kódjának visszaadása sztringként:
		std::string getFieldCodeString(void);
	};

	// Mezõ osztály a képfeldolgozáshoz: az alap mezõ osztály gyereke
	class Field: public BasicField
	{
	public:
		// Kalibráció által meghatározott színtartományok:
		static double DetectionThresh;
		static double ColorOfWhitePieceMin;
		static double ColorOfWhitePieceMax;
		static double ColorOfBlackPieceMin;
		static double ColorOfBlackPieceMax;


	private:
		// Mezõ változói:
		Point2f FieldCorners[4];
		Point2f FieldCenter;
		double meanOfRoiH;
		double meanOfRoiS;
		double meanOfRoiV;
		double MeanOfThresh;
		cv::Mat ROI_H;
		cv::Mat ROI_S;
		cv::Mat ROI_V;
		cv::Mat ROI_Thresh;

		// Függvények:
		void checkField(void);   // Bábu keresése a mezõn


	public:
		// Default konstruktor, hogy létre tudjon jönni a 8x8-as mezõ tábla:
		Field(){}
		// Konstruktor: létrehozás mezõ ID-k és sarkpontok kordinátái alapján
		Field(HORIZONTAL_ID HorizontalId, VERTICAL_ID VerticalId, vector<Point2f> CornerPoints, Mat Hpic, Mat Spic, Mat Vpic, Mat TreshPicture);

		// Középpont koordinátáinak lekérdezése
		cv::Point getFieldCenter(void);

		// Mezõre jellemzõ átlagértékek lekérdezése:
		double getMeanOfRoiH(void);
		double getMeanOfRoiS(void);
		double getMeanOfRoiV(void);
		double getMeanOfThresh(void);

	};
}

#endif