#include <BrooklynSerialComms.h>

BrooklynSerialComms computer = BrooklynSerialComms(9600);


void setup(){
    computer.begin();
}

void loop(){
    computer.send_packet.data_len = 1;
    computer.send_packet.data = 12;
    computer.send();
}
