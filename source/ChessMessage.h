#ifndef __CHESS_MESSAGE_H__
#define __CHESS_MESSAGE_H__

using namespace std;

#define HOME_IP  (  "192.168.1.68"  ) // TODO kitenni majd konstansokba

namespace chess_message
{
	const int  MSG_SIZE = 23;
	const char startSequence[6] = "START";
	const char finishSequence[4] = "END";
	const char actionReq[4] = "REQ";
	const char actionAck[4] = "ACK";
	const char hitTypeHit[4] = "HIT";
	const char hitTypeNoHit[4] = "---";

	// Sakkj�t�k �zenet el��ll�t�, feldolgoz�
	class ChessMessage
	{
	//========================= Publikus tartalom ========================//
	public:
		// Konstruktor
		ChessMessage();

		// Kapcsol�d�s
		bool connectToJavaChessCore();

		// �temez�: Ezt h�vjuk a round robin-ban
		bool ChessCoreMessageCycle();

		// Kapcsol�d�s megvolt-e
		bool isConnectedToJavaChessCore(){ return isConnectedToJava; }

		// ACK �zenet k�ld�se
		void sendAck( char* movStart, char* movEnd );

		// REQ k�ld�se
		void sendReq(char* movStart, char* movEnd);

		// �zenet k�ld�se alacsony szinten
		void sendMsg(char* mess, int len);


	//========================= Priv�t tartalom   ========================//
	private:
		// Kapcsol�dtunk m�r?
		bool isConnectedToJava = false;

		// Ez a flag mutatja, hogy az elk�ld�tt �zenet REQ vagy ACK lesz: ha REQ j�tt, akkor ACK-t k�ld�nk, egy�bk�nt REQ-t.
		bool isRequestInProgress = false;

		// H�l�zat kezel�se
		WSADATA wsa;
		SOCKET s;
		struct sockaddr_in server;
		char server_reply[2000];
		int recv_size = 2000;
		string str;


		char arrayReceivedChars[30]; // Ebbe gy�jt�m ami bej�tt
		int numberOfReceivedChars = 0;
		int numberOfProcessedChars = 0;

		// �zenet egyes r�szeinek j� meg�rkez�s�t jelz� flagek
		bool startAccepted  = false;
		bool actionAccepted = false;
		bool moveSourceAccepted = false;
		bool moveDestAccepted = false;
		bool hitTypeAccepted = false;
		bool endAccepted = false;

		// Az �zenetb�l kipakolt r�szek
		char action[4];
		char moveSource[3];
		char moveDest[3];
		char hitType[4];


		// �j �zenet j�het, mert az el�z� fogad�s�t befejeztem
		void startNewMessageWaiting();

		// Az �zenetben szerepl� t�blamez� ellen�rz�se
		bool validateField(char field[2]);

		// Be�rkez� �zenet feldolgoz�sa
		void handleIncomingMessage(char mess[], int size); 

	};
}

 


#endif // __CHESS_MESSAGE_H__