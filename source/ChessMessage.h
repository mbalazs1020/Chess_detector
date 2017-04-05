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

	// Sakkjáték üzenet elõállító, feldolgozó
	class ChessMessage
	{
	//========================= Publikus tartalom ========================//
	public:
		// Konstruktor
		ChessMessage();

		// Kapcsolódás
		bool connectToJavaChessCore();

		// Ütemezõ: Ezt hívjuk a round robin-ban
		bool ChessCoreMessageCycle();

		// Kapcsolódás megvolt-e
		bool isConnectedToJavaChessCore(){ return isConnectedToJava; }

		// ACK üzenet küldése
		void sendAck( char* movStart, char* movEnd );

		// REQ küldése
		void sendReq(char* movStart, char* movEnd);

		// Üzenet küldése alacsony szinten
		void sendMsg(char* mess, int len);


	//========================= Privát tartalom   ========================//
	private:
		// Kapcsolódtunk már?
		bool isConnectedToJava = false;

		// Ez a flag mutatja, hogy az elküldött üzenet REQ vagy ACK lesz: ha REQ jött, akkor ACK-t küldünk, egyébként REQ-t.
		bool isRequestInProgress = false;

		// Hálózat kezelése
		WSADATA wsa;
		SOCKET s;
		struct sockaddr_in server;
		char server_reply[2000];
		int recv_size = 2000;
		string str;


		char arrayReceivedChars[30]; // Ebbe gyûjtöm ami bejött
		int numberOfReceivedChars = 0;
		int numberOfProcessedChars = 0;

		// Üzenet egyes részeinek jó megérkezését jelzõ flagek
		bool startAccepted  = false;
		bool actionAccepted = false;
		bool moveSourceAccepted = false;
		bool moveDestAccepted = false;
		bool hitTypeAccepted = false;
		bool endAccepted = false;

		// Az üzenetbõl kipakolt részek
		char action[4];
		char moveSource[3];
		char moveDest[3];
		char hitType[4];


		// Új üzenet jöhet, mert az elõzõ fogadását befejeztem
		void startNewMessageWaiting();

		// Az üzenetben szereplõ táblamezõ ellenõrzése
		bool validateField(char field[2]);

		// Beérkezõ üzenet feldolgozása
		void handleIncomingMessage(char mess[], int size); 

	};
}

 


#endif // __CHESS_MESSAGE_H__