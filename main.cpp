#include "mbed.h"
extern "C" {
#include "libdw1000.h"
}

DigitalOut heartbeat(LED2);
DigitalOut led(LED1);
SPI spi(SPI_MOSI, SPI_MISO, SPI_SCK);
DigitalOut cs(SPI_CS);
DigitalIn sIRQ(PA_0);
DigitalOut sReset(PA_1);
Serial pc(PA_9, PA_10, 38400); // Serial Interface 4 Debug stuff
Serial copter_data(PA_2, PA_3,38400); // Serial Interface to the Copter
// static int state = DONE; // state for the Serial parse state machine

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
		spi.frequency(4*1000*1000);

	if (speed == dwSpiSpeedHigh)
		spi.frequency(20*1000*1000);
}

static void reset(dwDevice_t* dev)
{
	sReset = 0;
	wait(0.5);
	sReset = 1;
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

void txcallback(dwDevice_t *dev)
{
  //led = 1;
}
void rxcallback(dwDevice_t *dev)
{
  //Signal the reception of data by toggeling the leds
  //print the receved data to the uart
  //set the state of the module back to recive (keep listening to incoming data)
  led = !led;
  heartbeat = !heartbeat;
  uint8_t data[dwGetDataLength(dwm)];
  dwGetData(dwm, data, dwGetDataLength(dwm));
  pc.printf("Data Received:\r\n");
  //data[dwGetDataLength(dwm)] = '\0';
  pc.printf("%s\n", data);

  dwNewReceive(dwm);
  dwSetDefaults(dwm);
  dwStartReceive(dwm);
}
void serialcallback()
{
  // this should collect the packets to be sent, and if an packet is complets, it should be sent ... wow ...
  // for test reasons : just write everithing we recieved here to the debug serial :D
  //heartbeat = 0;
  //led = 1;
  if(copter_data.readable())
  {
    uint8_t data[255];
    //copter_data.read(data, 1);
    data[0] = copter_data.getc();
    pc.printf("%x", data[0]);
  }
}

char* txPacket = "foobar";

void send_dummy(dwDevice_t* dev) {
	dwNewTransmit(dev);
	dwSetDefaults(dev);
	dwSetData(dev, (uint8_t*)txPacket, strlen(txPacket));
	dwStartTransmit(dev);
}


// main() runs in its own thread in the OS
int main() {

  bool isSender = false;

	heartbeat = 1;
	sReset = 1;
	cs = 1;
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

	dwNewConfiguration(dwm);
	dwSetDefaults(dwm);
	dwEnableMode(dwm, MODE_SHORTDATA_FAST_ACCURACY);
	dwSetChannel(dwm, CHANNEL_2);
	dwSetPreambleCode(dwm, PREAMBLE_CODE_64MHZ_9);
	dwCommitConfiguration(dwm);

  //copter_data.attach(serialcallback); // if data is recived from the copter to send to the ground station, the serialcallback funktion is called
  // does not work, the attach callback funkion itself just loops througth itself forever

  uint8_t sendercount = 0;
  if(isSender == true){
    while (true) {
        send_dummy(dwm);
        sendercount ++;
        char str[20];
        sprintf(str, "%d", sendercount);
        strcat(str, ". Message");
        txPacket = str;
        heartbeat = !heartbeat;
        wait(.5f);
    }
  }
  else {
    dwNewReceive(dwm);
    dwSetDefaults(dwm);
    dwStartReceive(dwm);
    while (true){
      dwHandleInterrupt(dwm);
      serialcallback();
    }
  }


}
