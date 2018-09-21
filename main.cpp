#include "mbed.h"
extern "C" {
#include "libdw1000.h"
#include "circular_buffer.h"
}


DigitalOut txLed(LED2);
DigitalOut debugLed(LED1);
SPI spi(SPI_MOSI, SPI_MISO, SPI_SCK);
DigitalOut cs(SPI_CS);
InterruptIn sIRQ(PA_0);
DigitalInOut sReset(PA_1);
Serial uart1(PA_9, PA_10, 38400);
Serial uart2(PA_2, PA_3, 38400);
circularBuffer UARTcb;
circularBuffer DWMcb;
uint8_t UARTcb_data[256];
uint8_t DWMcb_data[256];
EventQueue queue(8 * EVENTS_EVENT_SIZE);
Thread t;

/* variables for ranging*/

static const double tsfreq = 499.2e6 * 128; // Timestamp counter frequency
static const double speedOfLight = 299792458.0; // Speed of light in m/s
#define NO_DATA_FRAME_SIZE 4
dwTime_t tStartRound1;
dwTime_t tEndRound1;
dwTime_t tStartReply1;
dwTime_t tEndReply1;
dwTime_t tStartRound2;
dwTime_t tEndRound2;
dwTime_t tStartReply2;
dwTime_t tEndReply2;
dwTime_t tDelay;
uint64_t tRound1;
uint64_t tReply1;
uint64_t tRound2;
uint64_t tReply2;
long double tPropTick;
typedef struct __attribute__((packed, aligned(1))) DataFrame {
    uint8_t source;
    uint8_t destination;
    uint8_t type;
    uint8_t number; 
    uint8_t data[15];
}DFrame;
DFrame frame;

void calculateDeltaTime(dwTime_t* startTime, dwTime_t* endTime, uint64_t* result){

	uint64_t start = (startTime->full);
	uint64_t end = (endTime->full);

	if(end > start){
		*result = (end - start);
	}
	else{
		*result = (end + (1099511628000-start));
	}
}

void calculatePropagationFormula(const uint64_t& tRound1, const uint64_t& tReply1, const uint64_t& tRound2, const uint64_t& tReply2, long double& tPropTick){
	tPropTick = (long double)((tRound1 * tRound2) - (tReply1 * tReply2)) / (tRound1 + tReply1 + tRound2 + tReply2);
}
static void spiWrite(dwDevice_t* dev, const void* header, size_t headerLength,
        const void* data, size_t dataLength) {
    cs = 0;
    uint8_t* headerP = (uint8_t*) header;
    uint8_t* dataP = (uint8_t*) data;

    for(size_t i = 0; i<headerLength; ++i) {
        spi.write(headerP[i]);
    }
    for(size_t i = 0; i<dataLength; ++i) {
        spi.write(dataP[i]);
    }
    cs = 1;
}

static void spiRead(dwDevice_t* dev, const void *header, size_t headerLength,
        void* data, size_t dataLength) {
    cs = 0;
    uint8_t* headerP = (uint8_t*) header;
    uint8_t* dataP = (uint8_t*) data;

    for(size_t i = 0; i<headerLength; ++i) {
        spi.write(headerP[i]);
    }
    for(size_t i = 0; i<dataLength; ++i) {
        dataP[i] = spi.write(0);
    }

    cs = 1;
}

static void spiSetSpeed(dwDevice_t* dev, dwSpiSpeed_t speed)
{
    if (speed == dwSpiSpeedLow)
        spi.frequency(3*1000*1000);

    if (speed == dwSpiSpeedHigh)
        spi.frequency(20*1000*1000);
}

static void reset(dwDevice_t* dev)
{
    sReset.output();
    sReset = 0;
    wait(0.1);
    sReset.input();
}

static void delayms(dwDevice_t* dev, unsigned int delay)
{
    wait(delay * 0.001f);
}

static dwOps_t ops = {
    .spiRead = spiRead,
    .spiWrite = spiWrite,
    .spiSetSpeed = spiSetSpeed,
    .delayms = delayms,
    .reset = reset
};

dwDevice_t dwm_device;
dwDevice_t* dwm = &dwm_device;

enum FrameType{
    RANGE_0=0,
    RANGE_1=1,
    RANGE_2=2,
    RANGE_3=3,
    RANGE_TRANSFER=4,
    RANGE_DATA=5,
    DATA_FRAME=42
};

void send_rp(FrameType type) {
    dwNewTransmit(dwm);
    frame.type = type;
    dwSetData(dwm, (uint8_t*) &frame, NO_DATA_FRAME_SIZE);
    dwStartTransmit(dwm);
}

void send_range_transfer() {
    dwGetReceiveTimestamp(dwm, &tEndRound2);
   	frame.type = RANGE_TRANSFER;
	memcpy(frame.data, tStartReply1.raw, 5);
	memcpy((frame.data+5), tEndReply1.raw, 5);
	memcpy((frame.data+10), tEndRound2.raw, 5);
	dwSetData(dwm, (uint8_t*)&frame, sizeof(frame));
	dwNewTransmit(dwm);
    dwStartTransmit(dwm);  
}

void startRanging() {
    send_rp(RANGE_0);
}

void calculate_range() {
   
	dwGetData(dwm, (uint8_t*)&frame, sizeof(frame));

	memcpy(tStartReply1.raw, frame.data, 5);
	memcpy(tEndReply1.raw, (frame.data+5), 5);
	memcpy(tEndRound2.raw, (frame.data+10), 5);

	calculateDeltaTime(&tStartRound1, &tStartReply2, &tRound1);
	calculateDeltaTime(&tStartReply1, &tEndReply1, &tReply1);
	calculateDeltaTime(&tStartReply2, &tEndReply2, &tReply2);
	calculateDeltaTime(&tEndReply1, &tEndRound2, &tRound2);
	
	calculatePropagationFormula(tRound1, tReply1, tRound2, tReply2, tPropTick);

	long double tPropTime = (tPropTick / tsfreq); //in seconds

	long double distance = (tPropTime * speedOfLight);// - 154.03; //~0.3 m per nanosecond; offset 154.03 calculated at ~1 cm distance
 
	uart2.printf("%0.12Lf,%Lf;\r\n",tPropTime, distance);
	//pc.printf(" is Distance in m\r\n", distance);
	/*
	pc.printf("%"PRIu64"\r\n",tRound1);
	pc.printf("%"PRIu64"\r\n",tReply1);
	pc.printf("%"PRIu64"\r\n",tReply2);
	pc.printf("%"PRIu64"\r\n",tRound2);
	
	pc.printf("%"PRIu64"\n",tStartRound1);
	pc.printf("%"PRIu64"\n",tEndRound1);
	pc.printf("%"PRIu64"\n",tStartReply1);
    pc.printf("%"PRIu64"\n",tEndReply1);
	pc.printf("%"PRIu64"\n",tEndRound2);
	pc.printf("%"PRIu64"\n",tEndReply2);
	*/
	tStartRound1.full = 0;
	tEndRound1.full = 0;
	tStartReply1.full = 0;
	tEndReply1.full = 0;
	tStartRound2.full = 0;
	tEndRound2.full = 0;
	tStartReply2.full = 0;
tEndReply2.full = 0; 
}

void txcallback(dwDevice_t *dev){
    /*
       dwNewReceive(dwm);
       dwStartReceive(dwm);
       */
    switch(frame.type) {
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
}
void handle_data_frame() {
    uint8_t length = dwGetDataLength(dwm);
    uint8_t Buffer[128];
    while(length > 128) {
        dwGetData(dwm, Buffer, 128);
        for(uint8_t i = 0; i<128; ++i) {
            circularBuffer_write_element(&DWMcb, Buffer[i]);
        }
        length -= 128;
    }
    dwGetData(dwm, Buffer, length);
    for(uint8_t i = 0; i<length; ++i) {
        circularBuffer_write_element(&DWMcb, Buffer[i]);
    }
}

//Signal the reception of data by toggeling the leds
//print the receved data to the uart
//set the state of the module back to recive (keep listening to incoming data)
void rxcallback(dwDevice_t *dev)
{
    dwGetData(dwm, (uint8_t*) &frame, NO_DATA_FRAME_SIZE);
    switch(frame.type) {
        case DATA_FRAME:
            handle_data_frame();
            break;
        case RANGE_0:
            send_rp(RANGE_1);
            break;
        case RANGE_1:
            send_rp(RANGE_2);
            break;
        case RANGE_2:
            send_range_transfer();
            break;
        case RANGE_TRANSFER:
            calculate_range();
            //send_range_answer();
            break;
        case RANGE_DATA:
            //receive_range_answer();
            break;
        default:
            uart2.printf("unknown frame type\n\r");
    }
    dwNewReceive(dwm); // Set Mode back to recive
    dwStartReceive(dwm);
}

int length;
int goalLength;

void sendDWM(uint8_t* data, int length) {
    dwNewTransmit(dwm);
    dwSetData(dwm, data, length);
    dwStartTransmit(dwm);
}

void sendUART(uint8_t* data, int length) {
    for(uint8_t i = 0; i<length; i++) {
        uart1.putc(data[i]);
    }
}

void serialRead() {
    // this should collect the packets to be sent, and if an packet is complets, it should be sent ... wow ...
    while(uart1.readable())
    {
        circularBuffer_write_element(&UARTcb, uart1.getc());
    }
}
void resetRangeVariables() {
    tStartRound1.full = 0;
    tEndRound1.full = 0;
    tStartReply1.full = 0;
    tEndReply1.full = 0;
    tStartRound2.full = 0;
    tEndRound2.full = 0;
    tStartReply2.full = 0;
    tEndReply2.full = 0;

    tDelay.full = 0;
    tDelay.full = 74756096;//63897600*5; //5msec 
    frame.type = 0;
}

void dwIRQFunction(){
    dwHandleInterrupt(dwm);
}

void initialiseDWM(void) {
    cs = 1;
    reset(dwm);
    dwInit(dwm, &ops);       // Init libdw
    uint8_t result = dwConfigure(dwm); // Configure the dw1000 chip
    if (result == 0) {
        dwEnableAllLeds(dwm);
    }


    t.start(callback(&queue, &EventQueue::dispatch_forever));
    dwTime_t delay = {.full = 0};
    dwSetAntenaDelay(dwm, delay);

    dwAttachSentHandler(dwm, txcallback);
    dwAttachReceivedHandler(dwm, rxcallback);
    dwInterruptOnReceived(dwm, true);
    dwInterruptOnSent(dwm, true);

    dwNewConfiguration(dwm);
    dwSetDefaults(dwm);
    dwEnableMode(dwm, MODE_SHORTDATA_MID_ACCURACY);
    dwSetChannel(dwm, CHANNEL_7);
    dwSetPreambleCode(dwm, PREAMBLE_CODE_64MHZ_9);
    dwCommitConfiguration(dwm);
    sIRQ.rise(queue.event(&dwIRQFunction));
    dwReceivePermanently(dwm, true);
    dwInterruptOnReceived(dwm, true);
    dwInterruptOnSent(dwm, true);
    resetRangeVariables();
    dwNewReceive(dwm);
    dwStartReceive(dwm);
    wait(0.5f);
}

void initialiseBuffers(){
    circularBuffer_init(&UARTcb, UARTcb_data, 256);
    circularBuffer_init(&DWMcb, DWMcb_data, 256);
}

uint8_t check_pprz(circularBuffer* cb, size_t i, size_t fill) {
    if(circularBuffer_peek(cb, 0) != 0x99)
        return 0;
    uint8_t l = circularBuffer_peek(cb, 1);
    if(l < fill)
        return 0;
    return l;
}

uint8_t parsePPRZ(circularBuffer* cb, uint8_t* out) {
    while(circularBuffer_status(cb)!=circularBuffer_EMPTY && circularBuffer_peek(cb, 0)!=0x99) {
        circularBuffer_read_element(cb);
    }
    size_t fill = circularBuffer_fill(cb);
    if(fill < 3) {
        return 0;
    } 
    uint8_t l = check_pprz(cb, 0, fill);
    if(l) {
        if(l > fill)
            return 0;
        for(size_t j=0; j<l; j++) {
            out[j] = circularBuffer_read_element(cb);
        }
        debugLed = !debugLed;
        return l;
    }
    return 0;
}

int main() {
    uart2.printf("Start Ranging\n");
    initialiseBuffers();
    initialiseDWM();

    while (true){
        serialRead(); // go to the serial parsing statemashine
        uint8_t WriteBuffer[258];
        // write to dwm
        size_t l = parsePPRZ(&UARTcb, WriteBuffer+1);
        if(l){
            WriteBuffer[0] = DATA_FRAME;
            sendDWM(WriteBuffer, l+1);
        }
        l = parsePPRZ(&DWMcb, WriteBuffer);
        if(l){
            sendUART(WriteBuffer, l);
        }
    }
}
