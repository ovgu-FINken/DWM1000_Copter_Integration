#pragma once
// Dataframe for wireless transmission - contains 15 Byte of Buffer space for convenience
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
