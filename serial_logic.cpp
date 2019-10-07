#include "serial_logic.h"
#include "leds.h"

circularBuffer UARTcb;
uint8_t UARTcb_data[256];

#if SWITCH_UART &&  MOVE_MOTOR == false
RawSerial uart1(SERIAL_TX, SERIAL_RX, TELEMETRY_BAUD);
Serial uart2(PC_6, PC_7, DEBUG_BAUD);
#elif !SWITCH_UART && MOVE_MOTOR == false
RawSerial uart1(PC_6, PC_7, TELEMETRY_BAUD);
Serial uart2(SERIAL_TX, SERIAL_RX, DEBUG_BAUD);
#else
Serial uart2(PC_6, PC_7, DEBUG_BAUD);
#endif

void serialRead() {
    // this should collect the packets to be sent, and if an packet is complets, it should be sent ... wow ...
#if MOVE_MOTOR == false
    greenLed = 0;
    while(uart1.readable()) {
        char c = uart1.getc();
#if ECHO == 1
        uart1.putc(c);
#endif
        circularBuffer_write_element(&UARTcb, c);
    }
    greenLed = 1;
#endif
}

void initialseSerial() {
    circularBuffer_init(&UARTcb, UARTcb_data, 256);
#if MOVE_MOTOR == false
    uart1.attach(&serialRead,Serial::RxIrq);
#endif
}
void sendUART(uint8_t* data, int length) {
#if MOVE_MOTOR == false
    for(uint8_t i = 0; i<length; i++) {
        uart1.putc(data[i]);
    }
#endif
  // dummy
}
