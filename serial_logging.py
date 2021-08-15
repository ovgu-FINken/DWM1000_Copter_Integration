import serial
from datetime import datetime

ser = serial.Serial("/dev/ttyUSB0", baudrate=115200)
myfile = open("./data/sync_error_test_boxed_modxxx.txt", "a")

for i in range(0, 10001):
    print(f"{i}/10001")
    g = ser.readline()
    now = datetime.now().time()
    print(g)
    myfile.write(g.decode('utf8'))  #
