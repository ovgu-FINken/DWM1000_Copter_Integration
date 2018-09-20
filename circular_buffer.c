#include "circular_buffer.h"

enum circularBufferStatus circularBuffer_status(struct circularBuffer* cb) {
    if(cb->head == cb->tail)
        return circularBuffer_EMPTY;
    if( (cb->head + 1) % cb->size == cb->tail)
        return circularBuffer_FULL;
    return circularBuffer_OK;
}


void circularBuffer_init(struct circularBuffer* cb, uint8_t* p_data, size_t size) {
    cb->data = p_data;
    cb->size = size;
    cb->head = 0;
    cb->tail = 0;
}

// return number of elements currently contained in circularBuffer
size_t circularBuffer_fill(struct circularBuffer* cb) {
    if(cb->head >= cb->tail) {
        return cb->head - cb->tail;
    }
    return cb->size - cb->tail + cb->head;
}

// return number of unused uint8_ts in the buffer
size_t circularBuffer_capacity(struct circularBuffer* cb) {
    return cb->size - 1 - circularBuffer_fill(cb);
}

int circularBuffer_write(struct circularBuffer* cb, uint8_t* Buf, size_t l) {
    for(size_t i = 0; i < l; ++i) {
        circularBuffer_write_element(cb, Buf[i]);
    }
    return 1;
}

int circularBuffer_write_element( struct circularBuffer* cb, uint8_t c ) {
    cb->data[cb->head] = c;
    if(circularBuffer_status(cb) == circularBuffer_FULL)
        cb->tail = (cb->tail + 1) % cb->size;
    cb->head = (cb->head + 1) % cb->size;
    return 1;
}

uint8_t circularBuffer_read_element( struct circularBuffer* cb ) {
    if(circularBuffer_status(cb) == circularBuffer_EMPTY)
        return '\0';
    uint8_t value = cb->data[cb->tail];
    cb->tail = (cb->tail + 1) % cb->size;
    return value;
}

// return an element in front of the tail (read pointer) without moving it
// if index>fill undefined values will be reported (fill needs to be checked)
uint8_t circularBuffer_peek(struct circularBuffer* cb, size_t index) {
    return cb->data[(index + cb->tail) % cb->size];
}

void circularBuffer_delete_all( struct circularBuffer* cb) {
    cb->tail = cb->head;
}
void circularBuffer_delete(struct circularBuffer* cb, size_t n){
    if(n > circularBuffer_fill(cb)){ 
        circularBuffer_delete_all(cb);
        return;
    }
    cb->tail = (cb->tail + n) % cb->size;
}

uint8_t* circularBuffer_getHead( struct circularBuffer *cb ) {
    return (uint8_t*)cb->data + cb->head;
}

void circularBuffer_incrementHead( struct circularBuffer *cb ) {
    if(circularBuffer_status(cb) == circularBuffer_FULL)
        cb->tail = (cb->tail + 1) % cb->size;
    cb->head = (cb->head + 1) % cb->size;
}
