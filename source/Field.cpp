// Alap dolgok:
#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

// OpenCV inklúdok:
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

// Saját header:
#include "Field.h"

namespace field
{
	// Global:
	double Field::DetectionThresh = 0;
	double Field::ColorOfWhitePieceMin = 0xFFFFFFFF;
	double Field::ColorOfWhitePieceMax = 0;
	double Field::ColorOfBlackPieceMin = 0xFFFFFFFF;
	double Field::ColorOfBlackPieceMax = 0;

	// BasicField ------------------------------------------------------
	// Konstruktor
	BasicField::BasicField(HORIZONTAL_ID HorizontalId, VERTICAL_ID VerticalId) : HorizontalId(HorizontalId), VerticalId(VerticalId) {}

	// Mezõ horizontal ID string-ként visszaadása:
	string BasicField::HorizIdtoString(void)
	{
		switch (this->HorizontalId)
		{
		case A:
			return "A";
			break;
		case B:
			return "B";
			break;
		case C:
			return "C";
			break;
		case D:
			return "D";
			break;
		case E:
			return "E";
			break;
		case F:
			return "F";
			break;
		case G:
			return "G";
			break;
		case H:
			return "H";
			break;
		default:
			return "BAD";
			break;
		}
	}

	// Mezõkód visszaadása sztringként:
	std::string BasicField::getFieldCodeString(void)
	{
		return this->HorizIdtoString() + std::to_string(this->VerticalId);
	}

	// Field -----------------------------------------------------------
	// Konstruktor:
	Field::Field(HORIZONTAL_ID HorizontalId, VERTICAL_ID VerticalId, vector<Point2f> CornerPoints, Mat Hpic, Mat Spic, Mat Vpic, Mat TreshPicture) : BasicField(HorizontalId, VerticalId)
	{
		// Field sarkainak kiválasztása:
		FieldCorners[0] = CornerPoints[(VerticalId - 1 + 9 * (HorizontalId - 1))];
		FieldCorners[1] = CornerPoints[(VerticalId - 1 + 9 * (HorizontalId - 1)) + 1];
		FieldCorners[2] = CornerPoints[(VerticalId - 1 + 9 * (HorizontalId - 1)) + 9];
		FieldCorners[3] = CornerPoints[(VerticalId - 1 + 9 * (HorizontalId - 1)) + 10];

		// ROI kép kivágása a nagy képbõl:
		float MinY = 10000;
		float MaxY = 0;
		float MaxX = 0;
		float MinX = 10000;

		// Mezõ sarok koordináták min és max keresése
		for (int CornerIdx = 0; CornerIdx < 4; CornerIdx++)
		{
			// Szélsõ pontok kiválasztása a tábla helyzetének megállapításához
			if (FieldCorners[CornerIdx].y > MaxY) MaxY = FieldCorners[CornerIdx].y;
			if (FieldCorners[CornerIdx].y < MinY) MinY = FieldCorners[CornerIdx].y;
			if (FieldCorners[CornerIdx].x > MaxX) MaxX = FieldCorners[CornerIdx].x;
			if (FieldCorners[CornerIdx].x < MinX) MinX = FieldCorners[CornerIdx].x;
		}

		double BorderToTrackPiece = 0.2 * ((MaxX - MinX) + (MaxY - MinY)) / 2;
		double BorderToTrackColor = 0.4 * ((MaxX - MinX) + (MaxY - MinY)) / 2;

		// ROI:
		ROI_H = Hpic(cv::Rect((int)MinX + BorderToTrackColor, // Sarkok
			(int)MinY + BorderToTrackColor,
			(int)(MaxX - MinX - 2 * BorderToTrackColor),    // Szélek hossza
			(int)(MaxY - MinY - 2 * BorderToTrackColor)));

		ROI_S = Spic(cv::Rect((int)MinX + BorderToTrackColor, // Sarkok
			(int)MinY + BorderToTrackColor,
			(int)(MaxX - MinX - 2 * BorderToTrackColor),    // Szélek hossza
			(int)(MaxY - MinY - 2 * BorderToTrackColor)));

		ROI_V = Vpic(cv::Rect((int)MinX + BorderToTrackColor, // Sarkok
			(int)MinY + BorderToTrackColor,
			(int)(MaxX - MinX - 2 * BorderToTrackColor),    // Szélek hossza
			(int)(MaxY - MinY - 2 * BorderToTrackColor)));

		ROI_Thresh = TreshPicture(cv::Rect((int)MinX + BorderToTrackPiece, // Sarkok
			(int)MinY + BorderToTrackPiece,
			(int)(MaxX - MinX - 2 * BorderToTrackPiece),                // Szélek hossza
			(int)(MaxY - MinY - 2 * BorderToTrackPiece)));


		// ROI középponjának koordinátája:
		FieldCenter = cv::Point2f((MinX + MaxX) / 2, (MinY + MaxY) / 2);


		// Mezõ feldolgozása: szétválasztás világos és sötét mezõ esetére
		if (((HorizontalId == A ||
			HorizontalId == C ||
			HorizontalId == E ||
			HorizontalId == G) &&
			(VerticalId == 1 ||
			VerticalId == 3 ||
			VerticalId == 5 ||
			VerticalId == 7))
			||
			((HorizontalId == B ||
			HorizontalId == D ||
			HorizontalId == F ||
			HorizontalId == H) &&
			(VerticalId == 2 ||
			VerticalId == 4 ||
			VerticalId == 6 ||
			VerticalId == 8))
			)
		{
			this->FieldColor = black;
		}
		else
		{
			this->FieldColor = white;
		}

		// Bábu keresése, és színének megállapítása:
		checkField();
	}

	// Bábu keresése a mezõn:
	void Field::checkField(void)
	{
		/* A maximumokat nem kell, mert szabvány szerint a statikus változók nullára inicializálódnak. */

		// ROI színátlagának számítása, elmentése:
		meanOfRoiH = cv::mean(ROI_H).val[0];
		meanOfRoiS = cv::mean(ROI_S).val[0];
		meanOfRoiV = cv::mean(ROI_V).val[0];
		MeanOfThresh = cv::mean(ROI_Thresh).val[0];


		// Ha a thresholdot megüti, akkor van bábu
		if (MeanOfThresh > DetectionThresh + 20)
		{
			ChesspieceOn = true;

			// Színének megállapítása:
			if ((meanOfRoiV > (ColorOfBlackPieceMin)) &&     //Sötét tartományban van
				(meanOfRoiV < (ColorOfBlackPieceMax)))
			{
				ChesspieceColor = black;
			}
			else if ((meanOfRoiV >(ColorOfWhitePieceMin)) && // Világos tartományban van
				(meanOfRoiV < (ColorOfWhitePieceMax)))
			{
				ChesspieceColor = white;
			}
			else // Ha nehezen megállapítható az eset, akkor a színtartomány felétõl nézzük:
			{ // TODO bestatisztikázni hogy hány ilyen lesz, legyen ez robusztusabb!!
				if ((meanOfRoiV >((ColorOfBlackPieceMax + ColorOfWhitePieceMin) / 2)))
				{
					ChesspieceColor = white;
				}
				else
				{
					ChesspieceColor = black;
				}
			}
		}

	}


	// Középpont koordinátáinak lekérdezése
	cv::Point Field::getFieldCenter(void)
	{
		return this->FieldCenter;
	}

	// Mezõre jellemzõ átlagértékek lekérdezése:
	double Field::getMeanOfRoiH(void)
	{
		return meanOfRoiH;
	}
	double Field::getMeanOfRoiS(void)
	{
		return meanOfRoiS;
	}
	double Field::getMeanOfRoiV(void)
	{
		return meanOfRoiV;
	}
	double Field::getMeanOfThresh(void)
	{
		return MeanOfThresh;
	}

}