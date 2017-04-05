// �ltal�nos inkl�dok
#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <thread>

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
#include "ChessMessage.h"

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
using namespace chess_message;


// Glob�lis v�ltoz�k
ChessMessage   myChessMessageController;          // Java sakkj�t�kkal kapcsolatot tart� oszt�lyom, glob�lis, hogy a thread is l�ssa

void ChessMessageMainCycleThread()
{
	//myChessMessageController.ChessCoreMessageCycle();
	cout << "A thread elindult" << endl;
}

// Main f�ggv�ny
int main( void )
{
	bool isStillRunning = true;  // M�g fut-e a f� ciklus, nem volt-e hiba
	Control        myChessController(VIDEO_1);          // J�t�kfigyel�s ir�ny�t� oszt�ly l�trehoz�sa
	
	// Fut�s v�gtelen ciklusa: ameddig valami hib�val l�p vissza
	while (isStillRunning)
	{
		isStillRunning = myChessController.runMainCycle(); // K�pfeldolgoz�s fusson

		if (myChessController.isGameDetectionRunning()) // Ha m�r lement a kalibr�ci� meg ilyenek, �s rendes fut�s van, akkor a TCP kapcsolat is mehet
		{
			if (!myChessMessageController.isConnectedToJavaChessCore()) // Ha m�g nem kapcsol�dtam
			{
				isStillRunning = myChessMessageController.connectToJavaChessCore();  // Kapcsol�dok
				thread thr(ChessMessageMainCycleThread); // Threadet megnyitom, ahol a TCP fogad�s megy
			}
		}
	}
	
	return 0;
}
