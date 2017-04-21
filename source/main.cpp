// �ltal�nos inkl�dok
#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <winsock2.h>
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
#include "ChessMessage.h"
#include "GameState.h"
#include "Control.h"

extern "C" 
{
#include "robothandler\\types.h"
#include "robothandler\\consts.h"
#include "robothandler\\modbus_driver.h"
#include "robothandler\\robothandler.h"
}

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
bool tcpRunning = true;

// Ebben fut a TCP fogad�s
void ChessMessageMainCycleThread()
{
	cout << "A TCP thread elindult" << endl;
	long time = clock();
	while (tcpRunning) // V�gtelen
	{
		if ((clock() - time) > 10) // 10 ms-enk�nt fogadok, legyen id� feldolgozni
		{
			tcpRunning = myChessMessageController.ChessCoreMessageCycle();
			time = clock();
		}
	}
}

// Main f�ggv�ny
int main( void )
{
	bool isStillRunning = true;  // M�g fut-e a f� ciklus, nem volt-e hiba
	//Control        myChessController(VIDEO_1);          // J�t�kfigyel�s ir�ny�t� oszt�ly l�trehoz�sa
	Control        myChessController;          // J�t�kfigyel�s ir�ny�t� oszt�ly l�trehoz�sa

	// PLC csatlakoz�s
#ifdef PLC_IS_ON
	cout << "PLC kapcsol�d�s bekapcsolva." << endl;
	isStillRunning = connectRobotModbus() ? true : false;
#endif
	
	// Fut�s v�gtelen ciklusa: ameddig valami hib�val l�p vissza
	while (isStillRunning)
	{
		isStillRunning = myChessController.runMainCycle(); // K�pfeldolgoz�s fusson

		if (myChessController.isGameDetectionRunning()) // Ha m�r lement a kalibr�ci� meg ilyenek, �s rendes fut�s van, akkor a TCP kapcsolat is mehet
		{
			if (!myChessMessageController.isConnectedToJavaChessCore()) // Ha m�g nem kapcsol�dtam
			{
				// Kapcsol�dok
				isStillRunning = myChessMessageController.connectToJavaChessCore();
				if (isStillRunning)
				{
					// Threadet megnyitom, ahol a TCP fogad�s megy
					thread thr(ChessMessageMainCycleThread);
					thr.detach(); // D�momba teszem, menjen k�l�n a t�bbit�l.

					// Ledobom a referenci�t a k�pfelismer�nek, hogy k�ldhessen
					myChessController.setJavaCoreReference(&myChessMessageController);
				}
			}
		}

		if (!tcpRunning) // Ha a TCP-ben hiba volt, azonnal kil�ptetek mindent
			return -1;
	}
	
	return 0;
}
