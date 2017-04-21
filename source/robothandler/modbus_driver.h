#ifndef __MODBUS_DRIVER_H__
#define __MODBUS_DRIVER_H__

// Csatlakozunk
C_BOOL robot_connect(char * ip, int port);

// Lekapcsolódunk
void robot_disconnect(void);

// Kiküldjük a parancsot
C_BOOL robot_sendCmd(CMD cmd);

// Lépés kiküldés és várás
C_BOOL robot_sendMove(MOVE move);


#endif
