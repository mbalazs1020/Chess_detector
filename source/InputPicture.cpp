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
#include "InputPicture.h"


namespace input_picture
{

	// Kép visszadása:
	cv::Mat InputPicture::getPicture()
	{
		// Új kép tároló
		cv::Mat newPic;

		// Kamera esetén egyszerû a dolog
		if ((inputType == CAMERA) )
		{
			// Addig olvasom, ameddig jön új!!!
			do
			{
				inputCapture >> newPic;
			} while (newPic.empty());
			// Ez lesz az új képem:
			newPic.copyTo(myPicture);
		}
		// Videó esetén úgy kérjük ki a képet, hogy az FPS-hez igazodjon
		if (inputType == VIDEO)
		{
			// Megnézzük, mennyi idõ telt el:
			time_t timePassed = clock() - lastTime;
			int NumberOfPastFrames = (int)timePassed / TIME_BW_FRAMES_MS;


			// Kérünk új képet annyiszor, amennyi idõ eltelt azóta a videóból
			for (int i = 0; i < NumberOfPastFrames; i++)
			{
				do
				{
					inputCapture >> newPic;
				} while (newPic.empty());
			}

			// Kimásoljuk az új képet, ha van
			if (!newPic.empty())
			{
				newPic.copyTo(myPicture);
			}
			
			// Idõ frissítése:
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

	// Eszköz megnyitása függvény:
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

	// Meg van-e nyitva visszaadása:
	bool InputPicture::isOpened()
	{
		return inputCapture.isOpened();
	}

}
