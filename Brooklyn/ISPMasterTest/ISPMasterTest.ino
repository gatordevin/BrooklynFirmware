#include <SPI.h>
#define SS  A1

#define RED 1
#define BLUE 2
#define GREEN 3
#define OFF 4

#define red_pin 7
#define blue_pin 11
#define green_pin A5

static uint8_t ss[] = {A1, A0, 0, 1, 10, 9, 12, 4};

uint8_t header;
uint8_t id;
uint8_t cmd;
uint8_t datalen;
uint8_t data[10];
uint8_t ck1;
uint8_t ck2;

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

bool calcChecksumSerial(){
    int packetSum = 0;
    packetSum += header;
    packetSum += cmd;
    packetSum += id;
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

bool SPISendpacket(uint8_t SSpin){
    int packetSum = 0;
    SPISend(SSpin,255);
    packetSum+=255;
    SPISend(SSpin,cmd);
    packetSum+=cmd;
    SPISend(SSpin,datalen);
    packetSum+=datalen;
    for(int i=0;i<datalen;i++){
        packetSum+=data[i];
        SPISend(SSpin,data[i]);
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
        
        if(calcChecksum()==true){
            return true;
        }
    return false;
    }
}

void SerialSendpacket(uint8_t data[]){
    int packetSum = 0;
    Serial.write(255);
    packetSum+=255;
    Serial.write(data[0]);
    packetSum+=data[0];
    Serial.write(data[1]);
    packetSum+=data[1];
    for(int i=0;i<data[1];i++){
        packetSum+=data[2+i];
        Serial.write(data[2+i]);
    }
    ck1 = floor(packetSum / 256);
    ck2 = packetSum % 256;
    Serial.write(ck1);
    Serial.write(ck2);
}

void setup(){
    Serial.begin(1000000);
    pinMode(red_pin, OUTPUT);
    pinMode(blue_pin, OUTPUT);
    pinMode(green_pin, OUTPUT);
    pinMode(SS, OUTPUT);
    pinMode(MOSI, OUTPUT);
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV16);
    digitalWrite(SS, HIGH);
    LED(RED);
}

uint8_t readData(){
  while(!Serial.available()){}
  uint8_t data = Serial.read();
  return(data);
}

void analyzeResponse(uint8_t cmd_resp){

}

void loop(void){
    LED(RED);
    header = readData();
    switch (header){
        case 140:
            Serial.write(140);
            break;
        case 255:
            id = readData();
            cmd = readData();
            datalen = readData();
            for(int i=0;i<datalen;i++){
                data[i] = readData();
            }
            ck1 = readData();
            ck2 = readData();
            if(calcChecksumSerial()){
                uint8_t serpacket[] = {1,1,3};
                if(SPISendpacket(ss[id])){
                    switch (cmd){
                        case 2:
                            LED(GREEN);
                            SerialSendpacket(serpacket);
                            break;
                        }
                }
                
            }
            break;
        case 72:
            Serial.write(72);
            break;
    }
}
