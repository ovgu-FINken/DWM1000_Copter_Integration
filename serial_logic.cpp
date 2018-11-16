#include "serial_logic.h"
#include "leds.h"

circularBuffer UARTcb;
uint8_t UARTcb_data[256];

#if SWITCH_UART
RawSerial uart1(PA_9, PA_10, TELEMETRY_BAUD);
Serial uart2(PA_2, PA_3, DEBUG_BAUD);
#else
RawSerial uart1(PA_2, PA_3, TELEMETRY_BAUD);
Serial uart2(PA_9, PA_10, DEBUG_BAUD);
#endif

void serialRead() {
    // this should collect the packets to be sent, and if an packet is complets, it should be sent ... wow ...
    greenLed = 0;
    while(uart1.readable()) {
        char c = uart1.getc();
#if ECHO == 1
        uart1.putc(c);
#endif
        circularBuffer_write_element(&UARTcb, c);
    }
    greenLed = 1;
}

void initialseSerial() {
    circularBuffer_init(&UARTcb, UARTcb_data, 256);
    uart1.attach(&serialRead,Serial::RxIrq);
}
void sendUART(uint8_t* data, int length) {
    for(uint8_t i = 0; i<length; i++) {
        uart1.putc(data[i]);
    }
}
