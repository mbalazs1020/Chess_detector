#include <stdio.h>
#include "string.h"
#include "types.h"
#include "consts.h"
#include "modbus_driver.h"
#include "robothandler.h"

C_BOOL connectRobotModbus(void)
{
	printf("Connecting to robot: \n");

	// Csatlakoz�s
	while (robot_connect(PLC_IP, PORT) == C_FALSE)
	{
		printf("Connection error to PLC! \n");
		return C_FALSE;
	}

	printf("Connection successful. \n");
	return C_TRUE;
}

void disconnectRobotModbus(void)
{

	robot_sendCmd(CMD_EXIT);

	// Kapcsolat bont�s
	robot_disconnect();

	printf("Shutting down robot... \n Disconnecting... \n");

	return;
}


// �tv�lt�s
static int getFieldNumFromChar(char id[3])
{
	int ret = 0;

	switch (id[1])
	{
	case '1':
		ret += (0 * 8);
		break;
	case '2':
		ret += (1 * 8);
		break;
	case '3':
		ret += (2 * 8);
		break;
	case '4':
		ret += (3 * 8);
		break;
	case '5':
		ret += (4 * 8);
		break;
	case '6':
		ret += (5 * 8);
		break;
	case '7':
		ret += (6 * 8);
		break;
	case '8':
		ret += (7 * 8);
		break;
	default:
		printf("GOND VAN AZ �TV�LT�BAN");
		break;
	}

	switch (id[0])
	{
	case 'A':
		ret += 1;
		break;
	case 'B':
		ret += 2;
		break;
	case 'C':
		ret += 3;
		break;
	case 'D':
		ret += 4;
		break;
	case 'E':
		ret += 5;
		break;
	case 'F':
		ret += 6;
		break;
	case 'G':
		ret += 7;
		break;
	case 'H':
		ret += 8;
		break;
	default:
		printf("GOND VAN AZ �TV�LT�BAN");
		break;
	}

	return ret;
}


//  Ezt fogjuk futtatni, ha a robottal l�p�nk.
//  �j threadben, �s meg is v�rjuk
int moveRobotModbus(char src[3], char dst[3], char hit[4] )
{
	int source = 0;
	int dest = 0;
	MOVE move;

	// KOnverter a k�t protokoll k�z�tt
	// A robotnak 1-64-ig adjuk a mez�ket
	// M�sodik karakter sz�mra v�ltva, nyolccal szorozva + els� karakter sz�mra v�ltva
	source = getFieldNumFromChar(src);
	dest = getFieldNumFromChar(dst);
	if (strncmp(hit, "HIT", 3) == 0)
	{
		move.type = HIT_MOVE;
	}
	else
	{
		move.type = NORMAL_MOVE;
	}

	printf("Starting normal move from %d to %d \n", source, dest);
	move.source = source;
	move.dest = dest;

	robot_sendMove(move);

}
