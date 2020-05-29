# 1 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino"
# 2 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino" 2
# 12 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino"
volatile uint8_t buffer[100];
volatile int idx = 0;
volatile int ridx = 0;

uint8_t header;
uint8_t cmd;
uint8_t datalen;
uint8_t data[10];
uint8_t ck1;
uint8_t ck2;
uint8_t cmdPacket[2];
uint8_t datapacket[100];







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


# 56 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino" 3
extern "C" void __vector_17 /* SPI Serial Transfer Complete */ (void) __attribute__ ((signal,used, externally_visible)) ; void __vector_17 /* SPI Serial Transfer Complete */ (void)

# 57 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino"
{
    buffer[idx] = 
# 58 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino" 3
                 (*(volatile uint8_t *)((0x2E) + 0x20))
# 58 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino"
                     ;
    
# 59 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino" 3
   (*(volatile uint8_t *)((0x2E) + 0x20))
# 59 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino"
       =20;
    if(idx==99){
        idx=0;
    }
    idx+=1;
}

uint8_t readData(){
  while(idx==ridx){}
  uint8_t data = buffer[ridx];
  if(ridx==99){
    ridx=0;
  }
  ridx+=1;
  return(data);
}

bool receivePacket(){
    header = readData();
    if(header==255){
        cmd = readData();
        datalen = readData();
        for(int i=0;i<datalen;i++){
            data[i] = readData();
        }
        ck1 = readData();
        ck2 = readData();
        if(calcChecksum()){
            return true;
        }
    }
    return false;
}

bool calcChecksum(){
    int packetSum = 0;
    packetSum += header;
    packetSum += cmd;
    packetSum += datalen;
    for(int i=0;i<datalen;i++){
        packetSum += data[i];
    }
    if(floor(packetSum / 256) == ck1){
        if(packetSum % 256 == ck2){
            return true;
        }
    }
    return false;
}

void SPISend(uint8_t data){
    
# 110 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino" 3
   (*(volatile uint8_t *)((0x2E) + 0x20)) 
# 110 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino"
        = data;
    readData();
}

void SPISendPacket(uint8_t command[], uint8_t data[]){
    int packetSum = 0;
    SPISend(255);
    packetSum+=255;
    SPISend(command[0]);
    packetSum+=command[0];
    SPISend(command[1]);
    packetSum+=command[1];
    for(int i=0;i<command[1];i++){
        packetSum+=data[i];
        SPISend(data[i]);
    }
    ck1 = floor(packetSum / 256);
    ck2 = packetSum % 256;
    SPISend(ck1);
    SPISend(ck2);
}

void setup(){
    pinMode(0, 0x1);
    pinMode(A2, 0x1);
    pinMode(A3, 0x1);
    pinMode(MISO,0x1);
    
# 137 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino" 3
   (*(volatile uint8_t *)((0x2C) + 0x20)) 
# 137 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino"
        |= 
# 137 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino" 3
           (1 << (6))
# 137 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino"
                   ;
    
# 138 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino" 3
   (*(volatile uint8_t *)((0x2C) + 0x20)) 
# 138 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino"
        &= ~(
# 138 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino" 3
             (1 << (4))
# 138 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino"
                      ); //Arduino is Slave
    
# 139 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino" 3
   (*(volatile uint8_t *)((0x2E) + 0x20)) 
# 139 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino"
        = 0x67; //test value
    
# 140 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino" 3
   (*(volatile uint8_t *)((0x2C) + 0x20)) 
# 140 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino"
        |= 
# 140 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino" 3
           (1 << (7))
# 140 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino"
                    ; //we not using SPI.attachInterrupt() why?
    
# 141 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino" 3
   __asm__ __volatile__ ("sei" ::: "memory")
# 141 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino"
        ;
    LED(1);
}

void loop(void){

    if(receivePacket()){
        switch(cmd){
            case 0x00 /* GET MOTOR SPEED*/:
                LED(3);
                cmdPacket[0] = 2;
                cmdPacket[1] = 1;
                datapacket[0] = 139;
                SPISendPacket(cmdPacket,datapacket);
                break;
        }
    }
}
