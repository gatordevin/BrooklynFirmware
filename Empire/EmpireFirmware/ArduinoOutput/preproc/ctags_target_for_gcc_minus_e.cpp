# 1 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino"
# 2 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino" 2
# 3 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino" 2
# 16 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino"
Servo servo_1;
Servo servo_2;
Servo servos[2] = {servo_1,servo_2};

int servo_1_min_angle = 0;
int servo_1_max_angle = 180;
int servo_1_min_microseconds = 1000;
int servo_1_max_microseconds = 2000;
int servo_2_min_angle = 0;
int servo_2_max_angle = 180;
int servo_2_min_microseconds = 1000;
int servo_2_max_microseconds = 2000;


volatile uint8_t interrupt_buff[100];
uint8_t spi_recv_buff[20];
uint8_t spi_send_buff[20];
volatile int idx = 0;
volatile int ridx = 0;

uint8_t checksum1 = 0;
uint8_t checksum2 = 0;






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


# 69 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino" 3
extern "C" void __vector_17 /* SPI Serial Transfer Complete */ (void) __attribute__ ((signal,used, externally_visible)) ; void __vector_17 /* SPI Serial Transfer Complete */ (void)

# 70 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino"
{
    interrupt_buff[idx] = 
# 71 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino" 3
                         (*(volatile uint8_t *)((0x2E) + 0x20))
# 71 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino"
                             ;
    if(idx==99){
        idx=0;
    }
    idx+=1;
}

uint8_t readByte(){
    while(idx==ridx){}
    uint8_t data = interrupt_buff[ridx];
    if(ridx==99){
        ridx=0;
    }
    ridx+=1;
    return(data);
}

void waitForByte(){
    while(idx==ridx){}
    ridx=idx;
}

void SPISend(uint8_t data){
    
# 94 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino" 3
   (*(volatile uint8_t *)((0x2E) + 0x20)) 
# 94 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino"
        = data;
    waitForByte();
}

bool verifyChecksum(uint8_t recv_buff[]){
    calculateChecksum(recv_buff);
    if(checksum1 == recv_buff[recv_buff[3]+4]){ //compare with checksum one
        if(checksum2 == recv_buff[recv_buff[3]+5]){ //compare with checksum two
            return true; //return true if checksums validate
        }
    }
    return false; //return false if either checksum fails
}

void calculateChecksum(uint8_t data_buff[]){
    int packetSum = 0;
    packetSum += data_buff[0]; //add header to checksum
    packetSum += data_buff[1]; //add controller id to checksum
    packetSum += data_buff[2]; //add controller command to checksum
    packetSum += data_buff[3]; //add data length to checksum
    for(int i=0;i<data_buff[3];i++){
        packetSum += data_buff[i+4]; //add data bytes to checksum
    }
    checksum1 = floor(packetSum / 256);
    checksum2 = packetSum % 256;
}

bool readSPIPacket(){
    spi_recv_buff[0] = 0;
    while(spi_recv_buff[0] != 255){
        spi_recv_buff[0] = readByte(); //header 255
    }
    spi_recv_buff[1] = readByte(); //ID
    spi_recv_buff[2] = readByte(); //Command
    spi_recv_buff[3] = readByte(); //Data length
    for(int i=0;i<spi_recv_buff[3];i++){
        spi_recv_buff[4+i] = readByte();
    }
    spi_recv_buff[spi_recv_buff[3]+4] = readByte(); //checksum 1
    spi_recv_buff[spi_recv_buff[3]+5] = readByte(); //checksum 2
    return(verifyChecksum(spi_recv_buff)); //return whether data was received succesfully
}

void sendSPIPacket(uint8_t send_buff[]){
    SPISend(send_buff[0]); //Send Header
    SPISend(send_buff[1]); //Send ID
    SPISend(send_buff[2]); //Send Command
    SPISend(send_buff[3]); //Send Data length
    for(int i=0;i<send_buff[3];i++){
        SPISend(send_buff[4+i]); //Send packet data
    }
    calculateChecksum(send_buff);
    SPISend(checksum1); //Send Checksum one
    SPISend(checksum2); //Send Checksum two
}

void setup(){
    pinMode(0, 0x1);
    pinMode(A2, 0x1);
    pinMode(A3, 0x1);
    pinMode(MISO,0x1);
    servo_1.attach(A4);
    servo_2.attach(A5);
    
# 157 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino" 3
   (*(volatile uint8_t *)((0x2C) + 0x20)) 
# 157 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino"
        |= 
# 157 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino" 3
           (1 << (6))
# 157 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino"
                   ;
    
# 158 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino" 3
   (*(volatile uint8_t *)((0x2C) + 0x20)) 
# 158 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino"
        &= ~(
# 158 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino" 3
             (1 << (4))
# 158 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino"
                      ); //Arduino is Slave
    
# 159 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino" 3
   (*(volatile uint8_t *)((0x2C) + 0x20)) 
# 159 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino"
        |= 
# 159 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino" 3
           (1 << (7))
# 159 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino"
                    ; //we not using SPI.attachInterrupt() why?
    LED(2);
    
# 161 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino" 3
   __asm__ __volatile__ ("sei" ::: "memory")
# 161 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino"
        ;
}

long convertToPWM(long angle, long minAngle, long maxAngle, long minPWM, long maxPWM){
  long valuePWM = map(angle,minAngle,maxAngle,minPWM,maxPWM);
  return valuePWM;
}

int ToDec(uint8_t lsb, uint8_t msb){
  int bigNumber = lsb + (msb * 255);

  return bigNumber;
}

void loop(){
    if(readSPIPacket()){
        spi_send_buff[0] = 255; //Set serial send header
        switch(spi_recv_buff[2]){
            case 3:
                spi_send_buff[1] = 0;
                spi_send_buff[2] = 3;
                spi_send_buff[3] = 1;
                spi_send_buff[4] = 1; //CARD TYPE FOR EMPIRE ID 1
                sendSPIPacket(spi_recv_buff);
                break;
            case 24:
                spi_send_buff[1] = 1;
                spi_send_buff[2] = 5;
                spi_send_buff[3] = 0;
                sendSPIPacket(spi_recv_buff);
                break;

            case 9:
                if(spi_recv_buff[4]==0){
                    servos[spi_recv_buff[4]].writeMicroseconds(convertToPWM(ToDec(spi_recv_buff[5], spi_recv_buff[6]),servo_1_min_angle,servo_1_max_angle,servo_1_min_microseconds,servo_1_max_microseconds));
                }
                if(spi_recv_buff[4]==1){
                    servos[spi_recv_buff[4]].writeMicroseconds(convertToPWM(ToDec(spi_recv_buff[5], spi_recv_buff[6]),servo_2_min_angle,servo_2_max_angle,servo_2_min_microseconds,servo_2_max_microseconds));
                }
                sendSPIPacket(spi_recv_buff);
                break;

            case 11:
                int servo_min_angle = ToDec(spi_recv_buff[5], spi_recv_buff[6]);
                int servo_max_angle = ToDec(spi_recv_buff[7], spi_recv_buff[8]);
                int servo_min_microseconds = ToDec(spi_recv_buff[9], spi_recv_buff[10]);
                int servo_max_microseconds = ToDec(spi_recv_buff[11], spi_recv_buff[12]);
                if(spi_recv_buff[4]==0){
                    servo_1_min_angle = servo_min_angle;
                    servo_1_max_angle = servo_max_angle;
                    servo_1_min_microseconds = servo_min_microseconds;
                    servo_1_max_microseconds = servo_max_microseconds;

                }
                if(spi_recv_buff[4]==1){
                    servo_2_min_angle = servo_min_angle;
                    servo_2_max_angle = servo_max_angle;
                    servo_2_min_microseconds = servo_min_microseconds;
                    servo_2_max_microseconds = servo_max_microseconds;
                }
                sendSPIPacket(spi_recv_buff);
                break;

            default:
                break;
        }
    }else{
        LED(1);
    }
}
