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
void SPISend(uint8_t SSpin, uint8_t data);
#line 45 "/home/techgarage/BrooklynFirmware/Brooklyn/ISPMasterTest/ISPMasterTest.ino"
void SPISendpacket(uint8_t SSpin, uint8_t data[]);
#line 63 "/home/techgarage/BrooklynFirmware/Brooklyn/ISPMasterTest/ISPMasterTest.ino"
void setup();
#line 73 "/home/techgarage/BrooklynFirmware/Brooklyn/ISPMasterTest/ISPMasterTest.ino"
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

void SPISend(uint8_t SSpin, uint8_t data){
    digitalWrite(SSpin, LOW);
    uint8_t resp = SPI.transfer(data);
    Serial.print(resp);
    digitalWrite(SSpin, HIGH);
}

void SPISendpacket(uint8_t SSpin, uint8_t data[]){
    int packetSum = 0;
    SPISend(SSpin,data[0]);
    packetSum+=data[0];
    SPISend(SSpin,data[1]);
    packetSum+=data[1];
    SPISend(SSpin,data[2]);
    packetSum+=data[2];
    for(int i=0;i<data[2];i++){
        packetSum+=data[3+i];
        SPISend(SSpin,data[3+i]);
    }
    uint8_t ck1 = floor(packetSum / 256);
    uint8_t ck2 = packetSum % 256;
    SPISend(SSpin, ck1);
    SPISend(SSpin, ck2);
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
    uint8_t data[] = {255,0,0};
    SPISendpacket(SS,data);
    //SPISend(SS,255);
    delay(500);
}
