#include <Arduino.h>
#pragma once

class CommsAbstract
{
    public:
        struct packet
        {
            byte header;
            
            byte destination_id;

            byte sender_id;

            unsigned long packet_id;

            byte command;

            byte data_length;

            signed long checksum;

            byte data[50];
        };

        union four_byte_conv {
            byte b[4];
            float fval;
            unsigned long uival;
            signed long ival;
        };

        union two_byte_conv {
            byte b[2];
            unsigned short int usval;
            short int sval;
        };

        union eight_byte_conv {
            byte b[8];
            unsigned long long ulval;
            long long lval;
        };
        packet recv_packet;
        packet send_packet;
        virtual void begin() = 0;
        virtual void send_full_packet() = 0;
        virtual void close(void(*end_CommsAbstract_handler)()) = 0;
        virtual void send(byte destination_id) = 0;
        void copy_packet_data(packet copy_from, packet copy_to);
        void clear_packet_data(packet packet_to_clear);
        float read_float();
        unsigned long read_unsigned_int();
        long read_int();
        unsigned short read_unsigned_short();
        short read_short();
        long long read_long_long();
        unsigned long long read_unsigned_long_long();

        void send_float();
        void send_unsigned_int(unsigned long data);
        void send_int(signed long data);
        void send_unsigned_short();
        void send_short();
        void send_long_long();
        void send_unsigned_long_long();

        void add(float data);
        void add(long data);
        void add(short data);
        void add(unsigned long data);
        void add(unsigned short data);
        void add(long long data);
        void add(unsigned long long data);

        void send(int destination_id);

    protected:
        CommsAbstract();
        unsigned short _ser_connect_time;
        unsigned long _packets_recieved;
        unsigned int _header_length_errors;
        unsigned int _data_length_errors;
        unsigned int _checksum_errors;
        unsigned int _watchdog_errors;
        unsigned short _starting_pos;
        unsigned short _send_starting_pos;

        four_byte_conv four_byte_converter;
        two_byte_conv two_byte_converter;
        eight_byte_conv eight_byte_converter;
        int _last_recv_time;
        int _current_time;
        signed long calc_checksum(packet data_packet);
};