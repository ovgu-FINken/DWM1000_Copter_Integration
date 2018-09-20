extern "C" {
#include "libdw1000.h"
#include "circularBuffer.h"
}
#include "platform/CircularBuffer.h"
#include "mbed.h"


DigitalOut txLed(LED2);
DigitalOut debugLed(LED1);
SPI spi(SPI_MOSI, SPI_MISO, SPI_SCK);
DigitalOut cs(SPI_CS);
InterruptIn sIRQ(PA_0);
DigitalInOut sReset(PA_1);
Serial uart1(PA_9, PA_10, 38400); // Serial Interface 4 Debug stuff
circularBuffer UARTcb;
circularBuffer DWMcb;
uint8_t UARTcb_data[256];
uint8_t DWMcb_data[256];

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

typedef struct __attribute__((packed, aligned(1))) DataFrame {
		uint16_t frameControl  = 0b1000100000000000;
		uint8_t sequenceNumber = 0x00;
		uint8_t destPan[2];
		uint16_t destAddress;
		uint8_t srcPan[2];
        uint8_t srcAddress[2];
        uint8_t type;
		uint8_t data[15];
}DFrame;

enum FrameType{
        PING=1,
        ANCHOR_RESPONSE=2,
        BEACON_RESPONSE=3,
        TRANSFER_FRAME=4,
        DISTANCES_FRAME=5,
        PPRZ_FRAME=6
};

// swtich back to receive mode when tx is finished
void txcallback(dwDevice_t *dev){
    dwNewReceive(dwm);
    dwStartReceive(dwm);
}

//Signal the reception of data by toggeling the leds
//print the receved data to the uart
//set the state of the module back to recive (keep listening to incoming data)
void rxcallback(dwDevice_t *dev)
{
    uint8_t Buffer[128];
    txLed = !txLed;
    uint8_t length = dwGetDataLength(dwm);
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
    dwNewReceive(dwm); // Set Mode back to recive
    dwStartReceive(dwm);
}

int length;
int goalLength;
enum {DONE, SOF, BUSY};
static int state = DONE;

void send(dwDevice_t* dev, uint8_t* data, int length) {
    dwNewTransmit(dev);
    dwSetData(dev, data, length);
    dwStartTransmit(dev);
}


void serialRead() {
    // this should collect the packets to be sent, and if an packet is complets, it should be sent ... wow ...
    while(uart1.readable())
    {
        circularBuffer_write_element(&UARTcb, uart1.getc());
        /*
        switch(state) {
            case(DONE): {
                            data[0] = uart1.getc();
                            goalLength = 0;
                            length = 0;
                            if(data[0] == 0x99) {
                                state = SOF;
                                length++;
                            }
                        }break;
            case(SOF): {
                           data[1] = uart1.getc();
                           length++;
                           goalLength = data[1];
                           state = BUSY;
                       }break;
            case(BUSY): {
                            data[length] = uart1.getc(); // i have to think about this, if this makes sense ?!? --> is there an other function to read things from serial ?
                            length ++;
                            if(length>=goalLength) {
                                send(dwm, length); // send packages
                                state = DONE;
                            }
                        } break;
        }
        */
    }
}

void initialiseDWM(void) {
    cs = 1;
    reset(dwm);
    dwInit(dwm, &ops);       // Init libdw
    uint8_t result = dwConfigure(dwm); // Configure the dw1000 chip
    if (result == 0) {
        dwEnableAllLeds(dwm);
    }


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
    wait(0.5f);
}
void dwIRQFunction(){
    dwHandleInterrupt(dwm);
}

void initialiseBuffers(){
    circularBuffer_init(&UARTcb, UARTcb_data, 256);
    circularBuffer_init(&DWMcb, DWMcb_data, 256);
}
int main() {
    initialiseDWM();

    dwNewReceive(dwm);
    dwStartReceive(dwm);
    sIRQ.rise(&dwIRQFunction);
    while (true){
        serialRead(); // go to the serial parsing statemashine
        uint8_t DWMWriteBuffer[255];
        uint8_t i = 0;
        // write to dwm
        while(circularBuffer_status(&UARTcb)!=circularBuffer_EMPTY) {
            DWMWriteBuffer[i++] = circularBuffer_read_element(&UARTcb);
        }
        send(dwm, (uint8_t*) DWMWriteBuffer, i);
        
        while(circularBuffer_status(&DWMcb) != circularBuffer_EMPTY) {
            uart1.putc(circularBuffer_read_element(&DWMcb));
        }
    }
}
