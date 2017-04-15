#include <iostream>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <winsock2.h>
#include <fcntl.h>
#include <string>
#include "ChessMessage.h"

using namespace std;

namespace chess_message
{
	// ChessMessage konstruktor: csatlakozás a hálózaton
	ChessMessage::ChessMessage(){}

	bool ChessMessage::connectToJavaChessCore()
	{
		u_long nonblock = 1;

		printf("\nInitialising Winsock...");
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			printf("Failed. Error Code : %d", WSAGetLastError());
			return false;
		}

		printf("Initialised.\n");

		//Create a socket
		if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
		{
			printf("Could not create socket : %d", WSAGetLastError());
			return false;
		}

		printf("Socket created.\n");


		server.sin_addr.s_addr = inet_addr(HOME_IP);
		server.sin_family = AF_INET;
		server.sin_port = htons(50003);

		//Connect to remote server
		if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0)
		{
			puts("connect error");
			return false;
		}

		puts("Connected");

		/* Set socket to non-blocking */
		if (ioctlsocket(s, FIONBIO, &nonblock) < 0)
		{
			cout << "NEM SIKERÜLT A NONBLOKKOLÁS";
			return false;
		}

		isConnectedToJava = true;
		return true;
	}

	bool ChessMessage::ChessCoreMessageCycle()
	{			
		if (sendAwaiting) // Küldés
		{
			return sendMsg();
		}
		else // Fogadás
		{
			recv_size = recv(s, server_reply, 2000, 0);
			if (recv_size == SOCKET_ERROR)
			{
				//puts("recv failed"); // TODO valahogy szét kellene választani, hogy ez hiba, vagy csak nonblocking kilépés. De hogy?
				//return false;
				return true;
			}
			else if (recv_size == WSAENOTCONN)
			{
				puts("Connection closed.");
				return false;
			}
			else
			{
				cout << "____________NYERS UZENET JOTT::: ";
				for (int i = 0; i < recv_size; i++)
				{
					cout << server_reply[i];
				}
				cout << endl;

				// Összerakom az üzenetet
				memcpy(&buf[accumulatedReceiveSize], server_reply, recv_size);
				accumulatedReceiveSize += recv_size;

				if (accumulatedReceiveSize >= 23)
				{
					handleIncomingMessage(buf, accumulatedReceiveSize);
					accumulatedReceiveSize = 0;
				}
			}
			return true;
		}
	}

	// Ezzel küldheti el a lépést, és majd én eldöntöm, hogy milyet
	void  ChessMessage::sendMoveToJavaChessCore( char* movStart, char* movEnd )
	{
		if (isRequestInProgress)
			sendAck(movStart, movEnd);
		else
			sendReq(movStart, movEnd);
	}

	// Bejövõ üzenet feldolgozása
	void ChessMessage::handleIncomingMessage(char mess[], int size)
	{
		// START és END legyen benne, ettõl egységes
		if (strncmp(&mess[0], startSequence, 5) != 0) // Start benne van
		{
			cout << "START nincs benne!" << endl;
			return;
		}

		if (strncmp(&mess[20], finishSequence, 3) != 0)
		{
			cout << "END nincs benne!" << endl;
			return;
		}

		// REQ legyen, ACK-t nem fogadom el
		if (strncmp(&mess[6], actionReq, 3) != 0)
		{
			cout << "REQ nincs benne!" << endl;
			return;
		}

		// Kipakolom az adatokat
		memcpy(&moveSource[0], &mess[10], 2);
		memcpy(&moveDest[0], &mess[13], 2);
		memcpy(&hitType[0], &mess[16], 3);


		isRequestInProgress = true; // MEgjegyzem, hogy még vissza kell igazolni

		// TODO lépést megtenni Visszaküldés most átmenetileg
		//sendAck(moveSource, moveDest);
		cout << "MSG ACKED" << endl;

		//-----------------------------------------------
		/*
		// Elteszem magamnak
		memcpy(&arrayReceivedChars[numberOfReceivedChars], &mess[0], size);
		numberOfReceivedChars += size;

		// Megnézem, van-e már valami feldolgozni való
		for (int numberOfTries = 0; ((numberOfProcessedChars < numberOfReceivedChars) && numberOfTries < 10 ); numberOfTries++) // Végigmegyek a fogadott karaktereken, nem ragadok be, tízszer próbálkozhatok
		{
			if ( memcmp( &(arrayReceivedChars[numberOfProcessedChars]), startSequence, 5) == 0 ) // Ha a start szekvencia jött be
			{
				cout << "NEW MESSAGE RECEIVED: ";
				startAccepted = true;
				numberOfProcessedChars += 5; // Lépek a kezdõ szekvencia végére
			}
			else if (startAccepted) // Start bejött
			{
				if (strncmp(&arrayReceivedChars[numberOfProcessedChars], " ", 1) == 0) // Szóközöket nem nézem
				{
					cout << " ";
					numberOfProcessedChars++; // Csak szóköz
				}
				else if (!actionAccepted && ( (numberOfReceivedChars - numberOfProcessedChars) > 3 ) ) // Akciót kérdezem 
				{
					memset(&action[0], 0, 4); //(nullázás)
					memcpy(&action[0], &arrayReceivedChars[numberOfProcessedChars], 3); // Kiveszem az akciót
					numberOfProcessedChars += 3;
					if ((memcmp(&action[0], actionReq, 3) == 0) || // Elfogadható dolog jött
						(memcmp(&action[0], actionAck, 3) == 0))
					{
						cout << action;
						actionAccepted = true;
					}
				}
				else if (actionAccepted) // Akció már megvan, lépés elejét keresem
				{
					if (!moveSourceAccepted && ((numberOfReceivedChars - numberOfProcessedChars) > 2)) // lépés kezdetét keresem
					{
						memset(&moveSource[0], 0, 3); //(nullázás)
						memcpy(&moveSource[0], &arrayReceivedChars[numberOfProcessedChars], 2); // Kiveszem a mezõt
						if (validateField(moveSource) )
						{
							numberOfProcessedChars += 2;
							moveSourceAccepted = true;
							cout << moveSource;
						}
					}
					else if (moveSourceAccepted) // Lépés kezdete megvan
					{
						if (!moveDestAccepted && ((numberOfReceivedChars - numberOfProcessedChars) > 2)) // Lépés végét keresem
						{
							memset(&moveDest[0], 0, 3); //(nullázás)
							memcpy(&moveDest[0], &arrayReceivedChars[numberOfProcessedChars], 2); // Kiveszem a mezõt
							if (validateField(moveDest))
							{
								numberOfProcessedChars += 2;
								moveDestAccepted = true;
								cout << moveDest;
							}
						}
						else if (moveDestAccepted)
						{
							if (!hitTypeAccepted && ((numberOfReceivedChars - numberOfProcessedChars) > 3)) // Ütök vagy nem ütök, ezt keresem
							{
								memset(&hitType[0], 0, 4); //(nullázás)
								memcpy(&hitType[0], &arrayReceivedChars[numberOfProcessedChars], 3); // Kiveszem a mezõt
								if ( ( memcmp(&hitType[0], hitTypeHit, 3 )  ) ||
								 	 ( memcmp(&hitType[0], hitTypeNoHit, 3 ) ) )
								{
									numberOfProcessedChars += 3;
									hitTypeAccepted = true;
									cout << hitType;
								}
							}
							else if (hitTypeAccepted)
							{
								if (!endAccepted && ((numberOfReceivedChars - numberOfProcessedChars) > 3)) // Lezáró szekvenciát keresem
								{
									if (memcmp(&(arrayReceivedChars[numberOfProcessedChars]), finishSequence, 3) == 0) // Ha a END szekvencia jött be
									{
										cout << " END" << endl;
										endAccepted = true;
										numberOfProcessedChars += 3; // Lépek a kezdõ szekvencia végére

										if (numberOfProcessedChars == MSG_SIZE)
										{
											cout << "Message is valid." << endl;
											// TODO üzenet feldolgozása:
											   // CSAK REQ lehet, amit kap. Itt bebillentek egy bitet, hogy van-e ACK nélküli  REQ  
											isRequestInProgress = true;
											// Visszaküldés most átmenetileg

											sendAck(moveSource, moveDest);

										//	sendMsg(arrayReceivedChars, numberOfProcessedChars);
											cout << "MSG ACKED" << endl;

											// Új kezdet
											startNewMessageWaiting();
										}
									}
								}
							}
						}
					}
				}
			}
		} */
	}


	// ACK üzenet küldése
	void ChessMessage::sendAck(char* movStart, char* movEnd)
	{
		char toSend[30]; // Ez lesz az üzenet lezáró nulla nélkül
		int len = 0; // Ez meg a hossza

		// Beleírom, hogy START szóköz
		toSend[0] = 'S'; toSend[1] = 'T'; toSend[2] = 'A'; toSend[3] = 'R'; toSend[4] = 'T'; toSend[5] = ' ';
		len += 6;

		// Beleírom, hogy ACK
		toSend[6] = 'A'; toSend[7] = 'C'; toSend[8] = 'K'; toSend[9] = ' ';
		len += 3;

		// Beleírom a lépéseket
		toSend[10] = movStart[0]; toSend[11] = movStart[1]; toSend[12] = ' ';
		toSend[13] = movEnd[0]; toSend[14] = movEnd[1]; toSend[15] = ' ';
		len += 6;

		// Végére írom, hogy END
		toSend[16] = 'E'; toSend[17] = 'N'; toSend[18] = 'D';
		toSend[19] = '\0'; // Meg egy retkes nullát is a végére, hogy boldog legyen
		len += 4;

		isRequestInProgress = false; // Már nincs folyamatban a várakozás a túloldal részérõl

		sendSync(toSend, len);
	}


	// REQ üzenet küldése
	void ChessMessage::sendReq(char* movStart, char* movEnd)
	{
		char toSend[30]; // Ez lesz az üzenet lezáró nulla nélkül
		int len = 0; // Ez meg a hossza

		// Beleírom, hogy START szóköz
		toSend[0] = 'S'; toSend[1] = 'T'; toSend[2] = 'A'; toSend[3] = 'R'; toSend[4] = 'T'; toSend[5] = ' ';
		len += 6;

		// Beleírom, hogy ACK
		toSend[6] = 'R'; toSend[7] = 'E'; toSend[8] = 'Q'; toSend[9] = ' ';
		len += 3;

		// Beleírom a lépéseket
		toSend[10] = movStart[0]; toSend[11] = movStart[1]; toSend[12] = ' ';
		toSend[13] = movEnd[0]; toSend[14] = movEnd[1]; toSend[15] = ' ';
		len += 6;

		// Itt nem írom bele, hogy HIT, mert a jávát nem érdekli.

		// Végére írom, hogy END
		toSend[16] = 'E'; toSend[17] = 'N'; toSend[18] = 'D';
		toSend[19] = '\0'; // Meg egy retkes nullát is a végére, hogy boldog legyen
		len += 4;

		sendSync(toSend, len);
	}

	void ChessMessage::sendSync( char* msg, int len )
	{
		memcpy(&arrayToSend, msg, len);
		arrayToSend[len] = '\0';
		sendLength = len;
		sendAwaiting = true;
		cout << endl << "Sending request started..." << endl;
		//sendMsg();
	}


	// Üzenet küldése alacsony szinten
	bool ChessMessage::sendMsg()
	{
		cout << "MESSAGE SENDING: ";
		for (int i = 0; i < sendLength; i++)
		{
			cout << arrayToSend[i];
		}
		cout << endl;

		arrayToSend[sendLength] = '\n'; // Sorvéget ráillesztem, mert a Javás fogadó azt szereti

		if (send(s, arrayToSend, (sendLength + 1), 0) < 0)
		{
			puts("Send failed");
			return false;
		}
		sendAwaiting = false;
		cout << "SENDING HAPPENED FOR REAL" << endl << endl;
		return true;
	}


	bool ChessMessage::validateField(char field[2])
	{
		if ( (field[0] == 'A') ||
			(field[0] == 'B') || 
			(field[0] == 'C') || 
			(field[0] == 'D') || 
			(field[0] == 'E') || 
			(field[0] == 'F') || 
			(field[0] == 'G') || 
			(field[0] == 'H')  )
		{
			if ((field[1] == '1') ||
				(field[1] == '2') ||
				(field[1] == '3') ||
				(field[1] == '4') ||
				(field[1] == '5') ||
				(field[1] == '6') ||
				(field[1] == '7') ||
				(field[1] == '8')  )
			{
				return true;
			}
		}

		return false;
	}

	/*void ChessMessage::startNewMessageWaiting()
	{
		numberOfReceivedChars = 0;
		numberOfProcessedChars = 0;

		// Üzenet egyes részeinek jó megérkezését jelzõ flagek
		startAccepted = false;
		actionAccepted = false;
		moveSourceAccepted = false;
		moveDestAccepted = false;
		hitTypeAccepted = false;
		endAccepted = false;
	} */

}