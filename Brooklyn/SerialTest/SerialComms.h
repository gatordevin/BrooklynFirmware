#include <Arduino.h>
#include "CommsAbstract.h"

class SerialComms : public CommsAbstract
{
    public:
        void begin();
        SerialComms(unsigned int baud, short watchdog_timer);
        void check_for_data(void(*on_data_callback)(), void(*no_data_callback)(), void(*watchdog_handler)());
        int check_for_conn(unsigned short handshake_byte);
        void close(void(*end_CommsAbstract_handler)());
        void send(byte destination_id);

    private:
        unsigned int _baud;
        unsigned short _watchdog_timer;
        void send_full_packet();

};