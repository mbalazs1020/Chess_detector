// Alap dolgok:
#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

// OpenCV inkl�dok:
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

// Field inkl�dja
#include "Field.h"

// Saj�t header:
#include "Chessboard.h"


namespace chessboard
{
	// Chessboard ------------------------------------------------------
	// Konstruktor
	Chessboard::Chessboard(Mat ActualPicture)
	{
		// Saj�t k�peim l�trehoz�sa:
		ActualPicture.copyTo(GreyPicture);

		// �tv�lt�s HSV-be:
		cv::cvtColor(GreyPicture, Hpic, cv::COLOR_BGR2HSV);
		vector<cv::Mat> channels;
		cv::split(Hpic, channels);           // Sz�ncsatorn�k lev�laszt�sa
		channels[0].copyTo(Hpic);
		channels[1].copyTo(Spic);
		channels[2].copyTo(Vpic);

		// �tv�lt�s sz�rke�rnyalatosra a feldolgoz�shoz:
		cv::cvtColor(GreyPicture, GreyPicture, cv::COLOR_BGR2GRAY);

		// Legyen a sz�rke k�p a megjelen�t�:
		GreyPicture.copyTo(DisplayPicture);
		cv::cvtColor(DisplayPicture, DisplayPicture, CV_GRAY2BGR);

		// Thresholdozott k�p l�trehoz�sa:
		GreyPicture.copyTo(ThreshPicture);

		cv::medianBlur(ThreshPicture, ThreshPicture, 7);  // Sz�r�s, hogy kevesebb zaj legyen benne
		cv::medianBlur(ThreshPicture, ThreshPicture, 5);
		cv::medianBlur(ThreshPicture, ThreshPicture, 3);
		cv::adaptiveThreshold(ThreshPicture, ThreshPicture, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 7, 0);

		cv::erode(ThreshPicture, ThreshPicture, Mat());  // Zaj tov�bbi kisz�r�se er�zi�val
		cv::dilate(ThreshPicture, ThreshPicture, Mat());
		cv::dilate(ThreshPicture, ThreshPicture, Mat());
		//cv::dilate(ThreshPicture, ThreshPicture, Mat());
	
		// Transzform�lt sarkpont koordin�t�k megkap�sa:
		if (getCornerGoodCoordinates())
		{
			// Mez�k l�trehoz�sa:
			createFields();

			// Mez�kre ki�rjuk amit tudni kell r�luk:
			writeInformationOnPicture();

			// Sakkt�bla sikeresen l�trej�tt:
			Valid = true;

			// Sakkt�bla valid�ci�s teszt
			Valid = chessboardValidationTest();
		}
	}


	// Mez�k l�trehoz�sa:
	void Chessboard::createFields(void)
	{
		// T�bla �sszes mez�j�nek l�trehoz�sa:
		for (int HorizIdx = BasicField::A; HorizIdx <= BasicField::H; HorizIdx++)
		{
			for (BasicField::VERTICAL_ID VertIdx = 1; VertIdx <= 8; VertIdx++)
			{
				// Field l�trehoz�sa:
				Field myField((BasicField::HORIZONTAL_ID)HorizIdx, VertIdx, CornerPoints, Hpic, Spic, Vpic, ThreshPicture);
				// Field t�rol�sa:
				Fields[HorizIdx - 1][VertIdx - 1] = myField;
			}
		}
	}

	// Sakkt�bla valid�ci� teszt
	bool Chessboard::chessboardValidationTest(void)
	{
		int pieceCount = 0; // B�buk sz�ma

		// B�bukat megsz�molom
		for (int idx1 = 0; idx1 < 8; idx1++)
		{
			for (int idx2 = 0; idx2 < 8; idx2++)
			{
				if (Fields[idx1][idx2].ChesspieceOn == true) pieceCount++;
			}
		}
		// Ha t�bb mint amennyi lehet, eldobom a t�bl�t
		if (pieceCount > 32)
		{
			return false;
		}

		return true;
	}
	

	// R��rja a k�pre a dolgokat
	void Chessboard::writeInformationOnPicture(void)
	{
		//Sakkt�bla sarkait lerajzoljuk:
		for (vector<Point2f>::iterator it = CornerPoints.begin();
			it != CornerPoints.end();
			it++
			)
		{
			cv::circle(DisplayPicture, *it, 3, cv::Scalar(0, 255, 0, 255), 3);
		}

		// Minden field-re a saj�t inf�j�t ki�rjuk:
		for (int idx1 = 0; idx1 < 8; idx1++)
		{
			for (int idx2 = 0; idx2 < 8; idx2++)
			{
				// Sz�veg: Mez� k�dja, van-e b�bu
				std::string Text = "\0";
				std::string FieldCode = Fields[idx1][idx2].getFieldCodeString();
				std::string PieceColor = Fields[idx1][idx2].ChesspieceColor == BasicField::black ? "BLACK" : "WHITE";

				cv::Scalar textColor(0, 255, 0, 255);
				if (Fields[idx1][idx2].ChesspieceOn)
				{
					if (Fields[idx1][idx2].ChesspieceColor == BasicField::white)
					{
						// Vil�gos b�bu
						textColor = cv::Scalar(0, 153, 255, 255);
					}
					else
					{
						// S�t�t b�bu
						textColor = cv::Scalar(0, 51, 255, 255);
					}
					Text = FieldCode; // +PieceColor;
				}
				else
				{
					// Ha nincs fent b�bu:
					//textColor = cv::Scalar(102, 255, 51, 255);
				}

				// Sz�veg k�pre rak�sa:
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

	

	// Sarkpontok koordin�t�inak helyes megkap�sa:
	bool Chessboard::getCornerGoodCoordinates(void)
	{
		// T�bla megkeres�se, sarkok visszaad�sa
		bool found = findChessboardCorners(this->GreyPicture, this->BoardSize, this->CornerPoints,
			CV_CALIB_CB_ADAPTIVE_THRESH);  ///| CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
		// Hibakezel�s:
		if (!found)
		{
			return false;
		}
		else
		{
			// ----------------------------------------------------------------------------------
			// Rendezni kell a sarkpontokat, mert alapvet�en random sorrendben jelennek meg.
			// Sarkpont iter�ci�: keress�k a min �s max y koordin�t�j� sarkpontokat:

			Point2f FirstPoint = CornerPoints[0];
			float MinY = 10000;
			float MaxY = 0;
			float MaxX = 0;
			float MinX = 10000;

			// Sarkpontokon (!) v�gigl�pkedve
			std::vector<Point2f>::iterator it = CornerPoints.begin();
			for (int CornerIdx = 0; CornerIdx < 4; CornerIdx++)
			{
				// Sz�ls� pontok kiv�laszt�sa a t�bla helyzet�nek meg�llap�t�s�hoz
				if (it->y > MaxY) MaxY = it->y;
				if (it->y < MinY) MinY = it->y;
				if (it->x > MaxX) MaxX = it->x;
				if (it->x < MinX) MinX = it->x;

				// K�vetkez� sarkpontra l�p�s:
				if (CornerIdx % 2 == 0) it += 8;
				else if (CornerIdx == 1) it += 64;

			}
			// Megl�v� sarkpontok transzform�ci�j�nak 4 esetre bont�sa:
			// Hat�rok megh�z�sa:
			float xBorder = MinX + ((MaxX - MinX) / 2);
			float yBorder = MinY + ((MaxY - MinY) / 2);

			if (FirstPoint.x <= xBorder && FirstPoint.y <= yBorder) transformCorners90();
			else if (FirstPoint.x < xBorder && FirstPoint.y > yBorder); // Ez alapb�l j�
			else if (FirstPoint.x > xBorder && FirstPoint.y < yBorder) transformCorners180();
			else if (FirstPoint.x >= xBorder && FirstPoint.y >= yBorder) transformCorners270();
			else
			{
				cout << "Rohadt nagy error a sarkpont koordin�ta transzform�ci�ban!" << endl;
				return false;
			}

			return true;
		}
	}


	// Sarokpont transzform�ci�k:
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



	// Kalibr�ci�: threshold �rt�kek meg�llap�t�sa:
	void Chessboard::calibrateChessboard(void)
	{
		// Mi a maximum �rt�ke a b�bu n�lk�li mez�nek:
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

		// Vil�gos b�bu sz�n�nek meg�llap�t�sa:
		for (int i2 = 0; i2 < 2; i2++)
		{
			for (int i1 = 0; i1 < 8; i1++)
			{
				double meanOfRoiV = Fields[i1][i2].getMeanOfRoiV();
				if (meanOfRoiV < Field::ColorOfWhitePieceMin) Field::ColorOfWhitePieceMin = meanOfRoiV;
				if (meanOfRoiV > Field::ColorOfWhitePieceMax) Field::ColorOfWhitePieceMax = meanOfRoiV;
			}
		}

		// S�t�t b�bu sz�n�nek meg�llap�t�sa:
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

	// Orient�ci� ellen�rz�s:
	bool Chessboard::checkChessboardOrientation(void)
	{
		// �tlag t�rol�ja:
		double Black = 0;
		double White = 0;

		// S�t�t mez� sz�ne:
		Black = Fields[3][3].getMeanOfRoiS();

		// Vil�gos mez� sz�ne:
		White = Fields[3][4].getMeanOfRoiS();

		// �tlag alapj�n j� helyen vannak-e a s�t�t mez�k:
		return (Black > White);
	}

	// Basic Chessboard l�trehoz�sa
	BasicChessboard::BasicChessboard(Chessboard Chessboard)
	{
		// Mez�k lem�sol�sa:
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
		// Mez�k lem�sol�sa:
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