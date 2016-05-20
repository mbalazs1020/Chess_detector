// Általános inklúdok
#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

// OpenCV inklúdok
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

// Saját inklúdok
#include "InputPicture.h"
#include "Field.h"
#include "Chessboard.h"
#include "GameState.h"
#include "Control.h"

// Értelmetlen errorok kiküszöbölése
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

// Használandó névterek
using namespace cv;
using namespace std;
using namespace input_picture;
using namespace field;
using namespace chessboard;
using namespace gamestate;
using namespace control;


// Main függvény
int main( void )
{
//	InputPicture inputPic(1);                    // Aktuális frame tárolója <----------------------------------IDE ÍRD BE !!!!!!!!!!!!!!!!
//	Mat Picture;                                 // Aktuális kép
	Control myChessController(VIDEO_1);                   // Mintavétel irányító osztály  

	while (myChessController.runMainCycle());    // Futás végtelen ciklusa

	return 0;
}
