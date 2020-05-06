#include <Arduino.h>
#line 1 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino"
#include <SPI.h>

#define RED 1
#define BLUE 2
#define GREEN 3
#define OFF 4

#define red_pin 0
#define blue_pin A2
#define green_pin A3

volatile uint8_t buffer[100];
volatile int idx = 0;
volatile int ridx = 0;

uint8_t header;
uint8_t cmd;
uint8_t datalen;
uint8_t data[10];
uint8_t ck1;
uint8_t ck2;

#define CID_GETSPEED    0  // GET MOTOR SPEED

#define RID_PACKETRROR 0 //PACKET CHECKSUM ERROR
#define RID_EMPTYREPLY 1 //SUCCESS EMPTY REPLY
#define RID_ENCODERSPEED 2 //SUCCESS RETURN ENCODER SPEED

#line 29 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino"
void LED(uint8_t color);
#line 63 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino"
uint8_t readData();
#line 73 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino"
bool receivePacket();
#line 90 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino"
bool calcChecksum();
#line 106 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino"
void SPISend(uint8_t data);
#line 111 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino"
void SPISendPacket(uint8_t data[]);
#line 129 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino"
void setup();
#line 139 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino"
void loop(void);
#line 29 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino"
void LED(uint8_t color){
    switch (color){
        case RED:
            digitalWrite(red_pin, LOW);
            digitalWrite(blue_pin, HIGH);
            digitalWrite(green_pin, HIGH);
            break;
        case BLUE:
            digitalWrite(red_pin, HIGH);
            digitalWrite(blue_pin, LOW);
            digitalWrite(green_pin, HIGH);
            break;
        case GREEN:
            digitalWrite(red_pin, HIGH);
            digitalWrite(blue_pin, HIGH);
            digitalWrite(green_pin, LOW);
            break;
        case OFF:
            digitalWrite(red_pin, HIGH);
            digitalWrite(blue_pin, HIGH);
            digitalWrite(green_pin, HIGH);
            break;
    }
}

ISR (SPI_STC_vect)
{
    buffer[idx] = SPDR;
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
    SPDR = data;
    readData();
}

void SPISendPacket(uint8_t data[]){
    int packetSum = 0;
    SPISend(255);
    packetSum+=255;
    SPISend(data[0]);
    packetSum+=data[0];
    SPISend(data[1]);
    packetSum+=data[1];
    for(int i=0;i<data[1];i++){
        packetSum+=data[2+i];
        SPISend(data[2+i]);
    }
    ck1 = floor(packetSum / 256);
    ck2 = packetSum % 256;
    SPISend(ck1);
    SPISend(ck2);
}

void setup(){
    pinMode(red_pin, OUTPUT);
    pinMode(blue_pin, OUTPUT);
    pinMode(green_pin, OUTPUT);
    pinMode(MISO,OUTPUT);
    SPCR |= _BV(SPE); 
    SPI.attachInterrupt();
    LED(RED);
}

void loop(void){
    uint8_t data[] = {RID_ENCODERSPEED,1,72};
    if(receivePacket()){
        switch(cmd){
            case CID_GETSPEED:
                LED(GREEN);
                SPISendPacket(data);
                break;
        }
    }
}
