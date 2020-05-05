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

void SPISend(uint8_t SSpin, uint8_t data){
    digitalWrite(SSpin, 0x0);
    uint8_t resp = SPI.transfer(data);
    Serial.print(resp);
    digitalWrite(SSpin, 0x1);
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
    pinMode(7, 0x1);
    pinMode(11, 0x1);
    pinMode(A5, 0x1);
    pinMode(A1, 0x1);
    digitalWrite(A1, 0x1);
    SPI.begin ();
}

void loop(void){
    uint8_t data[] = {255,0,0};
    SPISendpacket(A1,data);
    //SPISend(SS,255);
    delay(500);
}
