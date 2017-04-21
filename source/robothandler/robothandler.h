#ifndef __ROBOT_HANDLER_MODBUS_H__
#define __ROBOT_HANDLER_MODBUS_H__

// Robot modbus kezelése interfész: blokkol, külön threadben futtatni és megvárni

C_BOOL connectRobotModbus(void);                                 // Csatlakozás a robothoz modbuszon
void disconnectRobotModbus(void);                              // Lecsatlakozás a robotról modbuszon
int  moveRobotModbus(char src[3], char dst[3], char hit[4]);   // Lépés request küldése a robotnak modbuszon, a Java-C++ sakkprotokollban adva
// ^^Ez blokkoló !!!!!

#endif // __ROBOT_HANDLER_MODBUS_H__