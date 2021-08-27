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

#define NUMBER_ANCHORS 8
// sets the influence the calculated gradient has on the position update
// btw. we are in despreate need of some kind of filter !!
#define SOLVER_STEP_SIZE 0.1
// 2d instead of 3d localisation
#define TWODLOCALIZATION 1

// defines how often NMEA messages were sent (times/s)
#ifndef NMEA_UPDATE
#define NMEA_UPDATE 10
#endif

// After this inervall the next automatic ranging is started (actually it's ms)
#define RANGE_INTERVALL_US 100

// Intervall for checking wether the IRQ pin is not going to low again
// IRQ checking function in dwm_logic.h/cpp
#define IRQ_CHECKER_INTERVALL 100
#define IRQ_CHECKER_THRESHOLD 3

// Message ID of the range message send over UART in PPRZ protocol
// for more detail look into pprz.h/pprz.cpp
#define PPRZ_RANGE_MSG_ID 254

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
