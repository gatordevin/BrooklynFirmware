#include "Comms.h"

Comms::Comms(unsigned int baud, short watchdog_timer)
{
    _baud = baud;
    _watchdog_timer = watchdog_timer;
    _packets_recieved = 0;
    _header_length_errors = 0;
    _checksum_errors = 0;
    _data_length_errors = 0;
    _watchdog_errors = 0;
    _starting_pos = 0;
    _send_starting_pos = 0;
}

void Comms::begin()
{
    Serial.begin(_baud);
    Serial.setTimeout(100);
}

float Comms::read_float()
{
    four_byte_converter.b[0] = recv_packet.data[0+_starting_pos];
    four_byte_converter.b[1] = recv_packet.data[1+_starting_pos];
    four_byte_converter.b[2] = recv_packet.data[2+_starting_pos];
    four_byte_converter.b[3] = recv_packet.data[3+_starting_pos];
    _starting_pos += 4;
    return four_byte_converter.fval;
}

unsigned long Comms::read_unsigned_int(){
    four_byte_converter.b[0] = recv_packet.data[0+_starting_pos];
    four_byte_converter.b[1] = recv_packet.data[1+_starting_pos];
    four_byte_converter.b[2] = recv_packet.data[2+_starting_pos];
    four_byte_converter.b[3] = recv_packet.data[3+_starting_pos];
    _starting_pos += 4;
    return four_byte_converter.uival;
}

long Comms::read_int(){
    four_byte_converter.b[0] = recv_packet.data[0+_starting_pos];
    four_byte_converter.b[1] = recv_packet.data[1+_starting_pos];
    four_byte_converter.b[2] = recv_packet.data[2+_starting_pos];
    four_byte_converter.b[3] = recv_packet.data[3+_starting_pos];
    _starting_pos += 4;
    return four_byte_converter.ival;
}

short Comms::read_short(){
    two_byte_converter.b[0] = recv_packet.data[0+_starting_pos];
    two_byte_converter.b[1] = recv_packet.data[1+_starting_pos];
    _starting_pos += 2;
    return two_byte_converter.sval;
}

unsigned short Comms::read_unsigned_short(){
    two_byte_converter.b[0] = recv_packet.data[0+_starting_pos];
    two_byte_converter.b[1] = recv_packet.data[1+_starting_pos];
    _starting_pos += 2;
    return two_byte_converter.usval;
}

long long Comms::read_long_long(){
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

unsigned long long Comms::read_unsigned_long_long(){
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

void Comms::send_int(signed long data){
    four_byte_converter.ival = data;
    send_packet.data[0+_send_starting_pos] = four_byte_converter.b[0];
    send_packet.data[1+_send_starting_pos] = four_byte_converter.b[1];
    send_packet.data[2+_send_starting_pos] = four_byte_converter.b[2];
    send_packet.data[3+_send_starting_pos] = four_byte_converter.b[3];
    _send_starting_pos += 4;
}

void Comms::send_unsigned_int(unsigned long data){
    four_byte_converter.uival = data;
    send_packet.data[0+_send_starting_pos] = four_byte_converter.b[0];
    send_packet.data[1+_send_starting_pos] = four_byte_converter.b[1];
    send_packet.data[2+_send_starting_pos] = four_byte_converter.b[2];
    send_packet.data[3+_send_starting_pos] = four_byte_converter.b[3];
    _send_starting_pos += 4;
}

void Comms::add(float data){
    four_byte_converter.fval = data;
    send_packet.data[0+_send_starting_pos] = four_byte_converter.b[0];
    send_packet.data[1+_send_starting_pos] = four_byte_converter.b[1];
    send_packet.data[2+_send_starting_pos] = four_byte_converter.b[2];
    send_packet.data[3+_send_starting_pos] = four_byte_converter.b[3];
    _send_starting_pos += 4;
}
void Comms::add(short data){
    two_byte_converter.sval = data;
    send_packet.data[0+_send_starting_pos] = two_byte_converter.b[0];
    send_packet.data[1+_send_starting_pos] = two_byte_converter.b[1];
    _send_starting_pos += 2;
}
void Comms::add(unsigned long data){
    four_byte_converter.uival = data;
    send_packet.data[0+_send_starting_pos] = four_byte_converter.b[0];
    send_packet.data[1+_send_starting_pos] = four_byte_converter.b[1];
    send_packet.data[2+_send_starting_pos] = four_byte_converter.b[2];
    send_packet.data[3+_send_starting_pos] = four_byte_converter.b[3];
    _send_starting_pos += 4;
}

void Comms::add(long data){
    four_byte_converter.ival = data;
    send_packet.data[0+_send_starting_pos] = four_byte_converter.b[0];
    send_packet.data[1+_send_starting_pos] = four_byte_converter.b[1];
    send_packet.data[2+_send_starting_pos] = four_byte_converter.b[2];
    send_packet.data[3+_send_starting_pos] = four_byte_converter.b[3];
    _send_starting_pos += 4;
}

void Comms::add(unsigned short data){
    two_byte_converter.usval = data;
    send_packet.data[0+_send_starting_pos] = two_byte_converter.b[0];
    send_packet.data[1+_send_starting_pos] = two_byte_converter.b[1];
    _send_starting_pos += 2;
}
void Comms::add(long long data){
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
void Comms::add(unsigned long long data){
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

void Comms::send(int destination_id){
    send_packet.header = 255;
    send_packet.destination_id = destination_id;
    send_packet.sender_id = 1;
    send_packet.packet_id = recv_packet.packet_id;
    send_packet.command = 7;
    send_packet.data_length = _send_starting_pos;
    send_packet.checksum = calc_checksum(send_packet);
    send_full_packet();
}

signed long Comms::calc_checksum(packet data_packet){
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

void Comms::close(void(*end_comms_handler)()){
    _data_length_errors = 0;
    _packets_recieved = 0;
    _checksum_errors = 0;
    _header_length_errors = 0;
    end_comms_handler();
}

void Comms::send_full_packet()
{
    _send_starting_pos = 0;
    byte* byte_array = reinterpret_cast<byte*>(&send_packet);
    if(Serial.availableForWrite()>sizeof(packet)-sizeof(send_packet.data)){
        Serial.write(byte_array, sizeof(packet)-sizeof(send_packet.data)+send_packet.data_length);
        Serial.flush();
    }
}

void Comms::send_byte(byte value, byte destination_id){
    send_packet.header = 255;
    send_packet.destination_id = destination_id;
    send_packet.sender_id = 1;
    send_packet.packet_id = recv_packet.packet_id;
    send_packet.command = 7;
    send_packet.data_length = 1;
    send_packet.checksum = 0;
    send_packet.data[0] = value;
    send_full_packet();

}

void Comms::send_metrics(byte destination_id){
    send_packet.header = 255;
    send_packet.destination_id = destination_id;
    send_packet.sender_id = 1;
    send_packet.packet_id = recv_packet.packet_id;
    send_packet.command = 9;
    send_packet.data_length = 12;
    send_packet.checksum = 0;
    send_packet.data[0] = _packets_recieved;
    send_packet.data[1] = (_packets_recieved >> 8) & 0xFF;
    send_packet.data[2] = (_packets_recieved >> 16) & 0xFF;
    send_packet.data[3] = (_packets_recieved >> 24) & 0xFF;
    send_packet.data[4] = (byte) _header_length_errors;
    send_packet.data[5] = (byte) _header_length_errors >> 8;
    send_packet.data[6] = (byte) _checksum_errors;
    send_packet.data[7] = (byte) _checksum_errors >> 8;
    send_packet.data[8] = (byte) _data_length_errors;
    send_packet.data[9] = (byte) _data_length_errors >> 8;
    send_packet.data[10] = (byte) _watchdog_errors;
    send_packet.data[11] = (byte) _watchdog_errors >> 8;
    send_full_packet();
}

int Comms::check_for_conn(unsigned short handshake_byte)
{
    if(Serial.available()){
        int msg = Serial.read();
        if(msg == handshake_byte){
            if(Serial.availableForWrite()){
                Serial.write(msg);
                Serial.flush();
                _ser_connect_time = millis();
                return msg;
            }
        }else{
            return msg;
        }
        
    }else{
        return 0;
    }
    
}

void Comms::check_for_data(void(*on_data_callback)(packet recv_packet), void(*no_data_callback)(), void(*watchdog_handler)()){
    _current_time = millis();
    _starting_pos = 0;
    if(_current_time-_ser_connect_time<(_watchdog_timer*2) || _packets_recieved>0){
        if(_current_time-_last_recv_time<_watchdog_timer || _packets_recieved==0){
            if(Serial.available()){
                _last_recv_time  = _current_time;
                if(Serial.readBytes((char*)&recv_packet, sizeof(packet)-sizeof(recv_packet.data))==sizeof(packet)-sizeof(recv_packet.data)){
                    if(Serial.readBytes((char*)&recv_packet.data, recv_packet.data_length)==recv_packet.data_length){
                        _packets_recieved += 1;
                        on_data_callback(recv_packet);
                        no_data_callback();
                    }else{
                        _data_length_errors += 1;
                    }
                }else{
                    _header_length_errors += 1;
                }
            }else{
                no_data_callback();
            }
        }else{
            _data_length_errors = 0;
            _packets_recieved = 0;
            _checksum_errors = 0;
            _header_length_errors = 0;
            _watchdog_errors += 1;
            watchdog_handler();
        }
    }else{
        _data_length_errors = 0;
        _packets_recieved = 0;
        _checksum_errors = 0;
        _header_length_errors = 0;
        _watchdog_errors += 1;
        watchdog_handler();
    }
}