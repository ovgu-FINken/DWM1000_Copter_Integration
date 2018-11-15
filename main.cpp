#include "mbed.h"
#include "rtos.h"
#include "ranging.h"
#include "mlat.h"
#include "serial_logic.h"
#include "dwm_logic.h"
#include "thread_logic.h"
#include "message_handling.h"
#include "leds.h"
#include "pprz.h"
extern "C" {
#include "libdw1000.h"
#include "circular_buffer.h"
}

// ADDR should be same as AC_ID to match telemetry
#define RANGE_INTERVALL_US 100

using namespace Eigen;

uint8_t mlat_range_target = 1;
void startRanging() {
    greenLed = 0;
    rxFrame.src = mlat_range_target;
    send_rp(RANGE_0);
    switch(mlat_range_target) {
        case 1:
            mlat_range_target = MLAT_BASE_ADDR;
            break;
        case MLAT_BASE_ADDR + 5:
            mlat_range_target = 1;
            break;
        default:
            mlat_range_target++;
    }
    greenLed = 1;
}

int main() {
    initialseSerial();
    initialiseMlat();
    t_irq.start(callback(&IRQqueue, &EventQueue::dispatch_forever));
    t_irq.set_priority(osPriorityHigh);
    initialiseDWM();
    dwAttachSentHandler(dwm, txcallback);
    dwAttachReceivedHandler(dwm, rxcallback);
    dwAttachReceiveTimeoutHandler(dwm, failcallback);
    dwAttachReceiveFailedHandler(dwm, failcallback);
    startDWM();
    uart2.printf("Start Ranging\n");

    uint8_t WriteBuffer[256+4];
#if (ADDR != 1) && (ADDR < MLAT_BASE_ADDR)
    IRQqueue.call_every(RANGE_INTERVALL_US, startRanging);
    IRQqueue.call_every(1000, send_position);
#endif
    while (true){
        greenLed = 1;

#if ADDR < MLAT_BASE_ADDR
        mlat.iterative_step();
        Thread::yield();
#endif

        uint8_t l = parsePPRZ(&UARTcb);
        if(l){
            WriteBuffer[0] = ADDR;
            WriteBuffer[1] = 0; // Broacast
            WriteBuffer[2] = DATA_FRAME;
            WriteBuffer[3] = txFrame.seq++;
            circularBuffer_read(&UARTcb, WriteBuffer+4, l);
            sendDWM(WriteBuffer, l+4);
        }
        Thread::yield();
        l = parsePPRZ(&DWMcb);
        if(l){
            circularBuffer_read(&DWMcb, WriteBuffer, l);
            sendUART(WriteBuffer, l);
        }
        Thread::yield();
    }
}
