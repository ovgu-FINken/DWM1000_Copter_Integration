#include "pprz.h"
#include "mbed.h"
extern "C" {
#include "circular_buffer.h"
}


enum PPRZ_STATUS check_pprz(circularBuffer* cb) {
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

uint8_t parsePPRZ(circularBuffer* cb) {
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

/*
 * PPRZ-message: ABCxxxxxxxDE
    A PPRZ_STX (0x99)
    B LENGTH (PPRZ_STX->PPRZ_CHECKSUM_B)
    C PPRZ_DATA
      0 SENDER_ID
      1 MSG_ID
      2 MSG_PAYLOAD
      . DATA (messages.xml)
    D PPRZ_CHECKSUM_A (sum[B->C])
    E PPRZ_CHECKSUM_B (sum[ck_a])

 *
 */
void construct_pprz_range_message(uint8_t* buffer, uint8_t src, uint8_t dest, double range) {
    /* ABDE = 4; C0+C1 = 2; C2=2+sizeof(range) */
    uint8_t idx = 0;
    buffer[idx++] = 0x99; // A
    buffer[idx++] = sizeof(buffer); // Room for B
    buffer[idx++] = src; // C0
    buffer[idx++] = PPRZ_RANGE_MSG_ID; // C1
    // C2 Data
    buffer[idx++] = src; 
    buffer[idx++] = dest;
    memcpy(&buffer[idx], &range,  sizeof(range));
    idx += sizeof(range);
    uint8_t checksumA = 0;
    uint8_t checksumB = 0;
    for(uint8_t i = 1; i < idx; i++) {
        checksumA += buffer[i];
        checksumB += checksumA;
    }
    buffer[idx++] = checksumA;
    buffer[idx++] = checksumB;
}
