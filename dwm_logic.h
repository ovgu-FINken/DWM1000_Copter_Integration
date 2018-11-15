#ifndef __dwm_logic_h
#define __dwm_logic_h
#include "mbed.h"
#include "ranging.h"
extern "C" {
#include "libdw1000.h"
#include "circular_buffer.h"
}


#define IRQ_CHECKER_INTERVALL 100
#define IRQ_CHECKER_THRESHOLD 3
#ifndef ADDR
#define ADDR 2
#endif

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
