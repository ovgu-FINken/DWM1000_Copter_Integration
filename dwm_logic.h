#ifndef __dwm_logic_h
#define __dwm_logic_h
#include "mbed.h"
#include "ranging.h"
#include "node_config.h"
extern "C" {
#include "libdw1000.h"
#include "circular_buffer.h"
}

extern circularBuffer DWMcb;
extern dwDevice_t* dwm;
extern DFrame txFrame;
extern DFrame rxFrame;
extern volatile bool sending; // this should not be public (todo)
void initialiseDWM();
void startDWM();

void sendDWM(uint8_t* data, int length);
void DWMReceive();

#endif
