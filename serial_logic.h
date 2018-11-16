#ifndef __serial_logic_h
#define __serial_logic_h
#include "mbed.h"
#include "node_config.h"
extern "C" {
#include "circular_buffer.h"
}


extern RawSerial uart1;
extern Serial uart2;

extern circularBuffer UARTcb;
void initialseSerial();
void serialRead();
void sendUART(uint8_t* data, int length);

#endif
