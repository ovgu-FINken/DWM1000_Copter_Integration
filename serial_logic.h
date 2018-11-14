#ifndef __serial_logic_h
#define __serial_logic_h
#include "mbed.h"
extern "C" {
#include "circular_buffer.h"
}

//#define SWITCH_UART
#define TELEMETRY_BAUD 38400
#define DEBUG_BAUD 115200
#define ECHO 0

extern RawSerial uart1;
extern Serial uart2;

extern circularBuffer UARTcb;
void initialseSerial();
void serialRead();
void sendUART(uint8_t* data, int length);

#endif
