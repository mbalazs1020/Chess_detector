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

// Saj�t header:
#include "InputPicture.h"


namespace input_picture
{

	// K�p visszad�sa:
	cv::Mat InputPicture::getPicture()
	{
		// �j k�p t�rol�
		cv::Mat newPic;

		// Kamera eset�n egyszer� a dolog
		if ((inputType == CAMERA) )
		{
			// Addig olvasom, ameddig j�n �j!!!
			do
			{
				inputCapture >> newPic;
			} while (newPic.empty());
			// Ez lesz az �j k�pem:
			newPic.copyTo(myPicture);
		}
		// Vide� eset�n �gy k�rj�k ki a k�pet, hogy az FPS-hez igazodjon
		if (inputType == VIDEO)
		{
			// Megn�zz�k, mennyi id� telt el:
			time_t timePassed = clock() - lastTime;
			int NumberOfPastFrames = (int)timePassed / TIME_BW_FRAMES_MS;


			// K�r�nk �j k�pet annyiszor, amennyi id� eltelt az�ta a vide�b�l
			for (int i = 0; i < NumberOfPastFrames; i++)
			{
				do
				{
					inputCapture >> newPic;
				} while (newPic.empty());
			}

			// Kim�soljuk az �j k�pet, ha van
			if (!newPic.empty())
			{
				newPic.copyTo(myPicture);
			}
			
			// Id� friss�t�se:
			lastTime = clock();
		}

		return myPicture;
	}


	// Konstruktorok:
	InputPicture::InputPicture(int CameraID)
	{
		open(CameraID);
	}

	InputPicture::InputPicture(const string File)
	{
		open(File);
	}

	// Eszk�z megnyit�sa f�ggv�ny:
	bool InputPicture::open(int CameraID)
	{
		inputType = CAMERA;
		if (!inputCapture.isOpened())
		{
			if (inputCapture.open(CameraID))
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return true;
		}
	}

	bool InputPicture::openVideo(const string File)
	{
		inputType = VIDEO;
		if (!inputCapture.isOpened())
		{
			if (inputCapture.open(File))
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return true;
		}
	}
	bool InputPicture::open(const string File)
	{
		inputType = FILE;
		if (!(myPicture = imread(File, CV_LOAD_IMAGE_COLOR)).data)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	// Meg van-e nyitva visszaad�sa:
	bool InputPicture::isOpened()
	{
		return inputCapture.isOpened();
	}

}
