#include "ranging.h"
#include "dwm_logic.h"

extern "C" {
#include "libdw1000.h"
}


dwTime_t tStartRound1;
dwTime_t tStartReply1;
dwTime_t tEndReply1;
dwTime_t tEndRound2;
dwTime_t tStartReply2;
dwTime_t tEndReply2;
dwTime_t tDelay;
uint64_t tRound1;
uint64_t tReply1;
uint64_t tRound2;
uint64_t tReply2;
double tPropTick;

void calculateDeltaTime(dwTime_t* startTime, dwTime_t* endTime, uint64_t* result){
	uint64_t start = (startTime->full);
	uint64_t end = (endTime->full);

	if(end > start){
		*result = (end - start);
	}
	else{
		*result = (end + (1099511627776-start));
	}
}


void calculatePropagationFormula(const uint64_t& tRound1, const uint64_t& tReply1, const uint64_t& tRound2, const uint64_t& tReply2, double& tPropTick){
	tPropTick = (double)((tRound1 * tRound2) - (tReply1 * tReply2)) / (tRound1 + tReply1 + tRound2 + tReply2);
}

double calculateDistanceFromTicks(uint64_t tprop) {
    return speedOfLight * tprop / tsfreq - MAGIC_RANGE_OFFSET;
}

double calculate_range() {
	dwGetData(dwm, (uint8_t*) &rxFrame, sizeof(rxFrame));

	memcpy(tStartReply1.raw, rxFrame.data, 5);
	memcpy(tEndReply1.raw, (rxFrame.data+5), 5);
	memcpy(tEndRound2.raw, (rxFrame.data+10), 5);

	calculateDeltaTime(&tStartRound1, &tStartReply2, &tRound1);
	calculateDeltaTime(&tStartReply1, &tEndReply1, &tReply1);
	calculateDeltaTime(&tStartReply2, &tEndReply2, &tReply2);
	calculateDeltaTime(&tEndReply1, &tEndRound2, &tRound2);

	// uart2.printf("%" PRIu64 ", %" PRIu64 ", %" PRIu64 ", %" PRIu64 "\r\n", tRound1, tReply1, tRound2, tReply2);
	uart2.printf("%" PRIu64 ", %" PRIu64 ", %" PRIu64 ", %" PRIu64 ", %" PRIu64 "\r\n", (tStartRound1.full), (tStartReply1.full), (tEndReply1.full), (tEndReply2.full), (tEndRound2.full));
	// todo: send all the times


	calculatePropagationFormula(tRound1, tReply1, tRound2, tReply2, tPropTick);

    return calculateDistanceFromTicks(tPropTick);
	// todo: implement sending results via UART (especially the Round and Reply Times)
}

void resetRangeVariables() {
    tStartRound1.full = 0;
    tStartReply1.full = 0;
    tEndReply1.full = 0;
    tEndRound2.full = 0;
    tStartReply2.full = 0;
    tEndReply2.full = 0;

    tDelay.full = 0;
    tDelay.full = 74756096;//63897600*5; //5msec
    rxFrame.type = 0;
    txFrame.type = 0;
}
