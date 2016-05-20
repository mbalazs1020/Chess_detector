// �ltal�nos inkl�dok
#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

// OpenCV inkl�dok
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

// Saj�t inkl�dok
#include "InputPicture.h"
#include "Field.h"
#include "Chessboard.h"
#include "GameState.h"
#include "Control.h"

// �rtelmetlen errorok kik�sz�b�l�se
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

// Haszn�land� n�vterek
using namespace cv;
using namespace std;
using namespace input_picture;
using namespace field;
using namespace chessboard;
using namespace gamestate;
using namespace control;


// Main f�ggv�ny
int main( void )
{
//	InputPicture inputPic(1);                    // Aktu�lis frame t�rol�ja <----------------------------------IDE �RD BE !!!!!!!!!!!!!!!!
//	Mat Picture;                                 // Aktu�lis k�p
	Control myChessController(VIDEO_1);                   // Mintav�tel ir�ny�t� oszt�ly  

	while (myChessController.runMainCycle());    // Fut�s v�gtelen ciklusa

	return 0;
}
