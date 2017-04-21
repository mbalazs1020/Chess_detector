#ifndef __MODBUS_DRIVER_H__
#define __MODBUS_DRIVER_H__

// Csatlakozunk
C_BOOL robot_connect(char * ip, int port);

// Lekapcsol�dunk
void robot_disconnect(void);

// Kik�ldj�k a parancsot
C_BOOL robot_sendCmd(CMD cmd);

// L�p�s kik�ld�s �s v�r�s
C_BOOL robot_sendMove(MOVE move);


#endif
