import serial
from time import sleep
import subprocess
ser = serial.Serial(port="/dev/ttyACM0",baudrate=1000000)
chips_to_flash = [0,1,2,3]
for chip in chips_to_flash:
    print(chip)
    ser.write(bytearray([120,chip]))
    ser.close()
    subprocess.run(["avrdude", "-carduino", "-patmega328p" ,"-P/dev/ttyACM0", "-b1000000", "-U", "/‪ArduinoOutput/EmpireFirmware.ino.hex"])
    # subprocess.run("avrdude -c arduino -p atmega328p -P /dev/ttyACM0 -b 100000 -U /home/sam/Desktop/BrooklynFirmware/Empire/EmpireFirmware/ArduinoOutput/EmpireFirmware.ino.hex")
    ser.open()
ser.close()