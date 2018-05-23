import serial
import time
import sys
import subprocess
import binascii

if len(sys.argv) < 3:
    print "usage: %s <device> <baudrate>\n"%sys.argv[0]
    sys.exit()

Port = serial.Serial(sys.argv[1], int(sys.argv[2]))

print "Start sending"

#data = bytearray({0x99, 0x8, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16}) # byte oder bytearray
#data = ''.join(chr(x) for x in [0x99, 0x08, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16])
#data = bytearray({0x16, 0x99, 0x8, 0x11, 0x12, 0x13, 0x14, 0x15}) # warum ist das 0x99 immer HINTEN ??? in data[7] ???
#x = '9908101112131415'
#data = bytes.fromhex(x);

data = []
data.append(0x99);
data.append(0x08);
data.append(0x10);
data.append(0x11);
data.append(0x12);
data.append(0x13);
data.append(0x14);
data.append(0x15);

while(True):

    g = Port.write(data);
    print hex(data[0]), hex(data[1]), hex(data[2]), hex(data[3]), hex(data[4]), hex(data[5]), hex(data[6]), hex(data[7]);
    print "Wrote ", g, "Bytes"
    time.sleep(0.5);
