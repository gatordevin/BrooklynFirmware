# 1 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
# 2 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino" 2
# 12 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
//DEFINE ALL COMMAND BYTES




uint8_t ser_recv_buff[20];
uint8_t ser_send_buff[20];
uint8_t spi_recv_buff[20];
uint8_t spi_send_buff[20];

uint8_t checksum1 = 0;
uint8_t checksum2 = 0;
static uint8_t ss[] = {A0, A1, 0, 1, 10, 9, 12, 4};

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

uint8_t readByte(){
  while(!Serial.available()){} //wait until serial avaiable
  uint8_t data = Serial.read(); //read byte
  return(data);
}

bool verifyChecksum(uint8_t recv_buff[]){
    calculateChecksum(recv_buff);
    if(checksum1 == recv_buff[recv_buff[3]+4]){ //compare with checksum one
        if(checksum2 == recv_buff[recv_buff[3]+5]){ //compare with checksum two
            return true; //return true if checksums validate
        }
    }
    return false; //return false if either checksum fails
}

void calculateChecksum(uint8_t data_buff[]){
    int packetSum = 0;
    packetSum += data_buff[0]; //add header to checksum
    packetSum += data_buff[1]; //add controller id to checksum
    packetSum += data_buff[2]; //add controller command to checksum
    packetSum += data_buff[3]; //add data length to checksum
    for(int i=0;i<data_buff[3];i++){
        packetSum += data_buff[i+4]; //add data bytes to checksum
    }
    checksum1 = floor(packetSum / 256);
    checksum2 = packetSum % 256;
}

bool readSerialPacket(){
    ser_recv_buff[0] = 0;
    while(ser_recv_buff[0] != 255){
        ser_recv_buff[0] = readByte(); //header 255
    }
    ser_recv_buff[1] = readByte(); //controller ID
    ser_recv_buff[2] = readByte(); //controller command
    ser_recv_buff[3] = readByte(); //data length
    for(int i=0;i<ser_recv_buff[3];i++){
        ser_recv_buff[i+4] = readByte(); //data bytes
    }
    ser_recv_buff[ser_recv_buff[3]+4] = readByte(); //checksum 1
    ser_recv_buff[ser_recv_buff[3]+5] = readByte(); //checksum 2

    return(verifyChecksum(ser_recv_buff)); //return whether data was received succesfully
}

void sendSerialPacket(uint8_t send_buff[]){
    Serial.write(send_buff[0]); //send header 255
    Serial.write(send_buff[1]); //send ID
    Serial.write(send_buff[2]); //send command
    Serial.write(send_buff[3]); //send data length
    for(int i=0;i<send_buff[3];i++){
        Serial.write(send_buff[i+4]); //send data bytes
    }
    calculateChecksum(send_buff);
    Serial.write(checksum1); //send checksum 1
    Serial.write(checksum2); //send checksum 2
}

uint8_t SPISend(uint8_t SSpin, uint8_t data){
    digitalWrite(SSpin, 0x0);
    uint8_t resp = SPI.transfer(data);
    digitalWrite(SSpin, 0x1);
    delayMicroseconds(100);
    return resp;
}

bool SPIRecvPacket(uint8_t SSpin){
    delayMicroseconds(300);
    spi_recv_buff[0] = SPISend(SSpin,0); //read header
    spi_recv_buff[1] = SPISend(SSpin,0); //read id
    spi_recv_buff[2] = SPISend(SSpin,0); //read cmd
    spi_recv_buff[3] = SPISend(SSpin,0); //read data length
    for(int i=0;i<spi_recv_buff[3];i++){
        spi_recv_buff[4+i] = SPISend(SSpin,0); //read data
    }
    spi_recv_buff[spi_recv_buff[3]+4] = SPISend(SSpin,0);
    spi_recv_buff[spi_recv_buff[3]+5] = SPISend(SSpin,0);

    if(verifyChecksum(spi_recv_buff)==true){
        return true;
    }
    return false;
}

bool SPISendPacket(uint8_t SSpin){
    SPISend(SSpin,spi_send_buff[0]); //Header
    SPISend(SSpin,spi_send_buff[1]); //ID
    SPISend(SSpin,spi_send_buff[2]); //Command
    SPISend(SSpin,spi_send_buff[3]); //Data Length
    for(int i=0;i<spi_send_buff[3];i++){
        SPISend(SSpin,spi_send_buff[4+i]);
    }
    calculateChecksum(spi_send_buff);
    SPISend(SSpin, checksum1);
    SPISend(SSpin, checksum2);

    return(SPIRecvPacket(SSpin));
}

void CopySerToSPI(){
    for(int i=0;i<20;i++){
        spi_send_buff[i] = ser_recv_buff[i]; //copy data
    }
}

void CopySPIToSer(){
    for(int i=0;i<20;i++){
        ser_send_buff[i] = spi_recv_buff[i]; //copy data
    }
}

void setup(){
    Serial.begin(1000000);
    pinMode(7, 0x1);
    pinMode(11, 0x1);
    pinMode(A5, 0x1);
    for(int i=0;i<8;i++){
        pinMode(ss[i], 0x1);
        digitalWrite(ss[i], 0x1);
    }
    pinMode(MOSI, 0x1);
    SPI.begin();
    SPI.setClockDivider(0x05);

    LED(2);
}

void loop(){
    if(readSerialPacket()){
        ser_send_buff[0] = 255; //Set serial send header
        switch(ser_recv_buff[2]){
            case 72:
                ser_send_buff[1] = 0;
                ser_send_buff[2] = 72;
                ser_send_buff[3] = 0;
                sendSerialPacket(ser_send_buff);
                break;

            case 24: //In the case of an encoder request we copy over the request from the computer to the spi and send it to
                CopySerToSPI(); //the intended daughter card whcih was specificed in the packet
                if(SPISendPacket(ss[ser_recv_buff[1]-2])){ //The if statement then verifiwes the data was transferred properly by checking the checksum
                    CopySPIToSer(); //If there was transfer success we can decide what to do which in this case is relay the information from the daugfhter card to the computer
                    sendSerialPacket(ser_send_buff);
                }else{
                    LED(1); //If it failed we can do somethign different like specify the error message as a cehcksum error and the computer will decide wether it wants to ask for that data again
                    ser_send_buff[1] = 0; //In the case of a checksum error we most likely would if its a different error such as encoder being out of range or somethign liek that we can solve it before asking again
                    ser_send_buff[2] = 2; //You can manually set the send buffer by cahnging these three values which sepcifiy the destination the command and the length of data in the packet
                    ser_send_buff[3] = 0; //Destiantion for computer is 0 destination for brooklyn is 1 and all empire cards are 2-10
                    sendSerialPacket(ser_send_buff);
                }

                break;

            default:
                CopySerToSPI(); //the intended daughter card whcih was specificed in the packet
                if(SPISendPacket(ss[ser_recv_buff[1]-2])){ //The if statement then verifiwes the data was transferred properly by checking the checksum
                    CopySPIToSer(); //If there was transfer success we can decide what to do which in this case is relay the information from the daugfhter card to the computer
                    sendSerialPacket(ser_send_buff);
                }else{
                    LED(1); //If it failed we can do somethign different like specify the error message as a cehcksum error and the computer will decide wether it wants to ask for that data again
                    ser_send_buff[1] = 0; //In the case of a checksum error we most likely would if its a different error such as encoder being out of range or somethign liek that we can solve it before asking again
                    ser_send_buff[2] = 2; //You can manually set the send buffer by cahnging these three values which sepcifiy the destination the command and the length of data in the packet
                    ser_send_buff[3] = 0; //Destiantion for computer is 0 destination for brooklyn is 1 and all empire cards are 2-10
                    sendSerialPacket(ser_send_buff);
                }

                break;
        }
    }else{

    }
}
