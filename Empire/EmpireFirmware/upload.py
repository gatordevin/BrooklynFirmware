import serial
from time import sleep
import subprocess
ser = serial.Serial(port="/dev/ttyACM1",baudrate=1000000)
chips_to_flash = [0,1]
for chip in chips_to_flash:
    ser.write(bytearray([120,chip]))
    subprocess.run(["avrdude", "-c", "arduino", "-p", "atmega328p" ,"-P", "/dev/ttyACM1", "-b", "1000000", "-U", "flash:w:ArduinoOutput/EmpireFirmware.ino.hex"])
ser.close()