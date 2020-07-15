#include <Arduino.h>
#include "CommsAbstract.h"
#include <SPI.h>

class SPIMaster : public CommsAbstract
{
    public:
        SPIMaster();
        void begin();
        void close(void(*end_CommsAbstract_handler)());
        void read_full_packet(byte ss_pin);
        void send_full_packet();
        void send(byte destination_id);
    private:
        byte _destination_id;
        byte send_byte(byte data);
};