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
volatile int i = 0;
#line 14 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino"
void LED(uint8_t color);
#line 45 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino"
bool checkSumCalc(int packetSum);
#line 60 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino"
void checkpacket(uint8_t buff[]);
#line 79 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino"
void setup();
#line 89 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino"
void loop(void);
#line 14 "/home/techgarage/BrooklynFirmware/Empire/ISPSlaveTest.ino"
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
    buffer[i] = SPDR;
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
                    SPDR=20;
                }
                break;
        }
    }
    i=0;
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
    checkpacket(buffer);
    LED(RED);
}
