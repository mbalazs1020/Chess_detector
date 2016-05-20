#ifndef _FIELD_H_
#define _FIELD_H_

#include "PictureConsts.h"

// Haszn�land� n�vterek
using namespace cv;
using namespace std;

namespace field
{
	// Egyszer� mez� oszt�ly (a rohadt nagy k�peket nem t�rolja)
	class BasicField
	{
	public:
		// Mez� azonos�t�i:
		typedef enum
		{
			initID = -1, A = 1, B, C, D, E, F, G, H
		} HORIZONTAL_ID;
		typedef int VERTICAL_ID;

		// Sz�n:
		typedef enum { black, white, unknown_color } COLOR;

		// B�bu t�pusa
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
		// Ennek a konstruktora legyen protected, hogy ne hozhasson l�tre b�rki a semmib�l egy random sakkt�blamez�t.
		BasicField(HORIZONTAL_ID HorizontalId, VERTICAL_ID VerticalId);

		string HorizIdtoString(void);       // HorizontalID visszaad�sa string-k�nt

	public:
		// Default konstruktor
		BasicField(){}

		// Mez� k�dj�nak visszaad�sa sztringk�nt:
		std::string getFieldCodeString(void);
	};

	// Mez� oszt�ly a k�pfeldolgoz�shoz: az alap mez� oszt�ly gyereke
	class Field: public BasicField
	{
	public:
		// Kalibr�ci� �ltal meghat�rozott sz�ntartom�nyok:
		static double DetectionThresh;
		static double ColorOfWhitePieceMin;
		static double ColorOfWhitePieceMax;
		static double ColorOfBlackPieceMin;
		static double ColorOfBlackPieceMax;


	private:
		// Mez� v�ltoz�i:
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

		// F�ggv�nyek:
		void checkField(void);   // B�bu keres�se a mez�n


	public:
		// Default konstruktor, hogy l�tre tudjon j�nni a 8x8-as mez� t�bla:
		Field(){}
		// Konstruktor: l�trehoz�s mez� ID-k �s sarkpontok kordin�t�i alapj�n
		Field(HORIZONTAL_ID HorizontalId, VERTICAL_ID VerticalId, vector<Point2f> CornerPoints, Mat Hpic, Mat Spic, Mat Vpic, Mat TreshPicture);

		// K�z�ppont koordin�t�inak lek�rdez�se
		cv::Point getFieldCenter(void);

		// Mez�re jellemz� �tlag�rt�kek lek�rdez�se:
		double getMeanOfRoiH(void);
		double getMeanOfRoiS(void);
		double getMeanOfRoiV(void);
		double getMeanOfThresh(void);

	};
}

#endif