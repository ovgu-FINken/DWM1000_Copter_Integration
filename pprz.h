#ifndef __pprz_h
#define __pprz_h

#include "circular_buffer.h"
#define PPRZ_MSG_ID 254

/*
 * PPRZ message definition in sw/pprzlink/messages/v1.0/messages.xml
 * <message name="RANGE" id="254">
      <field name="src"                 type="uint8"/>
      <field name="dest"                type="uint8"/>
      <field name="range"               type="double"/>
    </message>
 * */
enum PPRZ_STATUS {
    PPRZ_OK,
    PPRZ_SHORT,
    PPRZ_BROKEN
};

enum PPRZ_STATUS check_pprz(struct circularBuffer* cb);
uint8_t parsePPRZ(struct circularBuffer* cb);

#endif // include guard
