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

// Field inklúdja
#include "Field.h"

// Saját header:
#include "Chessboard.h"


namespace chessboard
{
	// Chessboard ------------------------------------------------------
	// Konstruktor
	Chessboard::Chessboard(Mat ActualPicture)
	{
		// Saját képeim létrehozása:
		ActualPicture.copyTo(GreyPicture);

		// Átváltás HSV-be:
		cv::cvtColor(GreyPicture, Hpic, cv::COLOR_BGR2HSV);
		vector<cv::Mat> channels;
		cv::split(Hpic, channels);           // Színcsatornák leválasztása
		channels[0].copyTo(Hpic);
		channels[1].copyTo(Spic);
		channels[2].copyTo(Vpic);

		// Átváltás szürkeárnyalatosra a feldolgozáshoz:
		cv::cvtColor(GreyPicture, GreyPicture, cv::COLOR_BGR2GRAY);

		// Legyen a szürke kép a megjelenítõ:
		GreyPicture.copyTo(DisplayPicture);
		cv::cvtColor(DisplayPicture, DisplayPicture, CV_GRAY2BGR);

		// Thresholdozott kép létrehozása:
		GreyPicture.copyTo(ThreshPicture);

		cv::medianBlur(ThreshPicture, ThreshPicture, 7);  // Szûrés, hogy kevesebb zaj legyen benne
		cv::medianBlur(ThreshPicture, ThreshPicture, 5);
		cv::medianBlur(ThreshPicture, ThreshPicture, 3);
		cv::adaptiveThreshold(ThreshPicture, ThreshPicture, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 7, 0);

		cv::erode(ThreshPicture, ThreshPicture, Mat());  // Zaj további kiszûrése erózióval
		cv::dilate(ThreshPicture, ThreshPicture, Mat());
		cv::dilate(ThreshPicture, ThreshPicture, Mat());
		//cv::dilate(ThreshPicture, ThreshPicture, Mat());
	
		// Transzformált sarkpont koordináták megkapása:
		if (getCornerGoodCoordinates())
		{
			// Mezõk létrehozása:
			createFields();

			// Mezõkre kiírjuk amit tudni kell róluk:
			writeInformationOnPicture();

			// Sakktábla sikeresen létrejött:
			Valid = true;

			// Sakktábla validációs teszt
			Valid = chessboardValidationTest();
		}
	}


	// Mezõk létrehozása:
	void Chessboard::createFields(void)
	{
		// Tábla összes mezõjének létrehozása:
		for (int HorizIdx = BasicField::A; HorizIdx <= BasicField::H; HorizIdx++)
		{
			for (BasicField::VERTICAL_ID VertIdx = 1; VertIdx <= 8; VertIdx++)
			{
				// Field létrehozása:
				Field myField((BasicField::HORIZONTAL_ID)HorizIdx, VertIdx, CornerPoints, Hpic, Spic, Vpic, ThreshPicture);
				// Field tárolása:
				Fields[HorizIdx - 1][VertIdx - 1] = myField;
			}
		}
	}

	// Sakktábla validáció teszt
	bool Chessboard::chessboardValidationTest(void)
	{
		int pieceCount = 0; // Bábuk száma

		// Bábukat megszámolom
		for (int idx1 = 0; idx1 < 8; idx1++)
		{
			for (int idx2 = 0; idx2 < 8; idx2++)
			{
				if (Fields[idx1][idx2].ChesspieceOn == true) pieceCount++;
			}
		}
		// Ha több mint amennyi lehet, eldobom a táblát
		if (pieceCount > 32)
		{
			return false;
		}

		return true;
	}
	

	// Ráírja a képre a dolgokat
	void Chessboard::writeInformationOnPicture(void)
	{
		//Sakktábla sarkait lerajzoljuk:
		for (vector<Point2f>::iterator it = CornerPoints.begin();
			it != CornerPoints.end();
			it++
			)
		{
			cv::circle(DisplayPicture, *it, 3, cv::Scalar(0, 255, 0, 255), 3);
		}

		// Minden field-re a saját infóját kiírjuk:
		for (int idx1 = 0; idx1 < 8; idx1++)
		{
			for (int idx2 = 0; idx2 < 8; idx2++)
			{
				// Szöveg: Mezõ kódja, van-e bábu
				std::string Text = "\0";
				std::string FieldCode = Fields[idx1][idx2].getFieldCodeString();
				std::string PieceColor = Fields[idx1][idx2].ChesspieceColor == BasicField::black ? "BLACK" : "WHITE";

				cv::Scalar textColor(0, 255, 0, 255);
				if (Fields[idx1][idx2].ChesspieceOn)
				{
					if (Fields[idx1][idx2].ChesspieceColor == BasicField::white)
					{
						// Világos bábu
						textColor = cv::Scalar(0, 153, 255, 255);
					}
					else
					{
						// Sötét bábu
						textColor = cv::Scalar(0, 51, 255, 255);
					}
					Text = FieldCode; // +PieceColor;
				}
				else
				{
					// Ha nincs fent bábu:
					//textColor = cv::Scalar(102, 255, 51, 255);
				}

				// Szöveg képre rakása:
				cv::putText(this->DisplayPicture, Text, (cv::Point) Fields[idx1][idx2].getFieldCenter(), FONT_HERSHEY_SIMPLEX, 0.5, textColor, 2);
			}
		}

		/*
		for (std::vector<Point2f>::iterator it = CornerPoints.begin();
			it != CornerPoints.end();
			it++)
		{
			std::string PointText = std::to_string(PointNum) + ".(" + std::to_string((int)it->x) + "," + std::to_string((int)it->y) + ")";
			cv::putText(this->DisplayPicture, PointText, *it, FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(0, 0, 255, 255), 1);
			PointNum++;
		} */
	}

	

	// Sarkpontok koordinátáinak helyes megkapása:
	bool Chessboard::getCornerGoodCoordinates(void)
	{
		// Tábla megkeresése, sarkok visszaadása
		bool found = findChessboardCorners(this->GreyPicture, this->BoardSize, this->CornerPoints,
			CV_CALIB_CB_ADAPTIVE_THRESH);  ///| CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
		// Hibakezelés:
		if (!found)
		{
			return false;
		}
		else
		{
			// ----------------------------------------------------------------------------------
			// Rendezni kell a sarkpontokat, mert alapvetõen random sorrendben jelennek meg.
			// Sarkpont iteráció: keressük a min és max y koordinátájú sarkpontokat:

			Point2f FirstPoint = CornerPoints[0];
			float MinY = 10000;
			float MaxY = 0;
			float MaxX = 0;
			float MinX = 10000;

			// Sarkpontokon (!) végiglépkedve
			std::vector<Point2f>::iterator it = CornerPoints.begin();
			for (int CornerIdx = 0; CornerIdx < 4; CornerIdx++)
			{
				// Szélsõ pontok kiválasztása a tábla helyzetének megállapításához
				if (it->y > MaxY) MaxY = it->y;
				if (it->y < MinY) MinY = it->y;
				if (it->x > MaxX) MaxX = it->x;
				if (it->x < MinX) MinX = it->x;

				// Következõ sarkpontra lépés:
				if (CornerIdx % 2 == 0) it += 8;
				else if (CornerIdx == 1) it += 64;

			}
			// Meglévõ sarkpontok transzformációjának 4 esetre bontása:
			// Határok meghúzása:
			float xBorder = MinX + ((MaxX - MinX) / 2);
			float yBorder = MinY + ((MaxY - MinY) / 2);

			if (FirstPoint.x <= xBorder && FirstPoint.y <= yBorder) transformCorners90();
			else if (FirstPoint.x < xBorder && FirstPoint.y > yBorder); // Ez alapból jó
			else if (FirstPoint.x > xBorder && FirstPoint.y < yBorder) transformCorners180();
			else if (FirstPoint.x >= xBorder && FirstPoint.y >= yBorder) transformCorners270();
			else
			{
				cout << "Rohadt nagy error a sarkpont koordináta transzformációban!" << endl;
				return false;
			}

			return true;
		}
	}


	// Sarokpont transzformációk:
	void Chessboard::transformCorners90(void)
	{
		vector<Point2f> CornerPointsTemp;

		for (int ColIdx = 72; ColIdx <= 80; ColIdx++)
		{
			for (int RowIdx = 0; RowIdx < 9; RowIdx++)
			{ 
				CornerPointsTemp.push_back( CornerPoints[ColIdx - 9 * RowIdx] );
			}
		}
		CornerPoints = CornerPointsTemp;
	}
	void Chessboard::transformCorners180(void)
	{
		vector<Point2f> CornerPointsTemp;

		for (int ColIdx = 80; ColIdx >= 8; ColIdx -= 9)
		{
			for (int RowIdx = 0; RowIdx < 9; RowIdx++)
			{
				CornerPointsTemp.push_back ( CornerPoints[ColIdx - RowIdx] );

			}
		}
		CornerPoints = CornerPointsTemp;
	}
	void Chessboard::transformCorners270(void)
	{
		vector<Point2f> CornerPointsTemp;

		for (int ColIdx = 8; ColIdx >= 0; ColIdx--)
		{
			for (int RowIdx = 0; RowIdx < 9; RowIdx++)
			{
				CornerPointsTemp.push_back ( CornerPoints[ColIdx + 9* RowIdx] );

			}
		}
		CornerPoints = CornerPointsTemp;
	}



	// Kalibráció: threshold értékek megállapítása:
	void Chessboard::calibrateChessboard(void)
	{
		// Mi a maximum értéke a bábu nélküli mezõnek:
		for (int i2 = 2; i2 < 6; i2++)
		{
			for (int i1 = 0; i1 < 8; i1++)
			{

				if (Field::DetectionThresh < Fields[i1][i2].getMeanOfThresh())
				{
					Field::DetectionThresh = Fields[i1][i2].getMeanOfThresh();
				}
			}
		}

		// Világos bábu színének megállapítása:
		for (int i2 = 0; i2 < 2; i2++)
		{
			for (int i1 = 0; i1 < 8; i1++)
			{
				double meanOfRoiV = Fields[i1][i2].getMeanOfRoiV();
				if (meanOfRoiV < Field::ColorOfWhitePieceMin) Field::ColorOfWhitePieceMin = meanOfRoiV;
				if (meanOfRoiV > Field::ColorOfWhitePieceMax) Field::ColorOfWhitePieceMax = meanOfRoiV;
			}
		}

		// Sötét bábu színének megállapítása:
		for (int i2 = 6; i2 < 8; i2++)
		{
			for (int i1 = 0; i1 < 8; i1++)
			{
				double meanOfRoiV = Fields[i1][i2].getMeanOfRoiV();
				if (meanOfRoiV < Field::ColorOfBlackPieceMin) Field::ColorOfBlackPieceMin = meanOfRoiV;
				if (meanOfRoiV > Field::ColorOfBlackPieceMax) Field::ColorOfBlackPieceMax = meanOfRoiV;
			}
		}
	}

	// Orientáció ellenõrzés:
	bool Chessboard::checkChessboardOrientation(void)
	{
		// Átlag tárolója:
		double Black = 0;
		double White = 0;

		// Sötét mezõ színe:
		Black = Fields[3][3].getMeanOfRoiS();

		// Világos mezõ színe:
		White = Fields[3][4].getMeanOfRoiS();

		// Átlag alapján jó helyen vannak-e a sötét mezõk:
		return (Black > White);
	}

	// Basic Chessboard létrehozása
	BasicChessboard::BasicChessboard(Chessboard Chessboard)
	{
		// Mezõk lemásolása:
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				this->Fields[i][j].HorizontalId = Chessboard.Fields[i][j].HorizontalId;
				this->Fields[i][j].VerticalId = Chessboard.Fields[i][j].VerticalId;
				this->Fields[i][j].FieldColor = Chessboard.Fields[i][j].FieldColor;
				this->Fields[i][j].ChesspieceOn = Chessboard.Fields[i][j].ChesspieceOn;
				this->Fields[i][j].ChesspieceType = Chessboard.Fields[i][j].ChesspieceType;
				this->Fields[i][j].ChesspieceColor = Chessboard.Fields[i][j].ChesspieceColor;
			}
		}
	}

	// Copy konstruktor:
	BasicChessboard::BasicChessboard(const BasicChessboard& other)
	{
		// Mezõk lemásolása:
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				this->Fields[i][j].HorizontalId = other.Fields[i][j].HorizontalId;
				this->Fields[i][j].VerticalId = other.Fields[i][j].VerticalId;
				this->Fields[i][j].FieldColor = other.Fields[i][j].FieldColor;
				this->Fields[i][j].ChesspieceOn = other.Fields[i][j].ChesspieceOn;
				this->Fields[i][j].ChesspieceType = other.Fields[i][j].ChesspieceType;
				this->Fields[i][j].ChesspieceColor = other.Fields[i][j].ChesspieceColor;
			}
		}
	}
}