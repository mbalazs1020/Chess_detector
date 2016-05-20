#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "PictureConsts.h"

// Használandó névterek
using namespace cv;
using namespace std;
using namespace input_picture;
using namespace field;
using namespace chessboard;
using namespace gamestate;


namespace control
{
	// Control osztály: program futásának a gerince
	class Control
	{
	private:
		typedef enum
		{
			CAMERA,
			VIDEO
		} INPUT_TYPE;

		// Mûködés állapotai:
		typedef enum
		{
			CHECK_ORIENTATION,
			SET_UP_CHESSBOARD,
			CALIBRATION,
			CHECK_SETUP,
			RUN,
			EXIT
		} CONTROL_SM;

		// Aktuális állapot:
		CONTROL_SM State = SET_UP_CHESSBOARD;
		INPUT_TYPE InputType = CAMERA;

		// Idõzítõk:
		time_t LastSampleTime = clock();
		time_t ActualTime = clock();

		// Bemenet:
		InputPicture inputPic;

		// Aktuális kép:
		Mat RawPicture;
		Mat ChessboardPicture;

		// Aktuális GUI
		Mat gameGUI;

		// Játék állása:
		GameState gameState;

		// Idõzítõ függvénye:
		bool NewSampleAllowed(void);

		// Mûködés függvényei:
		bool checkOrientation(void);
		bool setUpChessBoard(void);
		bool calibrateDetection(void);
		bool run(void);

	public:
		// Konstruktor:
		Control();                      // Kamerával
		Control( const string File );   // Videóval

		// Mûködtetõ függvény:
		bool runMainCycle(void);

		// Képet visszaadó függvény:
		cv::Mat getPicture() { return ChessboardPicture; }

	};

}


#endif /* _CONTROL_H_ */