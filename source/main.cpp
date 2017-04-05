// Általános inklúdok
#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <winsock2.h>
#include <stdio.h>
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
#include "GameState.h"
#include "Control.h"
#include "ChessMessage.h"

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

void ChessMessageMainCycleThread()
{
	//myChessMessageController.ChessCoreMessageCycle();
	cout << "A thread elindult" << endl;
}

// Main függvény
int main( void )
{
	bool isStillRunning = true;  // Még fut-e a fõ ciklus, nem volt-e hiba
	Control        myChessController(VIDEO_1);          // Játékfigyelés irányító osztály létrehozása
	
	// Futás végtelen ciklusa: ameddig valami hibával lép vissza
	while (isStillRunning)
	{
		isStillRunning = myChessController.runMainCycle(); // Képfeldolgozás fusson

		if (myChessController.isGameDetectionRunning()) // Ha már lement a kalibráció meg ilyenek, és rendes futás van, akkor a TCP kapcsolat is mehet
		{
			if (!myChessMessageController.isConnectedToJavaChessCore()) // Ha még nem kapcsolódtam
			{
				isStillRunning = myChessMessageController.connectToJavaChessCore();  // Kapcsolódok
				thread thr(ChessMessageMainCycleThread); // Threadet megnyitom, ahol a TCP fogadás megy
			}
		}
	}
	
	return 0;
}
