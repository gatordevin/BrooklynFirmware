import serial
from time import sleep
import subprocess
ser = serial.Serial(port="COM3",baudrate=1000000)
chips_to_flash = [0]
for chip in chips_to_flash:
    ser.write(bytearray([120,chip]))
    ser.close()
    #subprocess.run(["avrdude", "-carduino", "-patmega328p" ,"-PCOM3", "-b1000000", "-U‪flash:w:C:/Arduino-Output/EmpireFirmware.ino.hex:i"])
    subprocess.run("avrdude -carduino -patmega328p -PCOM3 -b1000000 -Uflash:w:C:/Users/alexd/deepclean/BrooklynFirmware/Empire/EmpireFirmware/ArduinoOutput/EmpireFirmware.ino.hex:i")
    ser.open()
ser.close()