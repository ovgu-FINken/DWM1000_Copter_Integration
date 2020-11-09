#pragma once
#include "node_config.h"
#include "ranging.h"
#include "dwm_logic.h"
#include "pprz.h"
#include "dFrame.h"
#include "nmea_gps.h"
extern "C" {
#include "libdw1000.h"
#include "circular_buffer.h"
}

void txcallback(dwDevice_t *dev);
void failcallback(dwDevice_t *dev);
void rxcallback(dwDevice_t *dev);
void send_rp(FrameType type);
void send_position();
void send_nmea();
void attach_message_handlers();
