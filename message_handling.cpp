#include "message_handling.h"
#include "mlat.h"
#include "serial_logic.h"
#include "leds.h"

void send_pprz_range_message(uint8_t src, uint8_t dest, double range) {
    uint8_t message[PPRZ_RANGE_MSG_SIZE];
    construct_pprz_range_message(message, src, dest, range);
    sendUART(message, sizeof(message));
}

void send_range_transfer() {
    dwGetReceiveTimestamp(dwm, &tEndRound2);
   	txFrame.type = RANGE_TRANSFER;
    txFrame.src = node_address;
    txFrame.dest = rxFrame.src;
    txFrame.seq++;
	memcpy(txFrame.data, tStartReply1.raw, 5);
	memcpy((txFrame.data+5), tEndReply1.raw, 5);
	memcpy((txFrame.data+10), tEndRound2.raw, 5);
    sendDWM((uint8_t *)&txFrame, 19);
}

void send_range(double range) {
   	txFrame.type = RANGE_DATA;
    txFrame.src = node_address;
    txFrame.dest = rxFrame.src;
    txFrame.seq++;
	memcpy(txFrame.data, &range, sizeof(range));
    sendDWM((uint8_t *)&txFrame, NO_DATA_FRAME_SIZE + sizeof(range));
    send_pprz_range_message(txFrame.src, txFrame.dest, range);
}

void send_position() {
    greenLed = 0;
   	txFrame.type = RANGE_DATA;
    txFrame.src = node_address;
    txFrame.dest = 0;
    txFrame.seq++;
    float x = (float) mlat.position[0];
    float y = (float) mlat.position[1];
    float z = (float) mlat.position[2];
	memcpy(txFrame.data, &x, sizeof(x));
	memcpy(txFrame.data+sizeof(x), &y, sizeof(y));
	memcpy(txFrame.data+sizeof(x)+sizeof(y), &z, sizeof(z));
    sendDWM((uint8_t *)&txFrame, NO_DATA_FRAME_SIZE + sizeof(x)+sizeof(y)+sizeof(z));
    uart2.printf("%i, %.2f, %.2f, %2f", node_address, x, y, z);
    for(int i = 0; i< 8; i++){
      double range = mlat.m[i];
      uart2.printf("\t,%.3f", range);
    }
    uart2.printf("\r\n");
    greenLed = 1;
}

void receive_position() {
    float x;
    float y;
    float z;
	memcpy(&x, rxFrame.data, sizeof(x));
	memcpy(&y, rxFrame.data+sizeof(x), sizeof(y));
	memcpy(&z, rxFrame.data+sizeof(x)+sizeof(y), sizeof(z));
    //uart2.printf("%i: %.2f, %.2f, %.2f\r\n", rxFrame.src, x, y, z);
}

void send_nmea(){
  float x = (float) mlat.position[0];
  float y = (float) mlat.position[1];
  float z = (float) mlat.position[2];
  string nmea_msg = build_nmea_msg(x,y,z);
  uart2.printf("%s",nmea_msg.c_str());
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
            dwGetData(dwm, (uint8_t*) &rxFrame, sizeof(rxFrame));
            double range = calculate_range();
            if(rxFrame.src >= MLAT_BASE_ADDR) {
                mlat.m[(rxFrame.src - MLAT_BASE_ADDR) % NUMBER_ANCHORS] = range;
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
  if(rxFrame.src == node_address) {
    uart2.printf("received own packet - shouldn't happen\r\npossibly the address was given to multiple nodes\r\n\n");
    DWMReceive();
    return;
  }
  switch(rxFrame.dest) {
    case 0:
      handle_broadcast_packet();
      break;
    case 255:
      handle_broadcast_packet();
      break;
    default:
      if(rxFrame.dest == node_address)
        handle_own_packet();
      else
        handle_foreign_packet();
      break;
  }
  return;
}

void send_rp(FrameType type) {
    txFrame.type = type;
    txFrame.src = node_address;
    txFrame.dest = rxFrame.src;
    txFrame.seq++;
    sendDWM((uint8_t*)&txFrame, NO_DATA_FRAME_SIZE);
}

void attach_message_handlers() {
    dwAttachSentHandler(dwm, txcallback);
    dwAttachReceivedHandler(dwm, rxcallback);
    dwAttachReceiveTimeoutHandler(dwm, failcallback);
    dwAttachReceiveFailedHandler(dwm, failcallback);
}
