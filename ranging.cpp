#include "ranging.h"

extern "C" {
#include "libdw1000.h"
}

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


void calculatePropagationFormula(const uint64_t& tRound1, const uint64_t& tReply1, const uint64_t& tRound2, const uint64_t& tReply2, double& tPropTick){
	tPropTick = (double)((tRound1 * tRound2) - (tReply1 * tReply2)) / (tRound1 + tReply1 + tRound2 + tReply2);
}

double calculateDistanceFromTicks(uint64_t tprop) {
    return speedOfLight * tprop / tsfreq - MAGIC_RANGE_OFFSET;
}
