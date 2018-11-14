#include "mbed.h"
#include "rtos.h"
#include "ranging.h"
#include "mlat.h"
#include "serial_logic.h"
#include "dwm_logic.h"
#include "thread_logic.h"
#include "leds.h"
extern "C" {
#include "libdw1000.h"
#include "circular_buffer.h"
#include "pprz.h"
}

// ADDR should be same as AC_ID to match telemetry
#define RANGE_INTERVALL_US 100
#define MLAT_BASE_ADDR 128

using namespace Eigen;
void send_pprz_range_message(uint8_t src, uint8_t dest, double range);
/* variables for ranging*/

dwTime_t tStartRound1;
dwTime_t tStartReply1;
dwTime_t tEndReply1;
dwTime_t tEndRound2;
dwTime_t tStartReply2;
dwTime_t tEndReply2;
dwTime_t tDelay;
uint64_t tRound1;
uint64_t tReply1;
uint64_t tRound2;
uint64_t tReply2;
uint8_t knownNodes[32];
double tPropTick;

MLat<5> mlat;
uint8_t mlat_range_target = 1;


void register_node() {
    uint8_t addr = rxFrame.src;
    knownNodes[addr/32] |= 1 << (addr % 8);
}

void send_range_transfer() {
    dwGetReceiveTimestamp(dwm, &tEndRound2);
   	txFrame.type = RANGE_TRANSFER;
    txFrame.src = ADDR;
    txFrame.dest = rxFrame.src;
    txFrame.seq++;
	memcpy(txFrame.data, tStartReply1.raw, 5);
	memcpy((txFrame.data+5), tEndReply1.raw, 5);
	memcpy((txFrame.data+10), tEndRound2.raw, 5);
    sendDWM((uint8_t *)&txFrame, 19);
}

void send_range(double range) {
   	txFrame.type = RANGE_DATA;
    txFrame.src = ADDR;
    txFrame.dest = rxFrame.src;
    txFrame.seq++;
	memcpy(txFrame.data, &range, sizeof(range));
    sendDWM((uint8_t *)&txFrame, NO_DATA_FRAME_SIZE + sizeof(range));
    //uart2.printf("%u, %u, %Lf\r\n", txFrame.src, txFrame.dest, range);
     
}

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

void send_position() {
    greenLed = 0;
   	txFrame.type = RANGE_DATA;
    txFrame.src = ADDR;
    txFrame.dest = 0;
    txFrame.seq++;
    float x = (float) mlat.position[0];
    float y = (float) mlat.position[1];
    float z = (float) mlat.position[2];
	memcpy(txFrame.data, &x, sizeof(x));
	memcpy(txFrame.data+sizeof(x), &y, sizeof(y));
	memcpy(txFrame.data+sizeof(x)+sizeof(y), &z, sizeof(z));
    sendDWM((uint8_t *)&txFrame, NO_DATA_FRAME_SIZE + sizeof(x)+sizeof(y)+sizeof(z));
    uart2.printf("%i, %.2f, %.2f, %2f\r\n", ADDR, x, y, z);
    greenLed = 1;
}

void receive_position() {
    float x;
    float y;
    float z;
	memcpy(&x, rxFrame.data, sizeof(x));
	memcpy(&y, rxFrame.data+sizeof(x), sizeof(y));
	memcpy(&z, rxFrame.data+sizeof(x)+sizeof(y), sizeof(z));
    uart2.printf("%i: %.2f, %.2f, %.2f\r\n", rxFrame.src, x, y, z);
}


double calculate_range() {
	dwGetData(dwm, (uint8_t*) &rxFrame, sizeof(rxFrame));

	memcpy(tStartReply1.raw, rxFrame.data, 5);
	memcpy(tEndReply1.raw, (rxFrame.data+5), 5);
	memcpy(tEndRound2.raw, (rxFrame.data+10), 5);

	calculateDeltaTime(&tStartRound1, &tStartReply2, &tRound1);
	calculateDeltaTime(&tStartReply1, &tEndReply1, &tReply1);
	calculateDeltaTime(&tStartReply2, &tEndReply2, &tReply2);
	calculateDeltaTime(&tEndReply1, &tEndRound2, &tRound2);
	
	calculatePropagationFormula(tRound1, tReply1, tRound2, tReply2, tPropTick);
    return calculateDistanceFromTicks(tPropTick);
}


void txcallback(dwDevice_t *dev){
    sending = false;
    switch(txFrame.type) {
        case RANGE_0:
            dwGetTransmitTimestamp(dev, &tStartRound1);
            break;
        case RANGE_1:
            dwGetReceiveTimestamp(dev, &tStartReply1);
            dwGetTransmitTimestamp(dev, &tEndReply1);
            break;
        case RANGE_2:
            dwGetReceiveTimestamp(dev, &tStartReply2);
            dwGetTransmitTimestamp(dev, &tEndReply2);
            break;
    }
    DWMReceive();
}
uint8_t Buffer[256+4];
void handle_data_frame() {
    size_t length = dwGetDataLength(dwm);
    // cap length to buffer size
    if(length > sizeof(Buffer)) {
        length = sizeof(Buffer);
    }
    dwGetData(dwm, Buffer, length);
    // write to the circular buffer, ignoring the 4 header bytes
    // cap read_length to match buffer size (length - 4 may otherwise crash the buffer)
    uint8_t read_length = length - 4;
    circularBuffer_write(&DWMcb, Buffer+4, read_length);
    DWMReceive();
}
void receive_range_answer() {
    double range;
    dwGetData(dwm, (uint8_t*) &rxFrame, NO_DATA_FRAME_SIZE + sizeof(range));
    memcpy(&range, rxFrame.data, sizeof(range));
    //uart2.printf("%u, %u, %Lf\r\n", rxFrame.src, rxFrame.dest, range);
    send_pprz_range_message(rxFrame.src, rxFrame.dest, range);
}

void handle_broadcast_packet() {
    switch(rxFrame.type) {
        case DATA_FRAME:
            handle_data_frame();
            break;
        case PING:
            send_rp(PONG);
            break;
        case PONG:
            register_node();
            DWMReceive();
            break;
        case POSITION:
            receive_position();
            DWMReceive();
            break;
        default:
            uart2.printf("unknown frame type\n\r");
            DWMReceive();
            break;
    }
}

void handle_own_packet() {
    switch(rxFrame.type) {
        case RANGE_0:
            send_rp(RANGE_1);
            break;
        case RANGE_1:
            send_rp(RANGE_2);
            break;
        case RANGE_2:
            //send_rp(RANGE_TRANSFER);
            send_range_transfer();
            break;
        case RANGE_TRANSFER: {
            double range = calculate_range();
            if(rxFrame.src >= MLAT_BASE_ADDR) {
                mlat.m[(rxFrame.src - MLAT_BASE_ADDR) % 5] = range;
            }
            //uart2.printf("%u, %u, %Lf\r\n", rxFrame.src, rxFrame.dest, range);
            send_range(range);
            break;
                             }
        case RANGE_DATA:
            receive_range_answer();
            DWMReceive();
            break;
        default:
            handle_broadcast_packet();
            break;
    }
}

void handle_foreign_packet() {
    switch(rxFrame.type) {
        case RANGE_DATA:
            receive_range_answer();
            break;
        default:
            break;
    }
    DWMReceive();
}

void failcallback(dwDevice_t *dev) {
    DWMReceive();
}

void rxcallback(dwDevice_t *dev)
{ 
    dwGetData(dwm, (uint8_t*) &rxFrame, NO_DATA_FRAME_SIZE);
    if(rxFrame.src == ADDR) {
        uart2.printf("received own packet - shouldn't happen\r\npossibly the address was given to multiple nodes\r\n\n");
        return;
    }
    switch(rxFrame.dest) {
        case ADDR:
            handle_own_packet();
            break;
        case 0:
            handle_broadcast_packet();
            break;
        case 255:
            handle_broadcast_packet();
            break;
        default:
            handle_foreign_packet();
            break;
    }
    return;
}


void resetRangeVariables() {
    tStartRound1.full = 0;
    tStartReply1.full = 0;
    tEndReply1.full = 0;
    tEndRound2.full = 0;
    tStartReply2.full = 0;
    tEndReply2.full = 0;

    tDelay.full = 0;
    tDelay.full = 74756096;//63897600*5; //5msec 
    rxFrame.type = 0;
    txFrame.type = 0;
}




/*
 * PPRZ-message: ABCxxxxxxxDE
    A PPRZ_STX (0x99)
    B LENGTH (PPRZ_STX->PPRZ_CHECKSUM_B)
    C PPRZ_DATA
      0 SENDER_ID
      1 MSG_ID
      2 MSG_PAYLOAD
      . DATA (messages.xml)
    D PPRZ_CHECKSUM_A (sum[B->C])
    E PPRZ_CHECKSUM_B (sum[ck_a])

 *
 */
void send_pprz_range_message(uint8_t src, uint8_t dest, double range) {
    uint8_t message[4+2+2+sizeof(range)]; 
    /* ABDE = 4; C0+C1 = 2; C2=2+sizeof(range) */
    uint8_t idx = 0;
    message[idx++] = 0x99; // A
    message[idx++] = sizeof(message); // Room for B
    message[idx++] = src; // C0
    message[idx++] = PPRZ_MSG_ID; // C1
    // C2 Data
    message[idx++] = src; 
    message[idx++] = dest;
    memcpy(&message[idx], &range,  sizeof(range));
    idx += sizeof(range);
    uint8_t checksumA = 0;
    uint8_t checksumB = 0;
    for(uint8_t i = 1; i < idx; i++) {
        checksumA += message[i];
        checksumB += checksumA;
    }
    message[idx++] = checksumA;
    message[idx++] = checksumB;

    sendUART(message, sizeof(message));
}


int main() {
    mlat.anchors << 0  ,  0,  0,
                    .45,.60,  0,
                    .0 ,.60,  0,
                    .0 ,  0,.30,
                    .45,  0,.30;
    mlat.position << .5,.5,.5;
    mlat.m << 0, 1, 1, 1, 1.41;
    initialseSerial();
    t_irq.start(callback(&IRQqueue, &EventQueue::dispatch_forever));
    t_irq.set_priority(osPriorityHigh);
    resetRangeVariables();
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
