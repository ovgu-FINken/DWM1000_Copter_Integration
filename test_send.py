import serial
import time
import sys
import subprocess

period = 0.001
blockSize = 5000

if len(sys.argv) < 2:
    print "usage: %s <ttyUSB number> <baudrate> <rtscts=0|1> <output generating program call>\n"%sys.argv[0]
    sys.exit()

#inPort = serial.Serial('/dev/tty.usbmodemBDE5A903', int(sys.argv[1]), rtscts = True)
inPort = serial.Serial('/dev/tty.SLAB_USBtoUART', int(sys.argv[1]), rtscts = False)

inPort.write(' ')

start = time.time()
count = 0

print "Evaluation started: %s"%start
pre = time.time();
toWrite = blockSize
try:
    while toWrite>0:
        #out = "foobuianeuriaendariunedtrnaedtrunarsnafubingl%s"% (toWrite%20)
        out = "hallo"
        written = 0
	checkA = len(out)+4
	checkB = len(out)+4
	for c in list(out):
	    checkA = (checkA + ord(c)) % 256
            checkB = (checkA + checkB) % 256
	written = written + inPort.write(chr(0x99)+chr(len(out)+4)+out+chr(checkA)+chr(checkB))
        count = count + written
        toWrite = toWrite - written
        time.sleep(period*(len(out)+4))
        
        if toWrite < 0:
            post = time.time()
            print "Data rate(%u): %f baud"%(blockSize, 8 * blockSize/(post-pre)) 
            pre = time.time();
            toWrite = blockSize
	    period -= 0.0001
            if period < 0:
               period = 0.001

except:
    print sys.exc_info()

end = time.time()

print "Evaluation ended: %s"%end
print "Bytes send: %u"%count
print "Avg data rate: %f baud"%(8*count/(end-start)) 
