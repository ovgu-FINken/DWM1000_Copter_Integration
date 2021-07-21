import serial 

ser = serial.Serial("/dev/ttyUSB0", baudrate=115200)
myfile = open("log_1m_no2.txt", "a")

for i in range(0, 1001):
    print(f"{i}/1001")
    g = ser.readline()
    myfile.write(g.decode('utf8'))
