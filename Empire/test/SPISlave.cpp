#include "SPISlave.h"

SPISlave::SPISlave(){
    _destination_id = 0;
}

void SPISlave::begin(){
    pinMode(MISO,OUTPUT);
    pinMode(MOSI,INPUT);
    SPCR |= _BV(SPE);
    SPCR &= ~(_BV(MSTR)); //Arduino is Slave
    SPCR |= _BV(SPIE);      //we not using SPI.attachInterrupt() why?
    sei();
}

void SPISlave::update_data(byte data){
    // if(write){
    //     SPDR = interrupt_read_buff[x];
    //     x += 1;
    //     if(x==2){
    //         x = 0;
    //         r = 0;
    //         write = false;
    //         full_packet_ready = false;
    //     }
    // }
    // if(write){
    //     SPDR = interrupt_read_buff[x];
    //     x += 1;
    //     if(x==2){
    //         x = 0;
    //         r = 0;
    //         write = false;
    //     }
    // }
    
    if(!write){
        interrupt_read_buff[r] = SPDR;
        r+=1;
        if(r==sizeof(packet)-sizeof(recv_packet.data)){
            memcpy( &recv_packet, (char *) &interrupt_read_buff, sizeof(recv_packet)-sizeof(recv_packet.data));
        }
        if(r==sizeof(packet)-sizeof(recv_packet.data)+recv_packet.data_length){
            memcpy( &recv_packet.data, (char *) &interrupt_read_buff[sizeof(recv_packet)-sizeof(recv_packet.data)], recv_packet.data_length);
            full_packet_ready = true;
            write = true;
            r = 0;
        }
    }else{
        x+=1;
    }
    
    
    
}

void SPISlave::close(void(*end_CommsAbstract_handler)()){
    _data_length_errors = 0;
    _packets_recieved = 0;
    _checksum_errors = 0;
    _header_length_errors = 0;
    SPI.end();
    end_CommsAbstract_handler();
}

void SPISlave::send(byte destination_id){
    send_packet.header = recv_packet.header;
    send_packet.destination_id = destination_id;
    send_packet.sender_id = recv_packet.destination_id;
    send_packet.packet_id = recv_packet.packet_id;
    send_packet.command = recv_packet.command;
    send_packet.data_length = _send_starting_pos;
    send_packet.checksum = calc_checksum(send_packet);
    send_full_packet();
}

void SPISlave::check_for_data(void(*on_data_callback)(), void(*no_data_callback)()){
        if(full_packet_ready){
            on_data_callback();
            full_packet_ready = false;
        }
        no_data_callback();
        
}

void SPISlave::write_byte(byte data){
    SPDR = data;
    int current = x;
    while(x==current){}
}

void SPISlave::send_full_packet()
{
    _send_starting_pos = 0;
    byte* byte_array = reinterpret_cast<byte*>(&send_packet);
    for(int i=0; i<sizeof(send_packet)-sizeof(send_packet.data); i++){
        write_byte(byte_array[i]);
    }
    for(int i=0; i<send_packet.data_length; i++){
        write_byte(send_packet.data[i]);
    }
    write=false;
}


// void SPISlave::read_full_packet(){
//     byte byte_array[sizeof(packet)-sizeof(recv_packet.data)];
//     for(int i=0;i<sizeof(packet)-sizeof(recv_packet.data);i++){
//         byte_array[i] = read_byte();
//     }
//     memcpy( &recv_packet, byte_array, sizeof(packet)-sizeof(recv_packet.data));
//     for(int i=0;i<recv_packet.data_length;i++){
//         recv_packet.data[i] = read_byte();
//     }
//     ridx = idx;
    
// }