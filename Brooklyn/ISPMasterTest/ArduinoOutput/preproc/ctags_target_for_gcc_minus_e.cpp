# 1 "/home/techgarage/BrooklynFirmware/Brooklyn/ISPMasterTest/ISPMasterTest.ino"
# 2 "/home/techgarage/BrooklynFirmware/Brooklyn/ISPMasterTest/ISPMasterTest.ino" 2
# 13 "/home/techgarage/BrooklynFirmware/Brooklyn/ISPMasterTest/ISPMasterTest.ino"
uint8_t header;
uint8_t cmd;
uint8_t datalen;
uint8_t data[10];
uint8_t ck1;
uint8_t ck2;
uint8_t id;





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

bool calcChecksum(){
    int packetSum = 0;
    packetSum += header;
    packetSum += id;
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
    digitalWrite(SSpin, 0x0);
    uint8_t resp = SPI.transfer(data);
    Serial.print(resp);
    digitalWrite(SSpin, 0x1);
    return resp;
}

bool receivePacket(uint8_t SSpin){
    header = SPISend(SSpin,0);
    if(header==255){
        cmd = SPISend(SSpin,0);
        datalen = SPISend(SSpin,0);
        for(int i=0;i<datalen;i++){
            data[i] = SPISend(SSpin,0);
        }
        ck1 = SPISend(SSpin,0);
        ck2 = SPISend(SSpin,0);
        if(calcChecksum()){
            return true;
        }
    }
    return false;
}

uint8_t SPISendpacket(uint8_t SSpin, uint8_t data[]){
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

    if(receivePacket(SSpin)){
        LED(2);
    }
}

void handleResponse(){

}

void setup(){
    Serial.begin(1000000);
    pinMode(7, 0x1);
    pinMode(11, 0x1);
    pinMode(A5, 0x1);
    pinMode(A1, 0x1);
    digitalWrite(A1, 0x1);
    SPI.begin ();
    LED(1);
}

void loop(void){
    header = Serial.read();
    if(header==255){
        id = Serial.read();
        cmd = Serial.read();
        datalen = Serial.read();
        for(int i=0;i<datalen;i++){
            data[i] = Serial.read();
        }
        ck1 = Serial.read();
        ck2 = Serial.read();
        if(calcChecksum()){
            LED(3);
        }
    }


    // if(SPISendpacket(SS,data2)==2){
    //     LED(GREEN);
    // }
    // delay(100);
}
