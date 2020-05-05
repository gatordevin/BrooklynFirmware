#include <Arduino.h>
#line 1 "/home/techgarage/BrooklynFirmware/Brooklyn/ISPMasterTest/ISPMasterTest.ino"
#include <SPI.h>
#define SS  A1

#define RED 1
#define BLUE 2
#define GREEN 3
#define OFF 4

#define red_pin 7
#define blue_pin 11
#define green_pin A5

#line 13 "/home/techgarage/BrooklynFirmware/Brooklyn/ISPMasterTest/ISPMasterTest.ino"
void LED(uint8_t color);
#line 38 "/home/techgarage/BrooklynFirmware/Brooklyn/ISPMasterTest/ISPMasterTest.ino"
uint8_t SPISend(uint8_t SSpin, uint8_t data);
#line 47 "/home/techgarage/BrooklynFirmware/Brooklyn/ISPMasterTest/ISPMasterTest.ino"
uint8_t SPISendPacket(uint8_t SSpin, uint8_t data[], int arraySize);
#line 58 "/home/techgarage/BrooklynFirmware/Brooklyn/ISPMasterTest/ISPMasterTest.ino"
void setup();
#line 68 "/home/techgarage/BrooklynFirmware/Brooklyn/ISPMasterTest/ISPMasterTest.ino"
void loop(void);
#line 13 "/home/techgarage/BrooklynFirmware/Brooklyn/ISPMasterTest/ISPMasterTest.ino"
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

uint8_t SPISend(uint8_t SSpin, uint8_t data){
    digitalWrite(SSpin, LOW);
    SPI.transfer(data);
    delayMicroseconds(100);
    uint8_t resp = SPI.transfer(data);
    digitalWrite(SSpin, HIGH);
    return resp;
}

uint8_t SPISendPacket(uint8_t SSpin, uint8_t data[], int arraySize){
    digitalWrite(SSpin, LOW);
    for(int i=0; i < arraySize; i++){
        SPI.transfer(data[i]);
    }
    delayMicroseconds(200);
    uint8_t resp = SPI.transfer(0);
    digitalWrite(SSpin, HIGH);
    return resp;
}

void setup(){
    Serial.begin(9600);
    pinMode(red_pin, OUTPUT);
    pinMode(blue_pin, OUTPUT);
    pinMode(green_pin, OUTPUT);
    pinMode(SS, OUTPUT);
    digitalWrite(SS, HIGH);
    SPI.begin ();
}

void loop(void){
    uint8_t data[5];
    data[0] = 255; //Header
    data[1] = 7; //Command
    data[2] = 0; //Data Length
    int packetSum = 262;
    data[3] = floor(packetSum / 256);
    data[4] = packetSum % 256;
    SPISendPacket(SS,data,5);
}
