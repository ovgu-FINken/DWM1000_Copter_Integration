// Packet format with compressed PAN and 64Bit addresses
// Maximum 128s bytes payload
typedef struct packet_s {
    union {
      uint16_t fcf;
      struct {
        uint16_t type:3;
        uint16_t security:1;
        uint16_t framePending:1;
        uint16_t ack:1;
        uint16_t ipan:1;
        uint16_t reserved:3;
        uint16_t destAddrMode:2;
        uint16_t version:2;
        uint16_t srcAddrMode:2;
      } fcf_s;
    };

    uint8_t seq;
    uint16_t pan;
    uint8_t destAddress[8];
    uint8_t sourceAddress[8];

    uint8_t payload[128];
} __attribute__((packed)) packet_t;
