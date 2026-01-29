import serial
import time

time.sleep(1)
ser = serial.Serial()
ser.baudrate = 9600
ser.port = 'COM23'
ser.timeout = 1
ser.bytesize = serial.EIGHTBITS
ser.parity = serial.PARITY_NONE
ser.stopbits = serial.STOPBITS_ONE
ser.dsrdtr = False
ser.rtscts = False
ser.xonxoff = False
ser.open()
command = "*IDN?\n"
ser.write(command.encode(encoding="ascii",errors="ignore"))     # write a string
print(command)
s = ser.readline(100)       # read up to one hundred bytes
print(s)
command = ":SYSTEM:LD 1,0\n"
print(command)
ser.write(command.encode(encoding="ascii",errors="ignore"))     # write a string
time.sleep(0.2)
"""
while True:
    time.sleep(0.2)
    command = ":SYSTEM:LD 1,1\n"
    print(command)
    ser.write(command.encode(encoding="ascii",errors="ignore"))     # write a string
    time.sleep(0.2)
    command = ":SYSTEM:LD 3,0\n"
    print(command)
    ser.write(command.encode(encoding="ascii",errors="ignore"))     # write a string
    time.sleep(0.2)
    command = ":SYSTEM:LD 2,1\n"
    print(command)
    ser.write(command.encode(encoding="ascii",errors="ignore"))     # write a string
    time.sleep(0.2)
    command = ":SYSTEM:LD 1,0\n"
    print(command)
    ser.write(command.encode(encoding="ascii",errors="ignore"))     # write a string
    time.sleep(0.2)
    command = ":SYSTEM:LD 3,1\n"
    print(command)
    ser.write(command.encode(encoding="ascii",errors="ignore"))     # write a string
    time.sleep(0.2)
    command = ":SYSTEM:LD 2,0\n"
    print(command)
    ser.write(command.encode(encoding="ascii",errors="ignore"))     # write a string
    time.sleep(0.2)
"""
