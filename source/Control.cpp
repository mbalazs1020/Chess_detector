// Alap dolgok:
#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <string>

// OpenCV inkl�dok:
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

// Saj�t oszt�lyaim inkl�d:
#include "InputPicture.h"
#include "Field.h"
#include "Chessboard.h"
#include "ChessMessage.h"
#include "GameState.h"

// Saj�t header:
#include "Control.h"


namespace control
{
	// Kontroll oszt�ly konstruktora:
	Control::Control()
	{
		// K�pem inicializ�l�sa:
		inputPic.open(STARTING_SCREEN);
		Mat newPicture = inputPic.getPicture();
		newPicture.copyTo(ChessboardPicture);
		RawPicture = ChessboardPicture;

		// Kamera megnyit�sa:
		inputPic.open(CAMERA_ID);

	}
	Control::Control(const string File)
	{
		// Be�ll�tom, hogy vide�r�l megyek
		InputType = VIDEO;

		// K�pem inicializ�l�sa:
		inputPic.open(STARTING_SCREEN);
		Mat newPicture = inputPic.getPicture();
		newPicture.copyTo(ChessboardPicture);
		RawPicture = ChessboardPicture;

		// Kamera megnyit�sa:
		inputPic.openVideo(File);

	}

	// M�k�d�s f� �llapotg�pe:
	bool Control::runMainCycle(void)
	{
		switch (State)
		{
		case CHECK_ORIENTATION: // Orient�ci� ellen�rz�s:
			if (checkOrientation())
			{
				State = CALIBRATION;
			}
			else
			{
				State = SET_UP_CHESSBOARD;
			}
			break;
		case SET_UP_CHESSBOARD: // T�bla fel�ll�t�sa:
			if (setUpChessBoard())
			{
				State = CHECK_ORIENTATION;
			}
			else
			{
				if (State == EXIT) break;
			}
			break;
		case CALIBRATION:       // Felismer�s kalibr�ci�ja:
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
		case EXIT:             // Kil�p�s:
			return false;
			break;
		default:
			std::cout << "ERROR: Invalid state!" << endl;
			break;
		}
		return true;
	}

	// Run �llapot m�k�d�se:
	bool Control::run(void)
	{
		// Ha m�r lehet, akkor k�r�nk �j k�pet:
		if (NewSampleAllowed())
		{
			if (inputPic.isOpened())
			{
				Mat newPicture = inputPic.getPicture();
				newPicture.copyTo(RawPicture);
				Chessboard Cb(RawPicture);
				if (Cb.isValid())
				{
					// Ments�k el a t�bl�t
					gameState.storeChessboard(new BasicChessboard(Cb));

					// Rajzoljuk ki a GUI k�p�t, ha van
					if (gameState.getGameGUI(gameGUI))
					{
						cv::imshow("GUI" , gameGUI);
						cv::moveWindow("GUI", 30, 30);
						waitKey(WAITKEY_TIME);
					}

					// Vegy�k �t a feldolgozott k�pet:
					ChessboardPicture = Cb.getPicture();
					// Rajzoljuk ki a k�pet:
					imshow("Picture", ChessboardPicture);
					cv::moveWindow("Picture", 30, 30);
					// Lehet�s�g kil�p�sre:
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
					// Lehet�s�g kil�p�sre:
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

	// T�bla fel�ll�t�sa:
	bool Control::setUpChessBoard(void)
	{
		char key = 0;
		// Sz�veg
		std::cout << "Set up the Chessboard, then press \"C\" !" << endl;

		// Am�g a visszat�r�s nem helyes:
		while (key != 'c')
		{
			// V�rok �j mintav�teli alkalomra:
			if (NewSampleAllowed())
			{
				if (inputPic.isOpened())
				{
					//Mintav�tel:
					Mat newPicture = inputPic.getPicture();
					newPicture.copyTo(RawPicture);

					// Sz�veg elhelyez�se a k�pen:
					string darkside = "DARK SIDE";
					string whiteside = "WHITE SIDE";

					// Sz�vegek ki�r�sa a k�pre:
					cv::putText(RawPicture, darkside, cv::Point(20, 60), FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(0, 51, 255, 255), 6);
					cv::putText(RawPicture, whiteside, cv::Point(20, (RawPicture.size().height - 60)), FONT_HERSHEY_SIMPLEX, 2, cv::Scalar(51, 255, 255, 255), 6);

					// Rajzoljuk ki a k�pet:
					imshow("Picture", RawPicture);
					cv::moveWindow("Picture", 30, 30);

					// Lehet�s�g kil�p�sre:
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

	// Orient�ci� csekkol�s:
	bool Control::checkOrientation(void)
	{
		// Valid-e a sakkt�bla:
		bool valid = false;
		// Sz�veg
		std::cout << "Checking chessboard orientation..." << endl;

		// Am�g �rtelmes �rt�k nem j�n:
		while (!valid)
		{
			// V�rok �j mintav�teli alkalomra:
			while (!NewSampleAllowed());
			while (!inputPic.isOpened());

			//Mintav�tel:
			Mat newPicture = inputPic.getPicture();
			newPicture.copyTo(RawPicture);

			// Rajzoljuk ki a k�pet:
			imshow("Picture", RawPicture);
			cv::moveWindow("Picture", 30, 30);
			waitKey(WAITKEY_TIME);

			// Sakkt�bla l�trehoz�s
			Chessboard CbOriCheck(RawPicture);
			valid = CbOriCheck.isValid();
			if (valid)
			{
				// Orient�ci� ellen�rz�se
				bool ori = CbOriCheck.checkChessboardOrientation();
				if (ori)
				{
					// Ha helyes az orient�ci�:
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


	// Sakkt�bla kalibr�l�sa:
	bool Control::calibrateDetection(void)
	{
		// Valid-e a sakkt�bla:
		bool valid = false;
		// Sz�veg:
		std::cout << "Calibration of chesspiece recognition..." << endl;

		// Am�g a visszat�r�s nem helyes:
		while (valid == false)
		{
			// V�rok �j mintav�teli alkalomra:
			if (NewSampleAllowed())
			{
				if (inputPic.isOpened())
				{
					//Mintav�tel:
					Mat newPicture = inputPic.getPicture();
					newPicture.copyTo(RawPicture);

					// Rajzoljuk ki a k�pet:
					imshow("Picture", RawPicture);
					cv::moveWindow("Picture", 30, 30);

					// Lehet�s�g kil�p�sre:
					const char key = waitKey(WAITKEY_TIME);
					if (key == 27) // Escape gomb
					{
						State = EXIT;
						return false;
					}

					// Kalibr�ci�s sakkt�bla l�trehoz�sa:
					Chessboard Cb(RawPicture);
					valid = Cb.isValid();
					if (valid)
					{
						Cb.calibrateChessboard();
						// Sz�veg
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


	// 1 m�sodperces id�z�t�s:
	bool Control::NewSampleAllowed(void)
	{
		// Ha vide� van, akkor r�gt�n j�het:
		if (InputType == VIDEO)
		{
			return true;
		}

		// Mennyi az id�:
		ActualTime = clock();

		// Ha m�r a mintav�teli id� letelt:
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