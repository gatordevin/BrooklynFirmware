#include <SPI.h>
#include <Servo.h>

#define RED 1
#define BLUE 2
#define GREEN 3
#define OFF 4

#define red_pin 0
#define blue_pin A2
#define green_pin A3

#define servo_1_pin A4
#define servo_2_pin A5

Servo servo_1;
Servo servo_2;
Servo servos[2] = {servo_1,servo_2};

volatile uint8_t interrupt_buff[100];
uint8_t spi_recv_buff[20];
uint8_t spi_send_buff[20];
volatile int idx = 0;
volatile int ridx = 0;

uint8_t checksum1 = 0;
uint8_t checksum2 = 0;

#define CMD_GET_ENCODER 24
#define CMD_SET_PWM 9

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
    interrupt_buff[idx] = SPDR;
    if(idx==99){
        idx=0;
    }
    idx+=1;
}

uint8_t readByte(){
    while(idx==ridx){}
    uint8_t data = interrupt_buff[ridx];
    if(ridx==99){
        ridx=0;
    }
    ridx+=1;
    return(data);
}

void waitForByte(){
    while(idx==ridx){}
    ridx=idx;
}

void SPISend(uint8_t data){
    SPDR = data;
    waitForByte();
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

bool readSPIPacket(){
    spi_recv_buff[0] = 0;
    while(spi_recv_buff[0] != 255){
        spi_recv_buff[0] = readByte(); //header 255
    }
    spi_recv_buff[1] = readByte(); //ID
    spi_recv_buff[2] = readByte(); //Command
    spi_recv_buff[3] = readByte(); //Data length
    for(int i=0;i<spi_recv_buff[3];i++){
        spi_recv_buff[4+i] = readByte();
    }
    spi_recv_buff[spi_recv_buff[3]+4] = readByte(); //checksum 1
    spi_recv_buff[spi_recv_buff[3]+5] = readByte(); //checksum 2
    
    return(verifyChecksum(spi_recv_buff)); //return whether data was received succesfully
}

void sendSPIPacket(uint8_t send_buff[]){
    SPISend(send_buff[0]); //Send Header
    SPISend(send_buff[1]); //Send ID
    SPISend(send_buff[2]); //Send Command
    SPISend(send_buff[3]); //Send Data length
    for(int i=0;i<send_buff[3];i++){
        SPISend(send_buff[4+i]); //Send packet data
    }
    calculateChecksum(send_buff);
    SPISend(checksum1); //Send Checksum one
    SPISend(checksum2); //Send Checksum two
}

void setup(){
    pinMode(red_pin, OUTPUT);
    pinMode(blue_pin, OUTPUT);
    pinMode(green_pin, OUTPUT);
    pinMode(MISO,OUTPUT);
    servo_1.attach(servo_1_pin);
    servo_2.attach(servo_2_pin);
    SPCR |= _BV(SPE);
    SPCR &= ~(_BV(MSTR)); //Arduino is Slave
    SPCR |= _BV(SPIE);      //we not using SPI.attachInterrupt() why?
    LED(BLUE);
    sei();
    servo_1.write(0);
}

void loop(){
    if(readSPIPacket()){
        spi_send_buff[0] = 255; //Set serial send header
        switch(spi_recv_buff[2]){
            case CMD_GET_ENCODER:
                LED(GREEN);
                spi_send_buff[1] = 1;
                spi_send_buff[2] = 5;
                spi_send_buff[3] = 0;
                sendSPIPacket(spi_recv_buff);
                break;

            case CMD_SET_PWM:
                LED(GREEN);
                servos[spi_recv_buff[4]].write(spi_recv_buff[5]);
                sendSPIPacket(spi_recv_buff);
                break;
            
            default:
                break;
        }
    }else{
        LED(RED);
    }
}
