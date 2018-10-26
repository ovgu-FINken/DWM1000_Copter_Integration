#ifndef __ranging_h
#define __ranging_h
#include "inttypes.h"

extern "C" {
#include "libdw1000.h"
}
// offset of the ranging result in meters
#define MAGIC_RANGE_OFFSET 153.7

// size of the ranging frame without header
#define NO_DATA_FRAME_SIZE 4

typedef struct __attribute__((packed, aligned(1))) DataFrame {
    uint8_t src;
    uint8_t dest;
    uint8_t type;
    uint8_t seq; 
    uint8_t data[15];
}DFrame;

enum FrameType{
    RANGE_0=0,
    RANGE_1=1,
    RANGE_2=2,
    RANGE_3=3,
    RANGE_TRANSFER=4,
    RANGE_DATA=5,
    RANGE_REQUEST=6,
    POSITION=7,
    DATA_FRAME=42,
    PING=254,
    PONG=255
};

void calculateDeltaTime(dwTime_t* startTime, dwTime_t* endTime, uint64_t* result);

void calculatePropagationFormula(const uint64_t& tRound1, const uint64_t& tReply1, const uint64_t& tRound2, const uint64_t& tReply2, double& tPropTick);

static const double tsfreq = 499.2e6 * 128; // Timestamp counter frequency
static const double speedOfLight = 299792458.0; // Speed of light in m/s

double calculateDistanceFromTicks(uint64_t tprop);
#endif // include guard
