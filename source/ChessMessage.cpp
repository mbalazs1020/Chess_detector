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
	// ChessMessage konstruktor: csatlakoz�s a h�l�zaton
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
			cout << "NEM SIKER�LT A NONBLOKKOL�S";
			return false;
		}

		isConnectedToJava = true;
		return true;
	}

	bool ChessMessage::ChessCoreMessageCycle()
	{			
		if (sendAwaiting) // K�ld�s
		{
			return sendMsg();
		}
		else // Fogad�s
		{
			recv_size = recv(s, server_reply, 2000, 0);
			if (recv_size == SOCKET_ERROR)
			{
				//puts("recv failed"); // TODO valahogy sz�t kellene v�lasztani, hogy ez hiba, vagy csak nonblocking kil�p�s. De hogy?
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

				// �sszerakom az �zenetet
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

	// Ezzel k�ldheti el a l�p�st, �s majd �n eld�nt�m, hogy milyet
	void  ChessMessage::sendMoveToJavaChessCore( char* movStart, char* movEnd )
	{
		if (isRequestInProgress)
			sendAck(movStart, movEnd);
		else
			sendReq(movStart, movEnd);
	}

	// Bej�v� �zenet feldolgoz�sa
	void ChessMessage::handleIncomingMessage(char mess[], int size)
	{
		// START �s END legyen benne, ett�l egys�ges
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


		isRequestInProgress = true; // MEgjegyzem, hogy m�g vissza kell igazolni

		// TODO l�p�st megtenni Visszak�ld�s most �tmenetileg
		//sendAck(moveSource, moveDest);
		cout << "MSG ACKED" << endl;

		//-----------------------------------------------
		/*
		// Elteszem magamnak
		memcpy(&arrayReceivedChars[numberOfReceivedChars], &mess[0], size);
		numberOfReceivedChars += size;

		// Megn�zem, van-e m�r valami feldolgozni val�
		for (int numberOfTries = 0; ((numberOfProcessedChars < numberOfReceivedChars) && numberOfTries < 10 ); numberOfTries++) // V�gigmegyek a fogadott karaktereken, nem ragadok be, t�zszer pr�b�lkozhatok
		{
			if ( memcmp( &(arrayReceivedChars[numberOfProcessedChars]), startSequence, 5) == 0 ) // Ha a start szekvencia j�tt be
			{
				cout << "NEW MESSAGE RECEIVED: ";
				startAccepted = true;
				numberOfProcessedChars += 5; // L�pek a kezd� szekvencia v�g�re
			}
			else if (startAccepted) // Start bej�tt
			{
				if (strncmp(&arrayReceivedChars[numberOfProcessedChars], " ", 1) == 0) // Sz�k�z�ket nem n�zem
				{
					cout << " ";
					numberOfProcessedChars++; // Csak sz�k�z
				}
				else if (!actionAccepted && ( (numberOfReceivedChars - numberOfProcessedChars) > 3 ) ) // Akci�t k�rdezem 
				{
					memset(&action[0], 0, 4); //(null�z�s)
					memcpy(&action[0], &arrayReceivedChars[numberOfProcessedChars], 3); // Kiveszem az akci�t
					numberOfProcessedChars += 3;
					if ((memcmp(&action[0], actionReq, 3) == 0) || // Elfogadhat� dolog j�tt
						(memcmp(&action[0], actionAck, 3) == 0))
					{
						cout << action;
						actionAccepted = true;
					}
				}
				else if (actionAccepted) // Akci� m�r megvan, l�p�s elej�t keresem
				{
					if (!moveSourceAccepted && ((numberOfReceivedChars - numberOfProcessedChars) > 2)) // l�p�s kezdet�t keresem
					{
						memset(&moveSource[0], 0, 3); //(null�z�s)
						memcpy(&moveSource[0], &arrayReceivedChars[numberOfProcessedChars], 2); // Kiveszem a mez�t
						if (validateField(moveSource) )
						{
							numberOfProcessedChars += 2;
							moveSourceAccepted = true;
							cout << moveSource;
						}
					}
					else if (moveSourceAccepted) // L�p�s kezdete megvan
					{
						if (!moveDestAccepted && ((numberOfReceivedChars - numberOfProcessedChars) > 2)) // L�p�s v�g�t keresem
						{
							memset(&moveDest[0], 0, 3); //(null�z�s)
							memcpy(&moveDest[0], &arrayReceivedChars[numberOfProcessedChars], 2); // Kiveszem a mez�t
							if (validateField(moveDest))
							{
								numberOfProcessedChars += 2;
								moveDestAccepted = true;
								cout << moveDest;
							}
						}
						else if (moveDestAccepted)
						{
							if (!hitTypeAccepted && ((numberOfReceivedChars - numberOfProcessedChars) > 3)) // �t�k vagy nem �t�k, ezt keresem
							{
								memset(&hitType[0], 0, 4); //(null�z�s)
								memcpy(&hitType[0], &arrayReceivedChars[numberOfProcessedChars], 3); // Kiveszem a mez�t
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
								if (!endAccepted && ((numberOfReceivedChars - numberOfProcessedChars) > 3)) // Lez�r� szekvenci�t keresem
								{
									if (memcmp(&(arrayReceivedChars[numberOfProcessedChars]), finishSequence, 3) == 0) // Ha a END szekvencia j�tt be
									{
										cout << " END" << endl;
										endAccepted = true;
										numberOfProcessedChars += 3; // L�pek a kezd� szekvencia v�g�re

										if (numberOfProcessedChars == MSG_SIZE)
										{
											cout << "Message is valid." << endl;
											// TODO �zenet feldolgoz�sa:
											   // CSAK REQ lehet, amit kap. Itt bebillentek egy bitet, hogy van-e ACK n�lk�li  REQ  
											isRequestInProgress = true;
											// Visszak�ld�s most �tmenetileg

											sendAck(moveSource, moveDest);

										//	sendMsg(arrayReceivedChars, numberOfProcessedChars);
											cout << "MSG ACKED" << endl;

											// �j kezdet
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


	// ACK �zenet k�ld�se
	void ChessMessage::sendAck(char* movStart, char* movEnd)
	{
		char toSend[30]; // Ez lesz az �zenet lez�r� nulla n�lk�l
		int len = 0; // Ez meg a hossza

		// Bele�rom, hogy START sz�k�z
		toSend[0] = 'S'; toSend[1] = 'T'; toSend[2] = 'A'; toSend[3] = 'R'; toSend[4] = 'T'; toSend[5] = ' ';
		len += 6;

		// Bele�rom, hogy ACK
		toSend[6] = 'A'; toSend[7] = 'C'; toSend[8] = 'K'; toSend[9] = ' ';
		len += 3;

		// Bele�rom a l�p�seket
		toSend[10] = movStart[0]; toSend[11] = movStart[1]; toSend[12] = ' ';
		toSend[13] = movEnd[0]; toSend[14] = movEnd[1]; toSend[15] = ' ';
		len += 6;

		// V�g�re �rom, hogy END
		toSend[16] = 'E'; toSend[17] = 'N'; toSend[18] = 'D';
		toSend[19] = '\0'; // Meg egy retkes null�t is a v�g�re, hogy boldog legyen
		len += 4;

		isRequestInProgress = false; // M�r nincs folyamatban a v�rakoz�s a t�loldal r�sz�r�l

		sendSync(toSend, len);
	}


	// REQ �zenet k�ld�se
	void ChessMessage::sendReq(char* movStart, char* movEnd)
	{
		char toSend[30]; // Ez lesz az �zenet lez�r� nulla n�lk�l
		int len = 0; // Ez meg a hossza

		// Bele�rom, hogy START sz�k�z
		toSend[0] = 'S'; toSend[1] = 'T'; toSend[2] = 'A'; toSend[3] = 'R'; toSend[4] = 'T'; toSend[5] = ' ';
		len += 6;

		// Bele�rom, hogy ACK
		toSend[6] = 'R'; toSend[7] = 'E'; toSend[8] = 'Q'; toSend[9] = ' ';
		len += 3;

		// Bele�rom a l�p�seket
		toSend[10] = movStart[0]; toSend[11] = movStart[1]; toSend[12] = ' ';
		toSend[13] = movEnd[0]; toSend[14] = movEnd[1]; toSend[15] = ' ';
		len += 6;

		// Itt nem �rom bele, hogy HIT, mert a j�v�t nem �rdekli.

		// V�g�re �rom, hogy END
		toSend[16] = 'E'; toSend[17] = 'N'; toSend[18] = 'D';
		toSend[19] = '\0'; // Meg egy retkes null�t is a v�g�re, hogy boldog legyen
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


	// �zenet k�ld�se alacsony szinten
	bool ChessMessage::sendMsg()
	{
		cout << "MESSAGE SENDING: ";
		for (int i = 0; i < sendLength; i++)
		{
			cout << arrayToSend[i];
		}
		cout << endl;

		arrayToSend[sendLength] = '\n'; // Sorv�get r�illesztem, mert a Jav�s fogad� azt szereti

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

		// �zenet egyes r�szeinek j� meg�rkez�s�t jelz� flagek
		startAccepted = false;
		actionAccepted = false;
		moveSourceAccepted = false;
		moveDestAccepted = false;
		hitTypeAccepted = false;
		endAccepted = false;
	} */

}