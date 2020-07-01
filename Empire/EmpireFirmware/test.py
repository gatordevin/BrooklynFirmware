import serial
from time import sleep
import subprocess
ser = serial.Serial(port="/dev/ttyACM0",baudrate=1000000)
ser.write(bytearray([120,2]))

ser.close()