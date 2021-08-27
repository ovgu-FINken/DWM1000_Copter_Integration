import serial
from datetime import datetime


ser = serial.Serial("/dev/ttyUSB0", baudrate=115200)
myfile = open("./data/test.txt", "a")

for i in range(0, 10001):
    print(f"{i}/10001")
    g = ser.readline()
    now = datetime.now().time()
    print(str(datetime.now()) + " " + g.decode('utf8'))
    myfile.write(str(datetime.now) + " " + g.decode('utf8'))  #
