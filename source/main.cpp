// Általános inklúdok
#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>
#include <thread>

// OpenCV inklúdok
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

// Saját inklúdok
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
using namespace chess_message;


// Globális változók
ChessMessage   myChessMessageController;          // Java sakkjátékkal kapcsolatot tartó osztályom, globális, hogy a thread is lássa
bool tcpRunning = true;

// Ebben fut a TCP fogadás
void ChessMessageMainCycleThread()
{
	cout << "A TCP thread elindult" << endl;
	long time = clock();
	while (tcpRunning) // Végtelen
	{
		if ((clock() - time) > 10) // 10 ms-enként fogadok, legyen idõ feldolgozni
		{
			tcpRunning = myChessMessageController.ChessCoreMessageCycle();
			time = clock();
		}
	}
}

// Main függvény
int main( void )
{
	bool isStillRunning = true;  // Még fut-e a fõ ciklus, nem volt-e hiba
	//Control        myChessController(VIDEO_1);          // Játékfigyelés irányító osztály létrehozása
	Control        myChessController;          // Játékfigyelés irányító osztály létrehozása

	// PLC csatlakozás
#ifdef PLC_IS_ON
	cout << "PLC kapcsolódás bekapcsolva." << endl;
	isStillRunning = connectRobotModbus() ? true : false;
#endif
	
	// Futás végtelen ciklusa: ameddig valami hibával lép vissza
	while (isStillRunning)
	{
		isStillRunning = myChessController.runMainCycle(); // Képfeldolgozás fusson

		if (myChessController.isGameDetectionRunning()) // Ha már lement a kalibráció meg ilyenek, és rendes futás van, akkor a TCP kapcsolat is mehet
		{
			if (!myChessMessageController.isConnectedToJavaChessCore()) // Ha még nem kapcsolódtam
			{
				// Kapcsolódok
				isStillRunning = myChessMessageController.connectToJavaChessCore();
				if (isStillRunning)
				{
					// Threadet megnyitom, ahol a TCP fogadás megy
					thread thr(ChessMessageMainCycleThread);
					thr.detach(); // Démomba teszem, menjen külön a többitõl.

					// Ledobom a referenciát a képfelismerõnek, hogy küldhessen
					myChessController.setJavaCoreReference(&myChessMessageController);
				}
			}
		}

		if (!tcpRunning) // Ha a TCP-ben hiba volt, azonnal kiléptetek mindent
			return -1;
	}
	
	return 0;
}
