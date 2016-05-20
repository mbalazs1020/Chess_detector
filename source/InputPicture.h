#ifndef _INPUT_PICTURE_H_
#define _INPUT_PICTURE_H_

#include "PictureConsts.h"

// OpenCV �s standard n�vterek:
using namespace cv;
using namespace std;

namespace input_picture           // Input k�p n�vtere
{

	// Input k�p oszt�lya --------------------------------------------
	class InputPicture
	{
	private:
		typedef enum
		{
			FILE,
			CAMERA,
			VIDEO,
			INIT
		} INPUT_TYPE;

		time_t lastTime = clock();

		INPUT_TYPE inputType = INIT;      // Bemenet t�pusa: kamera / k�p
		cv::Mat myPicture;                // Aktu�lis elmentett k�p
		cv::VideoCapture inputCapture;    // Kamera k�pe innen j�n
		

	public:
		// Konstruktor:
		InputPicture(){};
		InputPicture(int CameraID);
		InputPicture(const string File);

		// Eszk�z megnyit�sa f�ggv�ny:
		bool open(int CameraID);
		bool open(const string File);
		bool openVideo(const string File);

		// Meg van-e nyitva visszaad�sa:
		bool isOpened();

		// K�p visszaad�sa
		cv::Mat getPicture();

	};


}


#endif // _INPUT_PICTURE_H_