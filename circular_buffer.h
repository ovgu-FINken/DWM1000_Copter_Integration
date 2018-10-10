#ifndef __circular_buffer_h
#define __circular_buffer_h

#include "inttypes.h"
#include "stddef.h"
struct circularBuffer {
    size_t size;
    volatile size_t head;
    volatile size_t tail;
    volatile uint8_t* data;
};

enum circularBufferStatus {circularBuffer_FULL, circularBuffer_EMPTY, circularBuffer_OK};

void circularBuffer_init(struct circularBuffer* cb, uint8_t* p_data, size_t size);

enum circularBufferStatus circularBuffer_status(struct circularBuffer* cb);
size_t circularBuffer_fill(struct circularBuffer* cb);
size_t circularBuffer_capacity(struct circularBuffer* cb);

int circularBuffer_write(struct circularBuffer* cb, uint8_t* Buf, size_t l );
int circularBuffer_write_element( struct circularBuffer* cb, uint8_t c );
int circularBuffer_read( struct circularBuffer* cb, uint8_t* Buf, size_t l );
uint8_t circularBuffer_read_element( struct circularBuffer* cb );
uint8_t circularBuffer_peek( struct circularBuffer* cb, size_t index );
void circularBuffer_delete_all( struct circularBuffer* cb);
void circularBuffer_delete(struct circularBuffer* cb, size_t n);
uint8_t* circularBuffer_getHead( struct circularBuffer *cb );
void circularBuffer_incrementHead( struct circularBuffer *cb );
#endif // include guard
