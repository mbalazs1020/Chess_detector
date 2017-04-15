#ifndef __CHESS_MESSAGE_H__
#define __CHESS_MESSAGE_H__

using namespace std;

// Saj�t IP c�mem att�l f�gg�en, hogy hol vagyok
#define HOME_IP_BUDAPEST (  "192.168.1.68"  )
#define HOME_IP_SZOLNOK  (  "192.168.0.101" )

#define HOME_IP  HOME_IP_SZOLNOK // TODO kitenni majd konstansokba

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

		// Ezzel k�ldheti el a l�p�st, �s majd �n eld�nt�m, hogy milyet
		void sendMoveToJavaChessCore( char* movStart, char* movEnd );

	private: // Ezek m�r csak bel�lr�l h�vhatom

		// ACK �zenet k�ld�se
		void sendAck( char* movStart, char* movEnd );

		// REQ k�ld�se
		void sendReq(char* movStart, char* movEnd);

		// �zenet k�ld�se alacsony szinten
		bool sendMsg();


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

		// Bufferbe teszem a k�ld�ttet, nehogy szinkron probl�m�k legyenek, majd a thread-b�l k�ld�m ki
		char arrayToSend[30];
		int sendLength = 0;
		bool sendAwaiting = false;
		void sendSync(char* msg, int len);

		// Ebbe gy�jt�m ami bej�tt
		int accumulatedReceiveSize = 0;
		char buf[30];

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