#ifndef __pprz_h
#define __pprz_h

#include "circular_buffer.h"

enum PPRZ_STATUS {
    PPRZ_OK,
    PPRZ_SHORT,
    PPRZ_BROKEN
};

enum PPRZ_STATUS check_pprz(struct circularBuffer* cb);
uint8_t parsePPRZ(struct circularBuffer* cb);

#endif // include guard
