#ifndef __ROBOT_HANDLER_MODBUS_H__
#define __ROBOT_HANDLER_MODBUS_H__

// Robot modbus kezel�se interf�sz: blokkol, k�l�n threadben futtatni �s megv�rni

C_BOOL connectRobotModbus(void);                                 // Csatlakoz�s a robothoz modbuszon
void disconnectRobotModbus(void);                              // Lecsatlakoz�s a robotr�l modbuszon
int  moveRobotModbus(char src[3], char dst[3], char hit[4]);   // L�p�s request k�ld�se a robotnak modbuszon, a Java-C++ sakkprotokollban adva
// ^^Ez blokkol� !!!!!

#endif // __ROBOT_HANDLER_MODBUS_H__