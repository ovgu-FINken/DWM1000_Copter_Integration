import serial
from datetime import datetime


ser = serial.Serial("/dev/ttyUSB0", baudrate=115200)
myfile = open("./data/mod2_one_hour_5hz_v5", "a")

for i in range(0, 18001):
    print(f"{i}/18001")
    g = ser.readline()
    now = datetime.now().time()
    print(now)
    print(g.decode('utf8'))
    print()
    myfile.write(g.decode('utf8'))  #
