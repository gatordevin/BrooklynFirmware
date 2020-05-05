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

uint8_t header;
uint8_t cmd;
uint8_t datalen;
uint8_t data[10];
uint8_t ck1;
uint8_t ck2;

#line 20 "/home/techgarage/BrooklynFirmware/Brooklyn/ISPMasterTest/ISPMasterTest.ino"
void LED(uint8_t color);
#line 45 "/home/techgarage/BrooklynFirmware/Brooklyn/ISPMasterTest/ISPMasterTest.ino"
bool calcChecksum();
#line 61 "/home/techgarage/BrooklynFirmware/Brooklyn/ISPMasterTest/ISPMasterTest.ino"
uint8_t SPISend(uint8_t SSpin, uint8_t data);
#line 69 "/home/techgarage/BrooklynFirmware/Brooklyn/ISPMasterTest/ISPMasterTest.ino"
bool SPISendpacket(uint8_t SSpin, uint8_t data[]);
#line 103 "/home/techgarage/BrooklynFirmware/Brooklyn/ISPMasterTest/ISPMasterTest.ino"
void setup();
#line 114 "/home/techgarage/BrooklynFirmware/Brooklyn/ISPMasterTest/ISPMasterTest.ino"
void loop(void);
#line 20 "/home/techgarage/BrooklynFirmware/Brooklyn/ISPMasterTest/ISPMasterTest.ino"
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

uint8_t SPISend(uint8_t SSpin, uint8_t data){
    digitalWrite(SSpin, LOW);
    uint8_t resp = SPI.transfer(data);
    Serial.print(resp);
    digitalWrite(SSpin, HIGH);
    return resp;
}

bool SPISendpacket(uint8_t SSpin, uint8_t data[]){
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
    ck1 = floor(packetSum / 256);
    ck2 = packetSum % 256;
    SPISend(SSpin, ck1);
    SPISend(SSpin, ck2);
    delayMicroseconds(200);
    header = SPISend(SSpin,0);
    if(header==255){
        cmd = SPISend(SSpin,0);
        datalen = SPISend(SSpin,0);
        for(int i=0;i<datalen;i++){
            data[i] = SPISend(SSpin,0);
            packetSum += data[i];
        }
        ck1 = SPISend(SSpin,0);
        ck2 = SPISend(SSpin,0);
        if(calcChecksum){
            return true;
        }
    return false;
    }
}

void setup(){
    Serial.begin(9600);
    pinMode(red_pin, OUTPUT);
    pinMode(blue_pin, OUTPUT);
    pinMode(green_pin, OUTPUT);
    pinMode(SS, OUTPUT);
    digitalWrite(SS, HIGH);
    SPI.begin ();
    LED(RED);
}

void loop(void){
    uint8_t data[] = {255,0,0};
    if(SPISendpacket(SS,data)){
        LED(GREEN);
    }
    delay(100);
}
