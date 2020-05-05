# 1 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino"
# 2 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino" 2
# 12 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino"
volatile uint8_t buffer[100];
volatile int i = 0;
void LED(uint8_t color){
    switch (color){
        case 1:
            digitalWrite(0, 0x0);
            digitalWrite(A2, 0x1);
            digitalWrite(A3, 0x1);
            break;
        case 2:
            digitalWrite(0, 0x1);
            digitalWrite(A2, 0x0);
            digitalWrite(A3, 0x1);
            break;
        case 3:
            digitalWrite(0, 0x1);
            digitalWrite(A2, 0x1);
            digitalWrite(A3, 0x0);
            break;
        case 4:
            digitalWrite(0, 0x1);
            digitalWrite(A2, 0x1);
            digitalWrite(A3, 0x1);
            break;
    }
}


# 39 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino" 3
extern "C" void __vector_17 /* SPI Serial Transfer Complete */ (void) __attribute__ ((signal,used, externally_visible)) ; void __vector_17 /* SPI Serial Transfer Complete */ (void)

# 40 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino"
{
    buffer[i] = 
# 41 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino" 3
               (*(volatile uint8_t *)((0x2E) + 0x20))
# 41 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino"
                   ;
    i+=1;
}

bool checkSumCalc(int packetSum){
  uint8_t ck1 = floor(packetSum / 256);
  if(ck1==buffer[3]){
    uint8_t ck2 = packetSum % 256;
    if(ck2==buffer[4]){
      return true;
    }else{
      return false;
    }
  }else{
    return false;
  }

}

void checkpacket(uint8_t buff[]){
    int packetSum = 0;
    uint8_t header = buff[0];
    uint8_t cmd = buff[1];
    uint8_t dataLength = buff[2];
    packetSum+=header;
    packetSum+=cmd;
    packetSum+=dataLength;
    if(header == 255){
        switch (cmd){
            case 7:
                if(checkSumCalc(packetSum)){
                    
# 72 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino" 3
                   (*(volatile uint8_t *)((0x2E) + 0x20))
# 72 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino"
                       =20;
                }
                break;
        }
    }
    i=0;
}
void setup(){
    pinMode(0, 0x1);
    pinMode(A2, 0x1);
    pinMode(A3, 0x1);
    pinMode(MISO,0x1);
    
# 84 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino" 3
   (*(volatile uint8_t *)((0x2C) + 0x20)) 
# 84 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino"
        |= 
# 84 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino" 3
           (1 << (6))
# 84 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino"
                   ;
    SPI.attachInterrupt();
    LED(2);
}

void loop(void){
    checkpacket(buffer);
    LED(1);
}
