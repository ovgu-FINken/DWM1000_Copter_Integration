#pragma once
#include "inttypes.h"

extern "C" {
#include "libdw1000.h"
}

// offset of the ranging result in meters
#define MAGIC_RANGE_OFFSET 153.7 - 0.869707882
// size of the ranging frame without header
#define NO_DATA_FRAME_SIZE 4

// variables for _current_ tof measurement
extern dwTime_t tStartRound1;
extern dwTime_t tStartReply1;
extern dwTime_t tEndReply1;
extern dwTime_t tEndRound2;
extern dwTime_t tStartReply2;
extern dwTime_t tEndReply2;
extern dwTime_t tDelay;


void calculateDeltaTime(dwTime_t* startTime, dwTime_t* endTime, uint64_t* result);

void calculatePropagationFormula(const uint64_t& tRound1, const uint64_t& tReply1, const uint64_t& tRound2, const uint64_t& tReply2, double& tPropTick);

static const double tsfreq = 499.2e6 * 128; // Timestamp counter frequency
static const double speedOfLight = 299792458.0; // Speed of light in m/s

double calculateDistanceFromTicks(uint64_t tprop);
double calculate_range();
void resetRangeVariables();
