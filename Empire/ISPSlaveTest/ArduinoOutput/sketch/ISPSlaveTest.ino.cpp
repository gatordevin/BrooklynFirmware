#include <Arduino.h>
#line 1 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino"
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
uint8_t cmdPacket[2];
uint8_t datapacket[100];

#define CID_GETSPEED    0x00  // GET MOTOR SPEED

#define RID_CHECKSUMERROR    0x00  // CHECKSUM ERROR
#define RID_NORESPONSE    0x01  // NO RESP
#define RID_MOTORSPEED    2  // GET MOTOR SPEED

#line 31 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino"
void LED(uint8_t color);
#line 66 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino"
uint8_t readData();
#line 76 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino"
bool receivePacket();
#line 93 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino"
bool calcChecksum();
#line 109 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino"
void SPISend(uint8_t data);
#line 114 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino"
void SPISendPacket(uint8_t command[], uint8_t data[]);
#line 132 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino"
void setup();
#line 145 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino"
void loop(void);
#line 31 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest/ISPSlaveTest.ino"
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
    SPDR=20;
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
    pinMode(red_pin, OUTPUT);
    pinMode(blue_pin, OUTPUT);
    pinMode(green_pin, OUTPUT);
    pinMode(MISO,OUTPUT);
    SPCR |= _BV(SPE);
    SPCR &= ~(_BV(MSTR)); //Arduino is Slave
    SPDR = 0x67;  //test value
    SPCR |= _BV(SPIE);      //we not using SPI.attachInterrupt() why?
    sei();
    LED(RED);
}

void loop(void){
    
    if(receivePacket()){
        switch(cmd){
            case CID_GETSPEED:
                LED(GREEN);
                cmdPacket[0] = 2;
                cmdPacket[1] = 1;
                datapacket[0] = 139;
                SPISendPacket(cmdPacket,datapacket);
                break;
        }
    }
}

