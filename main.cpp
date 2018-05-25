#include "mbed.h"
extern "C" {
#include "libdw1000.h"
}

DigitalOut heartbeat(LED2);
DigitalOut led(LED1);
SPI spi(SPI_MOSI, SPI_MISO, SPI_SCK);
DigitalOut cs(SPI_CS);
DigitalIn sIRQ(PA_0);
DigitalInOut sReset(PA_1);
Serial copter_data(PA_9, PA_10, 38400); // Serial Interface 4 Debug stuff
//Serial copter_data(PA_2, PA_3,38400); // Serial Interface to the Copter
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

void txcallback(dwDevice_t *dev)
{
  dwNewReceive(dwm); // go back to recive mode if tx is finished !!!!!!!!
  dwStartReceive(dwm);
}
void rxcallback(dwDevice_t *dev)
{
  //Signal the reception of data by toggeling the leds
  //print the receved data to the uart
  //set the state of the module back to recive (keep listening to incoming data)
  heartbeat = !heartbeat;
  uint8_t recievedData[dwGetDataLength(dwm)];
  dwGetData(dwm, recievedData, dwGetDataLength(dwm));
  int length = sizeof(recievedData);
  for(int i = 0; i<length; i++){
    copter_data.putc(recievedData[i]); // write the recived data to the Serial busd
  }

  dwNewReceive(dwm); // Set Mode back to recive
  dwSetDefaults(dwm);
  dwStartReceive(dwm);
}

int length;
int goalLength;
enum {DONE, SOF, BUSY};
static int state = DONE;
uint8_t data[255];


void send(dwDevice_t* dev, int length) {
	dwNewTransmit(dev);
	dwSetDefaults(dev);
	dwSetData(dev, data, length);
	dwStartTransmit(dev);
}


void serialcallback()
{
  // this should collect the packets to be sent, and if an packet is complets, it should be sent ... wow ...
  if(copter_data.readable())
  {
    switch(state) {
      case(DONE): {
        data[0] = copter_data.getc();
        goalLength = 0;
        length = 0;
        if(data[0] == 0x99) {
          state = SOF;
          length++;
        }
      }break;
      case(SOF): {
        data[1] = copter_data.getc();
        length++;
        goalLength = data[1];
        state = BUSY;
      }break;
      case(BUSY): {
        data[length] = copter_data.getc(); // i have to think about this, if this makes sense ?!? --> is there an other function to read things from serial ?
        length ++;
        if(length>=goalLength) {
          send(dwm, length); // send packages
          state = DONE;
        }
      } break;
    }
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

  bool isSender = false; // change to send dummy packages only if true (Debug Reasons ...)

  reset(dwm);

	heartbeat = 1;
	sReset.input();
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
  dwInterruptOnSent(dwm, true);

	dwNewConfiguration(dwm);
	dwSetDefaults(dwm);
	dwEnableMode(dwm, MODE_SHORTDATA_FAST_ACCURACY);
	dwSetChannel(dwm, CHANNEL_2);
	dwSetPreambleCode(dwm, PREAMBLE_CODE_64MHZ_9);
	dwCommitConfiguration(dwm);
  wait(0.5f);

  //copter_data.attach(&serialcallback); // ??? maybe sometime in the future

  // DEBUG REASONS //
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
        wait(.01f);
    }
  }
  // DEBUG RESONS END //

  else {
    dwNewReceive(dwm);
    dwSetDefaults(dwm);
    dwStartReceive(dwm);
    while (true){
      if(sIRQ == 1){
        dwHandleInterrupt(dwm); // check dwm status, if rx incoming, go to rx callback, if tx finished, go to txcallback and turn mode back to recive
      }
      serialcallback(); // go to the serial parsing statemashine
    }
  }


}
