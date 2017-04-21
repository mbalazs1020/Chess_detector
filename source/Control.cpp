// Alap dolgok:
#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string>

// OpenCV inklúdok:
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

// Saját osztályaim inklúd:
#include "InputPicture.h"
#include "Field.h"
#include "Chessboard.h"
#include "ChessMessage.h"
#include "GameState.h"

// Saját header:
#include "Control.h"


namespace control
{
	// Kontroll osztály konstruktora:
	Control::Control()
	{
		// Képem inicializálása:
		inputPic.open(STARTING_SCREEN);
		Mat newPicture = inputPic.getPicture();
		newPicture.copyTo(ChessboardPicture);
		RawPicture = ChessboardPicture;

		// Kamera megnyitása:
		inputPic.open(CAMERA_ID);

	}
	Control::Control(const string File)
	{
		// Beállítom, hogy videóról megyek
		InputType = VIDEO;

		// Képem inicializálása:
		inputPic.open(STARTING_SCREEN);
		Mat newPicture = inputPic.getPicture();
		newPicture.copyTo(ChessboardPicture);
		RawPicture = ChessboardPicture;

		// Kamera megnyitása:
		inputPic.openVideo(File);

	}

	// Mûködés fõ állapotgépe:
	bool Control::runMainCycle(void)
	{
		switch (State)
		{
		case CHECK_ORIENTATION: // Orientáció ellenõrzés:
			if (checkOrientation())
			{
				State = CALIBRATION;
			}
			else
			{
				State = SET_UP_CHESSBOARD;
			}
			break;
		case SET_UP_CHESSBOARD: // Tábla felállítása:
			if (setUpChessBoard())
			{
				State = CHECK_ORIENTATION;
			}
			else
			{
				if (State == EXIT) break;
			}
			break;
		case CALIBRATION:       // Felismerés kalibrációja:
			if (calibrateDetection())
			{
				State = RUN;
			}
			else
			{
				if (State == EXIT) break;
			}
			break;
		case RUN:
			if (!run())
			{
				if (State == EXIT) break;
			}
			break;
		case EXIT:             // Kilépés:
			return false;
			break;
		default:
			std::cout << "ERROR: Invalid state!" << endl;
			break;
		}
		return true;
	}

	// Run állapot mûködése:
	bool Control::run(void)
	{
		// Ha már lehet, akkor kérünk új képet:
		if (NewSampleAllowed())
		{
			if (inputPic.isOpened())
			{
				Mat newPicture = inputPic.getPicture();
				newPicture.copyTo(RawPicture);
				Chessboard Cb(RawPicture);
				if (Cb.isValid())
				{
					// Mentsük el a táblát
					gameState.storeChessboard(new BasicChessboard(Cb));

					// Rajzoljuk ki a GUI képét, ha van
					if (gameState.getGameGUI(gameGUI))
					{
						cv::imshow("GUI" , gameGUI);
						cv::moveWindow("GUI", 30, 30);
						waitKey(WAITKEY_TIME);
					}

					// Vegyük át a feldolgozott képet:
					ChessboardPicture = Cb.getPicture();
					// Rajzoljuk ki a képet:
					imshow("Picture", ChessboardPicture);
					cv::moveWindow("Picture", 30, 30);
					// Lehetõség kilépésre:
					const char key = waitKey(WAITKEY_TIME);
					if (key == 27) // Escape gomb
					{
						State = EXIT;
						return false;
					}
				}
				else
				{
					// Rajzoljuk ki a nyers:
					imshow("Picture", RawPicture);
					cv::moveWindow("Picture", 30, 30);
					// Lehetõség kilépésre:
					const char key = waitKey(WAITKEY_TIME);
					if (key == 27) // Escape gomb
					{
						State = EXIT;
						return false;
					}
				}
			}
		}
		return true;
	}

	// Tábla felállítása:
	bool Control::setUpChessBoard(void)
	{
		char key = 0;
		// Szöveg
		std::cout << "Set up the Chessboard, then press \"C\" !" << endl;

		// Amíg a visszatérés nem helyes:
		while (key != 'c')
		{
			// Várok új mintavételi alkalomra:
			if (NewSampleAllowed())
			{
				if (inputPic.isOpened())
				{
					//Mintavétel:
					Mat newPicture = inputPic.getPicture();
					newPicture.copyTo(RawPicture);

					// Szöveg elhelyezése a képen:
					string darkside = "DARK SIDE";
					string whiteside = "WHITE SIDE";

					// Szövegek kiírása a képre:
					cv::putText(RawPicture, darkside, cv::Point(20, 60), FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 51, 255, 255), 6);
					cv::putText(RawPicture, whiteside, cv::Point(20, (RawPicture.size().height - 60)), FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(51, 255, 255, 255), 6);

					// Rajzoljuk ki a képet:
					imshow("Picture", RawPicture);
					cv::moveWindow("Picture", 30, 30);

					// Lehetõség kilépésre:
					key = waitKey(WAITKEY_TIME);
					if (key == 27) // Escape gomb
					{
						State = EXIT;
						return false;
					}
				}
			}
		}
		return true;
	}

	// Orientáció csekkolás:
	bool Control::checkOrientation(void)
	{
		// Valid-e a sakktábla:
		bool valid = false;
		// Szöveg
		std::cout << "Checking chessboard orientation..." << endl;

		// Amíg értelmes érték nem jön:
		while (!valid)
		{
			// Várok új mintavételi alkalomra:
			while (!NewSampleAllowed());
			while (!inputPic.isOpened());

			//Mintavétel:
			Mat newPicture = inputPic.getPicture();
			newPicture.copyTo(RawPicture);

			// Rajzoljuk ki a képet:
			imshow("Picture", RawPicture);
			cv::moveWindow("Picture", 30, 30);
			waitKey(WAITKEY_TIME);

			// Sakktábla létrehozás
			Chessboard CbOriCheck(RawPicture);
			valid = CbOriCheck.isValid();
			if (valid)
			{
				// Orientáció ellenõrzése
				bool ori = CbOriCheck.checkChessboardOrientation();
				if (ori)
				{
					// Ha helyes az orientáció:
					std::cout << "Orientation corrent." << endl;
					return true;
				}
				else
				{
					// Ha nem helyes:
					std::cout << "Wrong orientation!" << endl;
					std::cout << "Make sure that the Bottom Left field is BLACK !" << endl;
					return false;
				}
			}
			else
			{
				std::cout << "Chessboard not found on the picture!" << endl;
				return false;
			}
		}
		return false;
	}


	// Sakktábla kalibrálása:
	bool Control::calibrateDetection(void)
	{
		// Valid-e a sakktábla:
		bool valid = false;
		// Szöveg:
		std::cout << "Calibration of chesspiece recognition..." << endl;

		// Amíg a visszatérés nem helyes:
		while (valid == false)
		{
			// Várok új mintavételi alkalomra:
			if (NewSampleAllowed())
			{
				if (inputPic.isOpened())
				{
					//Mintavétel:
					Mat newPicture = inputPic.getPicture();
					newPicture.copyTo(RawPicture);

					// Rajzoljuk ki a képet:
					imshow("Picture", RawPicture);
					cv::moveWindow("Picture", 30, 30);

					// Lehetõség kilépésre:
					const char key = waitKey(WAITKEY_TIME);
					if (key == 27) // Escape gomb
					{
						State = EXIT;
						return false;
					}

					// Kalibrációs sakktábla létrehozása:
					Chessboard Cb(RawPicture);
					valid = Cb.isValid();
					if (valid)
					{
						Cb.calibrateChessboard();
						// Szöveg
						std::cout << "Calibration done." << endl;
						std::cout << "Game detection is starting..." << endl;
						return true;
					}
					else
					{
						std::cout << "No chessboard on the picture!" << endl;
					}
				}
			}
		}
		return false;
	}


	// 1 másodperces idõzítés:
	bool Control::NewSampleAllowed(void)
	{
		// Ha videó van, akkor rögtön jöhet:
		if (InputType == VIDEO)
		{
			return true;
		}

		// Mennyi az idõ:
		ActualTime = clock();

		// Ha már a mintavételi idõ letelt:
		if ((ActualTime - LastSampleTime) > SAMPLING_TIME_MS)
		{
			LastSampleTime = ActualTime;
			return true;
		}
		else
		{
			return false;
		}
	}

}