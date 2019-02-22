#include "mbed.h"
#include "node_config.h"
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
        case MLAT_BASE_ADDR + 6:
            mlat_range_target = 1;
            break;
        default:
            mlat_range_target++;
    }
    greenLed = 1;
}

int main() {
    resetRangeVariables();
    initialseSerial();
    initialiseMlat();
    t_irq.start(callback(&IRQqueue, &EventQueue::dispatch_forever));
    t_irq.set_priority(osPriorityHigh);
    initialiseDWM();
    attach_message_handlers();
    startDWM();
    uart2.printf("Start Ranging\n");

    uint8_t WriteBuffer[256+4];
#if (ADDR != 1) && (ADDR < MLAT_BASE_ADDR)
    IRQqueue.call_every(RANGE_INTERVALL_US, startRanging);
    IRQqueue.call_every(1000, send_position);
#endif
    while (true){
        greenLed = 1;
#if MLAT_ACTIVE
#if ADDR < MLAT_BASE_ADDR
        mlat.iterative_step();
        Thread::yield();
#endif
#endif

        uint8_t l = parsePPRZ(&UARTcb);
        if(l){
            if(get_pkg_msg_id(&UARTcb) == PPRZ_ALIVE_MSG_ID) {
              node_address = get_pkg_sender_id(&UARTcb);
            }
            WriteBuffer[0] = node_address;
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
