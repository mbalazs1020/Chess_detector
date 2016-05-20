#ifndef _INPUT_PICTURE_H_
#define _INPUT_PICTURE_H_

#include "PictureConsts.h"

// OpenCV és standard névterek:
using namespace cv;
using namespace std;

namespace input_picture           // Input kép névtere
{

	// Input kép osztálya --------------------------------------------
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

		INPUT_TYPE inputType = INIT;      // Bemenet típusa: kamera / kép
		cv::Mat myPicture;                // Aktuális elmentett kép
		cv::VideoCapture inputCapture;    // Kamera képe innen jön
		

	public:
		// Konstruktor:
		InputPicture(){};
		InputPicture(int CameraID);
		InputPicture(const string File);

		// Eszköz megnyitása függvény:
		bool open(int CameraID);
		bool open(const string File);
		bool openVideo(const string File);

		// Meg van-e nyitva visszaadása:
		bool isOpened();

		// Kép visszaadása
		cv::Mat getPicture();

	};


}


#endif // _INPUT_PICTURE_H_