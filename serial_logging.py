import serial
from datetime import datetime


ser = serial.Serial("/dev/ttyUSB0", baudrate=115200)
myfile = open("./data/mod3_one_hour_10hz_v3", "a")

for i in range(0, 36001):
    print(f"{i}/36001")
    g = ser.readline()
    now = datetime.now().time()
    print(str(datetime.now()) + " " + g.decode('utf8'))
    myfile.write(g.decode('utf8'))  #
