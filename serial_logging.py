import serial
from datetime import datetime

ser = serial.Serial("/dev/ttyUSB0", baudrate=115200)
myfile = open("./data/sync_error_test2.txt", "a")

for i in range(0, 5001):
    print(f"{i}/5001")
    g = ser.readline()
    now = datetime.now().time()
    print(g)
    myfile.write(g.decode('utf8'))  #
