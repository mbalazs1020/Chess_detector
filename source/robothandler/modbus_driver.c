#include "stdio.h"
#include "stdlib.h"
#include "types.h"
#include "consts.h"
#include "modbus/modbus.h"

// Globális változók
modbus_t *plc = NULL;

// Csatlakozunk
C_BOOL robot_connect(char * ip, int port)
{
	int i = 0;
	plc = modbus_new_tcp(ip, port);

	if(modbus_connect(plc) == -1)
	{
		return C_FALSE;
	}

	modbus_write_register(plc, ADDR_CMD_1, (REG16) 0 );
	for(i = 0; i < 1000; i++);
	modbus_write_register(plc, ADDR_CMD_2, (REG16) 0 );
	for(i = 0; i < 1000; i++);
	modbus_write_register(plc, ADDR_MOVESOURCE, (REG16) 0 );
	for(i = 0; i < 1000; i++);
	modbus_write_register(plc, ADDR_MOVEDEST, (REG16) 0 );
	for(i = 0; i < 1000; i++);
	modbus_write_register(plc, ADDR_MOVETYPE, (REG16) 0 );
	for(i = 0; i < 1000; i++);
	modbus_write_register(plc, ADDR_CMD_EXIT, (REG16) 0 );
	for(i = 0; i < 1000; i++);

	return C_TRUE;
}


// Lekapcsolódunk
void robot_disconnect(void)
{
	modbus_close(plc);
	modbus_free(plc);
}


// Kiküldjük a parancsot
C_BOOL robot_sendCmd(CMD cmd)
{
	int ret = 0;

	// Ha CMD_CLR_READY-t küldök
	if( cmd == CMD_CLR_READY )
	{
		ret = modbus_write_register(plc, ADDR_CMD_1, (REG16) cmd );
	}

	// Ha exit-et küldök
	else if( cmd == CMD_EXIT )
	{
		ret = modbus_write_register(plc, ADDR_CMD_EXIT, (REG16) cmd );
	}

	// Ha allow1-et küldök, az elsõbe
	else if( cmd == CMD_ALLOW_MOVE_1 )
	{
		ret = modbus_write_register(plc, ADDR_CMD_1, (REG16) cmd );
	}

	// Ha allow2-t, akkor a másodikba
	else if( cmd == CMD_ALLOW_MOVE_2 )
	{
		ret = modbus_write_register(plc, ADDR_CMD_2, (REG16) cmd );
	}
	else
	{
		printf("Invalid command");
		return C_FALSE;
	}

	if(ret == -1)
	{
		return C_FALSE;
	}

	return C_TRUE;
}

// Kiküldjük a lépés pozíciót a robotnak
C_BOOL robot_sendMove(MOVE move)
{
	int i = 0;
	int ret = 0;
	REG16 ans = 0;
	// Ebbe a tömbbe összeszedem az adat minden részét
	REG16 regTemp[3] = {0, 0, 0};

	regTemp[0] = (REG16) move.source;
	regTemp[1] = (REG16) move.dest;
	regTemp[2] = (REG16) move.type;


	// Elsõ engedélyezõ parancs
	robot_sendCmd(CMD_ALLOW_MOVE_1);
	for(i = 0; i < 1000; i++);

	// Elküldöm a lépést
	ret = modbus_write_registers(plc, ADDR_MOVESOURCE, 3, regTemp);
	for(i = 0; i < 1000; i++);

	if (ret == -1)
	{
		return C_FALSE;
	}

	// Utolsó engedélyezõ parancs
	robot_sendCmd(CMD_ALLOW_MOVE_2);
	for(i = 0; i < 1000; i++);

	printf("Waiting for answer... \n");

	// Várom a választ
	while (ans != CMD_READY)
	{
		ret = modbus_read_registers(plc, 1040, 1, &ans);
		for(i = 0; i < 1000; i++);
		if(ret == -1)
		{
			return C_FALSE;
		}
	}

	printf("Move ready. \n");

	// Memóriák törlése
	modbus_write_register(plc, ADDR_CMD_1, (REG16) 0 );
	for(i = 0; i < 1000; i++);
	modbus_write_register(plc, ADDR_CMD_2, (REG16) 0 );
	for(i = 0; i < 1000; i++);

	// Megkérem, hogy törölje a kész jelzést
	robot_sendCmd(CMD_CLR_READY);

	return C_TRUE;
}
