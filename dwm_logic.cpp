#include "dwm_logic.h"
#include "leds.h"
#include "thread_logic.h"

dwDevice_t dwm_device;
dwDevice_t* dwm = &dwm_device;
SPI spi(PB_15, PB_14, PB_13);
volatile bool sending;
DigitalOut cs(PB_12);
InterruptIn sIRQ(PA_7);
DigitalInOut sReset(PA_6);
circularBuffer DWMcb;
uint8_t DWMcb_data[256];
DFrame txFrame;
DFrame rxFrame;

void dwIRQFunction();
void irq_checker();
static void spiWrite(dwDevice_t* dev, const void* header, size_t headerLength,
        const void* data, size_t dataLength) {
    cs = 0;
    spi.lock();
    uint8_t* headerP = (uint8_t*) header;
    uint8_t* dataP = (uint8_t*) data;

    for(size_t i = 0; i<headerLength; ++i) {
        spi.write(headerP[i]);
    }
    for(size_t i = 0; i<dataLength; ++i) {
        spi.write(dataP[i]);
    }
    spi.unlock();
    cs = 1;
}

static void spiRead(dwDevice_t* dev, const void *header, size_t headerLength,
        void* data, size_t dataLength) {
    cs = 0;
    spi.lock();
    uint8_t* headerP = (uint8_t*) header;
    uint8_t* dataP = (uint8_t*) data;

    for(size_t i = 0; i<headerLength; ++i) {
        spi.write(headerP[i]);
    }
    for(size_t i = 0; i<dataLength; ++i) {
        dataP[i] = spi.write(0);
    }
    spi.unlock();
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
    redLed = 1;
    sReset = 0;
    wait(0.1);
    redLed = 0;
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


void initialiseDWM(void) {
    circularBuffer_init(&DWMcb, DWMcb_data, 256);
    sIRQ.mode(PullDown);
    sIRQ.rise(IRQqueue.event(&dwIRQFunction));
    reset(dwm);
    dwInit(dwm, &ops);       // Init libdw
    uint8_t result = dwConfigure(dwm); // Configure the dw1000 chip
    if (result == 0) {
        dwEnableAllLeds(dwm);
    }


    dwTime_t delay = {.full = 0};
    dwSetAntenaDelay(dwm, delay);
}

void startDWM(){
    dwInterruptOnReceived(dwm, true);
    dwInterruptOnSent(dwm, true);
    dwInterruptOnReceiveTimeout(dwm, true);
    dwInterruptOnReceiveFailed(dwm, true);

    dwNewConfiguration(dwm);
    dwSetDefaults(dwm);
    dwEnableMode(dwm, MODE_SHORTDATA_MID_ACCURACY);
    dwSetChannel(dwm, CHANNEL_5);
    dwSetPreambleCode(dwm, PREAMBLE_CODE_64MHZ_9);
    dwCommitConfiguration(dwm);
    //dwReceivePermanently(dwm, true);

    dwNewReceive(dwm);
    dwSetDefaults(dwm);
    dwStartReceive(dwm);
    wait(0.5f);
    IRQqueue.call_every(IRQ_CHECKER_INTERVALL, irq_checker);
}

void sendDWM(uint8_t* data, int length) {
    sending = true;
    spi.lock();
    dwNewTransmit(dwm);
    dwSetData(dwm, data, length);
    dwStartTransmit(dwm);
    spi.unlock();
}


void DWMReceive() {
    if(sending)
        return;
    dwNewReceive(dwm);
    dwStartReceive(dwm);
}

uint8_t irq_checker_count = 0;
void irq_checker() {
    greenLed = 0;
    if(sIRQ.read()) {
        irq_checker_count++;
    } else {
        irq_checker_count = 0;
        redLed = 0;
        return;
    }
    if(irq_checker_count < IRQ_CHECKER_THRESHOLD)
        return;
    redLed = 1;
    sending = false;
    DWMReceive();
    greenLed = 1;
}

void dwIRQFunction(){
    greenLed=0;
    dwHandleInterrupt(dwm);
    greenLed=1;
}
