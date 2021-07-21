import serial 
from datetime import datetime

ser = serial.Serial("/dev/ttyUSB0", baudrate=115200)
myfile = open("times.txt", "a")

for i in range(0, 1001):
    print(f"{i}/1001")
    g = ser.readline()
    now = datetime.now().time()
    print(now)
    myfile.write(str(now))  #.decode('utf8')
