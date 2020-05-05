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

#define CID_GETSPEED    0x00  // GET MOTOR SPEED

#line 25 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino"
void LED(uint8_t color);
#line 59 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino"
uint8_t readData();
#line 69 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino"
bool calcChecksum();
#line 85 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino"
void setup();
#line 95 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino"
void loop(void);
#line 25 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino"
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

void setup(){
    pinMode(red_pin, OUTPUT);
    pinMode(blue_pin, OUTPUT);
    pinMode(green_pin, OUTPUT);
    pinMode(MISO,OUTPUT);
    SPCR |= _BV(SPE); 
    SPI.attachInterrupt();
    LED(BLUE);
}

void loop(void){
    header = readData();
    switch(header){
        case (255):
            cmd = readData();
            datalen = readData();
            for(int i=0;i<datalen;i++){
                data[i] = readData();
            }
            ck1 = readData();
            ck2 = readData();
            if(calcChecksum()){
                switch(cmd){
                    case CID_GETSPEED:
                        LED(GREEN);
                        break;
                }
            }else{
                LED(RED);
            }
            break;
    }
}
