#pragma once
// Node Address for wireless interface
#include "cstdint"
extern uint8_t node_address;
#ifndef ADDR
#define ADDR 2
#endif

// Base Adress for multilateration - address space above this value is reserved for anchor nodes
// Anchor node configuration is in mlat.cpp for now
#ifndef MLAT_BASE_ADDR
#define MLAT_BASE_ADDR 128
#endif

// Flag to turn off multilateration computation
// multilateration uses an anytime, iterative solver that will consume computation power unless turned off
#ifndef MLAT_ACTIVE
#define MLAT_ACTIVE 0
#endif

// After this inervall the next automatic ranging is started
#define RANGE_INTERVALL_US 100

// Intervall for checking wether the IRQ pin is not going to low again
// IRQ checking function in dwm_logic.h/cpp
#define IRQ_CHECKER_INTERVALL 100
#define IRQ_CHECKER_THRESHOLD 3

// enable the height measurement
// define the intervall between measuerements in ms
// define the i2c adress of the height sensors (7 bit adress needs to be shifted once to match 8 bit structure)
// e.g. adress is 0x30 --> HEIGHT_SENSOR_ADDR = 0x60
// size of the height-message
#define ENABLE_HEIGHT false
#define HEIGHT_INTERVALL_MS 1000
#define HEIGHT_SENSOR_ADDR 0x60
#define HEIGHT_MESSAGESIZE 8

//enable Motor enables the motor for the Seilbahn (DOES NOT WORK TOGETHER WITH THE HEIGHT MEASUREMENT BECAUSE OF REUSED PINS)
#define MOVE_MOTOR true

// Message ID of the range message send over UART in PPRZ protocol
// for more detail look into pprz.h/pprz.cpp
#define PPRZ_RANGE_MSG_ID 254
#define PPRZ_HEIGHT_MSG_ID 253 // oder so kp ?
#define PPRZ_MOTOR_MSG_ID 252 // oder so kp ?
// SWITCH_UART: Defines wether to swtich the UARTs (in case one is not working soldered)
// By default UART1 is used for PPRZ, UART2 is used for debug/range output
// For implementation details of UART look into serial_logic.h/cpp
#ifndef SWITCH_UART
#define SWITCH_UART 1
#endif

// Baudrate of telemetry UART
#define TELEMETRY_BAUD 38400

// Baudrate of debug UART
#define DEBUG_BAUD 115200

// Disable/Enable UART-Echo (puts characters to output in UART-isr, only in telemetry UART)
#define ECHO 0
