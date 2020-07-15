#include "SPIMaster.h"

SPIMaster::SPIMaster(){
    _destination_id = 0;
}

void SPIMaster::begin(){
    pinMode(MOSI, OUTPUT);
    pinMode(MISO, INPUT);
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV8);
}

void SPIMaster::close(void(*end_CommsAbstract_handler)()){
    _data_length_errors = 0;
    _packets_recieved = 0;
    _checksum_errors = 0;
    _header_length_errors = 0;
    SPI.end();
    end_CommsAbstract_handler();
}

void SPIMaster::send(byte destination_id){
    send_packet.checksum = calc_checksum(send_packet);
    digitalWrite(destination_id, LOW);
    send_full_packet();
    digitalWrite(destination_id, HIGH);
}

void SPIMaster::send_full_packet()
{
    _send_starting_pos = 0;
    byte* byte_array = reinterpret_cast<byte*>(&send_packet);
    for(int i=0; i<sizeof(packet)-sizeof(send_packet.data)+send_packet.data_length;i++){
        SPI.transfer(byte_array[i]);
        delayMicroseconds(100);
    }
}

void SPIMaster::read_full_packet(byte ss_pin){
    _starting_pos = 0;
    digitalWrite(ss_pin, LOW);
    int resp = 0 ;
    uint8_t buff[sizeof(packet)-sizeof(recv_packet.data)];
    while(resp==0){
        resp = SPI.transfer(0);
        delayMicroseconds(100);
    }
    buff[0] = resp;
    for(int i=1; i<sizeof(packet)-sizeof(recv_packet.data);i++){
        resp = SPI.transfer(0);
        buff[i] = resp;
        delayMicroseconds(100);
    }
    memcpy(&recv_packet, &buff, sizeof(packet)-sizeof(recv_packet.data));
    for(int i=0; i<2;i++){
        resp = SPI.transfer(0);
        buff[i] = resp;
        delayMicroseconds(100);
    }
    memcpy(&recv_packet.data, &buff, 2);
    digitalWrite(ss_pin, HIGH);
}