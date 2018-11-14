#include "pprz.h"
#include "circular_buffer.h"


enum PPRZ_STATUS check_pprz(struct circularBuffer* cb) {
    size_t fill = circularBuffer_fill(cb);
    if(fill < 5)
        // buffer not full enough for whole package
        return PPRZ_SHORT;
    if(circularBuffer_peek(cb, 0) != 0x99)
        // no startbyte
        return PPRZ_BROKEN;
    uint8_t l = circularBuffer_peek(cb, 1);
    if(l > fill)
        // Buffer does not contain the hole package yet
        return PPRZ_SHORT;
    if(l <= 4) {
        // dismiss packet if length < HEADER + PAYLOAD
        return PPRZ_BROKEN;
    }
    uint8_t checksumA = 0;
    uint8_t checksumB = 0;
    for(size_t i = 1; i<l-2; i++) {
        checksumA += circularBuffer_peek(cb, i);
        checksumB += checksumA;
    }
    if(checksumA != circularBuffer_peek(cb, l-2)) {
        return PPRZ_BROKEN;
    }
    if(checksumB != circularBuffer_peek(cb, l-1)) {
        return PPRZ_BROKEN;
    }
    return PPRZ_OK;
}

uint8_t parsePPRZ(struct circularBuffer* cb) {
    enum PPRZ_STATUS status = check_pprz(cb);
    while(1) {
        switch(status) {
            case PPRZ_SHORT:
                return 0;
            case PPRZ_BROKEN:
                circularBuffer_read_element(cb);
                status = check_pprz(cb);
                break;
            case PPRZ_OK:
                return circularBuffer_peek(cb, 1);
        }
    }
    return 0;
}
