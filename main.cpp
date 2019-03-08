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
#include "AccelStepper.h"
extern "C" {
#include "libdw1000.h"
#include "circular_buffer.h"
}
#if ENABLE_HEIGHT == true
I2C i2c(PB_7,PB_6); // todo: auslagern in i2c datei ?
uint8_t i2c_msg_buffer[HEIGHT_MESSAGESIZE];
bool height_update = false;
uint16_t height;
#endif
#if MOVE_MOTOR == true
AccelStepper stepper1(1, PA_10, PA_9);
Timer t;
#endif


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
#if ENABLE_HEIGHT == true
void getHeigth() {
  char cmd[3];
  cmd[0] = 0x01;
  cmd[1] = 0x00;
  cmd[2] = 0x00;
  i2c.read(HEIGHT_SENSOR_ADDR, cmd, 3);
  height = (uint16_t)(cmd[0]<<8 | cmd[1]);
  uart2.printf("Wert = %d\n\r", height);
  uart2.printf("\n\r");
  //constuct_pprz_height_message(i2c_msg_buffer, height);
  //sendDWM(i2c_msg_buffer, sizeof(i2c_msg_buffer));
  height_update = true;
}
#endif



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
    IRQqueue.call_every(500, send_position);
#endif
#if ENABLE_HEIGHT == true
    IRQqueue.call_every(HEIGHT_INTERVALL_MS, getHeigth);
#endif
#if MOVE_MOTOR == true
    stepper1.setMaxSpeed(MOTOR_MAX_SPEED);
    stepper1.setAcceleration(MOTOR_ACCEL);
    stepper1.setMinPulseWidth(200);
    t.start();
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
#if MOVE_MOTOR == true
            if(WriteBuffer[3] == PPRZ_MOTOR_MSG_ID){ // get message ID
              int32_t value = WriteBuffer[7] << 24 | WriteBuffer[6] << 16 | WriteBuffer[5] << 8 | WriteBuffer[4] ;
              uart2.printf("MotorMovement is: %i \n\r", value);
              stepper1.moveTo(value);
            }
            if(WriteBuffer[3] == PPRZ_MOTOR_RST_ID){
              stepper1.setCurrentPosition(0);
              redLed = 1;
            }
#endif
        }
        Thread::yield();
#if MOVE_MOTOR == true
          stepper1.run();
#endif


#if ENABLE_HEIGHT == true

        if(height_update == true){
          constuct_pprz_height_message(i2c_msg_buffer, height);
          WriteBuffer[0] = node_address;
          WriteBuffer[1] = 0; // Broacast
          WriteBuffer[2] = DATA_FRAME;
          WriteBuffer[3] = txFrame.seq++;
          for(uint i = 0; i< sizeof(i2c_msg_buffer); i++){
            WriteBuffer[i+4] = i2c_msg_buffer[i];
          }
          sendDWM(WriteBuffer, 8+4);
          height_update = false;
          redLed = !redLed;
        }

#endif


    }
}
