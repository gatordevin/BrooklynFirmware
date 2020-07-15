#include <Arduino.h>
#include "CommsAbstract.h"
#include <SPI.h>

class SPISlave : public CommsAbstract
{
    public:
        volatile int x = 0;
        volatile int r = 0;
        uint8_t data_buff[sizeof(recv_packet)];
        volatile bool write = false;
        volatile bool full_packet_ready = false;
        volatile uint8_t interrupt_read_buff[100];
        SPISlave();
        void begin();
        void close(void(*end_CommsAbstract_handler)());
        void read_full_packet();
        void send_full_packet();
        void send(byte destination_id);
        void update_data(byte data);
        void write_byte(byte data);
        void check_for_data(void(*on_data_callback)(), void(*no_data_callback)());
        byte read_byte();
        
    private:
        byte _destination_id;
        byte send_byte(byte data);
};