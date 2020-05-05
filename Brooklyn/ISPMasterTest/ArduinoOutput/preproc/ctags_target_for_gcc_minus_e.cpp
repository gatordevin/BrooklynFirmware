# 1 "/home/techgarage/BrooklynFirmware/Brooklyn/ISPMasterTest/ISPMasterTest.ino"
# 2 "/home/techgarage/BrooklynFirmware/Brooklyn/ISPMasterTest/ISPMasterTest.ino" 2
# 13 "/home/techgarage/BrooklynFirmware/Brooklyn/ISPMasterTest/ISPMasterTest.ino"
void LED(uint8_t color){
    switch (color){
        case 1:
            digitalWrite(7, 0x0);
            digitalWrite(11, 0x1);
            digitalWrite(A5, 0x1);
            break;
        case 2:
            digitalWrite(7, 0x1);
            digitalWrite(11, 0x0);
            digitalWrite(A5, 0x1);
            break;
        case 3:
            digitalWrite(7, 0x1);
            digitalWrite(11, 0x1);
            digitalWrite(A5, 0x0);
            break;
        case 4:
            digitalWrite(7, 0x1);
            digitalWrite(11, 0x1);
            digitalWrite(A5, 0x1);
            break;
    }
}

uint8_t SPISend(uint8_t SSpin, uint8_t data){
    digitalWrite(SSpin, 0x0);
    SPI.transfer(data);
    delayMicroseconds(100);
    uint8_t resp = SPI.transfer(data);
    digitalWrite(SSpin, 0x1);
    return resp;
}

uint8_t SPISendPacket(uint8_t SSpin, uint8_t data[], int arraySize){
    digitalWrite(SSpin, 0x0);
    for(int i=0; i < arraySize; i++){
        SPI.transfer(data[i]);
    }
    delayMicroseconds(200);
    uint8_t resp = SPI.transfer(0);
    digitalWrite(SSpin, 0x1);
    return resp;
}

void setup(){
    Serial.begin(9600);
    pinMode(7, 0x1);
    pinMode(11, 0x1);
    pinMode(A5, 0x1);
    pinMode(A1, 0x1);
    digitalWrite(A1, 0x1);
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
    SPISendPacket(A1,data,5);
}
