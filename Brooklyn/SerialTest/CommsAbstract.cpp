#include "CommsAbstract.h"

CommsAbstract::CommsAbstract()
{
    _packets_recieved = 0;
    _header_length_errors = 0;
    _checksum_errors = 0;
    _data_length_errors = 0;
    _watchdog_errors = 0;
    _starting_pos = 0;
    _send_starting_pos = 0;
}



float CommsAbstract::read_float()
{
    four_byte_converter.b[0] = recv_packet.data[0+_starting_pos];
    four_byte_converter.b[1] = recv_packet.data[1+_starting_pos];
    four_byte_converter.b[2] = recv_packet.data[2+_starting_pos];
    four_byte_converter.b[3] = recv_packet.data[3+_starting_pos];
    _starting_pos += 4;
    return four_byte_converter.fval;
}

unsigned long CommsAbstract::read_unsigned_int(){
    four_byte_converter.b[0] = recv_packet.data[0+_starting_pos];
    four_byte_converter.b[1] = recv_packet.data[1+_starting_pos];
    four_byte_converter.b[2] = recv_packet.data[2+_starting_pos];
    four_byte_converter.b[3] = recv_packet.data[3+_starting_pos];
    _starting_pos += 4;
    return four_byte_converter.uival;
}

long CommsAbstract::read_int(){
    four_byte_converter.b[0] = recv_packet.data[0+_starting_pos];
    four_byte_converter.b[1] = recv_packet.data[1+_starting_pos];
    four_byte_converter.b[2] = recv_packet.data[2+_starting_pos];
    four_byte_converter.b[3] = recv_packet.data[3+_starting_pos];
    _starting_pos += 4;
    return four_byte_converter.ival;
}

short CommsAbstract::read_short(){
    two_byte_converter.b[0] = recv_packet.data[0+_starting_pos];
    two_byte_converter.b[1] = recv_packet.data[1+_starting_pos];
    _starting_pos += 2;
    return two_byte_converter.sval;
}

unsigned short CommsAbstract::read_unsigned_short(){
    two_byte_converter.b[0] = recv_packet.data[0+_starting_pos];
    two_byte_converter.b[1] = recv_packet.data[1+_starting_pos];
    _starting_pos += 2;
    return two_byte_converter.usval;
}

long long CommsAbstract::read_long_long(){
    eight_byte_converter.b[0] = recv_packet.data[0+_starting_pos];
    eight_byte_converter.b[1] = recv_packet.data[1+_starting_pos];
    eight_byte_converter.b[2] = recv_packet.data[2+_starting_pos];
    eight_byte_converter.b[3] = recv_packet.data[3+_starting_pos];
    eight_byte_converter.b[4] = recv_packet.data[4+_starting_pos];
    eight_byte_converter.b[5] = recv_packet.data[5+_starting_pos];
    eight_byte_converter.b[6] = recv_packet.data[6+_starting_pos];
    eight_byte_converter.b[7] = recv_packet.data[7+_starting_pos];
    _starting_pos += 8;
    return eight_byte_converter.lval;
}

unsigned long long CommsAbstract::read_unsigned_long_long(){
    eight_byte_converter.b[0] = recv_packet.data[0+_starting_pos];
    eight_byte_converter.b[1] = recv_packet.data[1+_starting_pos];
    eight_byte_converter.b[2] = recv_packet.data[2+_starting_pos];
    eight_byte_converter.b[3] = recv_packet.data[3+_starting_pos];
    eight_byte_converter.b[4] = recv_packet.data[4+_starting_pos];
    eight_byte_converter.b[5] = recv_packet.data[5+_starting_pos];
    eight_byte_converter.b[6] = recv_packet.data[6+_starting_pos];
    eight_byte_converter.b[7] = recv_packet.data[7+_starting_pos];
    _starting_pos += 8;
    return eight_byte_converter.ulval;
}

void CommsAbstract::send_int(signed long data){
    four_byte_converter.ival = data;
    send_packet.data[0+_send_starting_pos] = four_byte_converter.b[0];
    send_packet.data[1+_send_starting_pos] = four_byte_converter.b[1];
    send_packet.data[2+_send_starting_pos] = four_byte_converter.b[2];
    send_packet.data[3+_send_starting_pos] = four_byte_converter.b[3];
    _send_starting_pos += 4;
}

void CommsAbstract::send_unsigned_int(unsigned long data){
    four_byte_converter.uival = data;
    send_packet.data[0+_send_starting_pos] = four_byte_converter.b[0];
    send_packet.data[1+_send_starting_pos] = four_byte_converter.b[1];
    send_packet.data[2+_send_starting_pos] = four_byte_converter.b[2];
    send_packet.data[3+_send_starting_pos] = four_byte_converter.b[3];
    _send_starting_pos += 4;
}

void CommsAbstract::add(float data){
    four_byte_converter.fval = data;
    send_packet.data[0+_send_starting_pos] = four_byte_converter.b[0];
    send_packet.data[1+_send_starting_pos] = four_byte_converter.b[1];
    send_packet.data[2+_send_starting_pos] = four_byte_converter.b[2];
    send_packet.data[3+_send_starting_pos] = four_byte_converter.b[3];
    _send_starting_pos += 4;
}
void CommsAbstract::add(short data){
    two_byte_converter.sval = data;
    send_packet.data[0+_send_starting_pos] = two_byte_converter.b[0];
    send_packet.data[1+_send_starting_pos] = two_byte_converter.b[1];
    _send_starting_pos += 2;
}
void CommsAbstract::add(unsigned long data){
    four_byte_converter.uival = data;
    send_packet.data[0+_send_starting_pos] = four_byte_converter.b[0];
    send_packet.data[1+_send_starting_pos] = four_byte_converter.b[1];
    send_packet.data[2+_send_starting_pos] = four_byte_converter.b[2];
    send_packet.data[3+_send_starting_pos] = four_byte_converter.b[3];
    _send_starting_pos += 4;
}

void CommsAbstract::add(long data){
    four_byte_converter.ival = data;
    send_packet.data[0+_send_starting_pos] = four_byte_converter.b[0];
    send_packet.data[1+_send_starting_pos] = four_byte_converter.b[1];
    send_packet.data[2+_send_starting_pos] = four_byte_converter.b[2];
    send_packet.data[3+_send_starting_pos] = four_byte_converter.b[3];
    _send_starting_pos += 4;
}

void CommsAbstract::add(unsigned short data){
    two_byte_converter.usval = data;
    send_packet.data[0+_send_starting_pos] = two_byte_converter.b[0];
    send_packet.data[1+_send_starting_pos] = two_byte_converter.b[1];
    _send_starting_pos += 2;
}
void CommsAbstract::add(long long data){
    eight_byte_converter.lval = data;
    send_packet.data[0+_send_starting_pos] = eight_byte_converter.b[0];
    send_packet.data[1+_send_starting_pos] = eight_byte_converter.b[1];
    send_packet.data[2+_send_starting_pos] = eight_byte_converter.b[2];
    send_packet.data[3+_send_starting_pos] = eight_byte_converter.b[3];
    send_packet.data[4+_send_starting_pos] = eight_byte_converter.b[4];
    send_packet.data[5+_send_starting_pos] = eight_byte_converter.b[5];
    send_packet.data[6+_send_starting_pos] = eight_byte_converter.b[6];
    send_packet.data[7+_send_starting_pos] = eight_byte_converter.b[7];
    _send_starting_pos += 8;
}
void CommsAbstract::add(unsigned long long data){
    eight_byte_converter.ulval = data;
    send_packet.data[0+_send_starting_pos] = eight_byte_converter.b[0];
    send_packet.data[1+_send_starting_pos] = eight_byte_converter.b[1];
    send_packet.data[2+_send_starting_pos] = eight_byte_converter.b[2];
    send_packet.data[3+_send_starting_pos] = eight_byte_converter.b[3];
    send_packet.data[4+_send_starting_pos] = eight_byte_converter.b[4];
    send_packet.data[5+_send_starting_pos] = eight_byte_converter.b[5];
    send_packet.data[6+_send_starting_pos] = eight_byte_converter.b[6];
    send_packet.data[7+_send_starting_pos] = eight_byte_converter.b[7];
    _send_starting_pos += 8;
}

void CommsAbstract::send(int destination_id){
    send_packet.header = 255;
    send_packet.destination_id = destination_id;
    send_packet.sender_id = 1;
    send_packet.packet_id = recv_packet.packet_id;
    send_packet.command = 7;
    send_packet.data_length = _send_starting_pos;
    send_packet.checksum = calc_checksum(send_packet);
    send_full_packet();
}

signed long CommsAbstract::calc_checksum(packet data_packet){
    signed long check = 0;
    check += data_packet.header;
    check += data_packet.destination_id;
    check += data_packet.sender_id;
    check += data_packet.packet_id;
    check += data_packet.command;
    check += data_packet.data_length;
    for(int i=0; i<data_packet.data_length; i++){
        check += data_packet.data[i];
    }
    return check;
}

void CommsAbstract::close(void(*end_CommsAbstract_handler)()){
    _data_length_errors = 0;
    _packets_recieved = 0;
    _checksum_errors = 0;
    _header_length_errors = 0;
    end_CommsAbstract_handler();
}