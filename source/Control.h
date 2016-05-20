#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "PictureConsts.h"

// Haszn�land� n�vterek
using namespace cv;
using namespace std;
using namespace input_picture;
using namespace field;
using namespace chessboard;
using namespace gamestate;


namespace control
{
	// Control oszt�ly: program fut�s�nak a gerince
	class Control
	{
	private:
		typedef enum
		{
			CAMERA,
			VIDEO
		} INPUT_TYPE;

		// M�k�d�s �llapotai:
		typedef enum
		{
			CHECK_ORIENTATION,
			SET_UP_CHESSBOARD,
			CALIBRATION,
			CHECK_SETUP,
			RUN,
			EXIT
		} CONTROL_SM;

		// Aktu�lis �llapot:
		CONTROL_SM State = SET_UP_CHESSBOARD;
		INPUT_TYPE InputType = CAMERA;

		// Id�z�t�k:
		time_t LastSampleTime = clock();
		time_t ActualTime = clock();

		// Bemenet:
		InputPicture inputPic;

		// Aktu�lis k�p:
		Mat RawPicture;
		Mat ChessboardPicture;

		// Aktu�lis GUI
		Mat gameGUI;

		// J�t�k �ll�sa:
		GameState gameState;

		// Id�z�t� f�ggv�nye:
		bool NewSampleAllowed(void);

		// M�k�d�s f�ggv�nyei:
		bool checkOrientation(void);
		bool setUpChessBoard(void);
		bool calibrateDetection(void);
		bool run(void);

	public:
		// Konstruktor:
		Control();                      // Kamer�val
		Control( const string File );   // Vide�val

		// M�k�dtet� f�ggv�ny:
		bool runMainCycle(void);

		// K�pet visszaad� f�ggv�ny:
		cv::Mat getPicture() { return ChessboardPicture; }

	};

}


#endif /* _CONTROL_H_ */