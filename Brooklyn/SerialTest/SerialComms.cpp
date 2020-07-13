#include "SerialComms.h"


SerialComms::SerialComms(unsigned int baud, short watchdog_timer)
{
    _baud = baud;
    _watchdog_timer = watchdog_timer;
}

void SerialComms::begin()
{
    Serial.begin(_baud);
    Serial.setTimeout(100);
}

void SerialComms::send_full_packet()
{
    _send_starting_pos = 0;
    byte* byte_array = reinterpret_cast<byte*>(&send_packet);
    if(Serial.availableForWrite()>sizeof(packet)-sizeof(send_packet.data)){
        Serial.write(byte_array, sizeof(packet)-sizeof(send_packet.data)+send_packet.data_length);
        Serial.flush();
    }
}

void SerialComms::check_for_data(void(*on_data_callback)(packet recv_packet), void(*no_data_callback)(), void(*watchdog_handler)()){
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

int SerialComms::check_for_conn(unsigned short handshake_byte)
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