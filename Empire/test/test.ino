#include "SPISlave.h"

#define RED 1
#define BLUE 2
#define GREEN 3
#define OFF 4

#define red_pin 0
#define blue_pin A2
#define green_pin A3

SPISlave spi_bus = SPISlave();

void LED(uint8_t color){
    switch (color){
        case RED:
            digitalWrite(red_pin, LOW);
            digitalWrite(blue_pin, HIGH);
            digitalWrite(green_pin, HIGH);
            break;
        case BLUE:
            digitalWrite(red_pin, HIGH);
            digitalWrite(blue_pin, LOW);
            digitalWrite(green_pin, HIGH);
            break;
        case GREEN:
            digitalWrite(red_pin, HIGH);
            digitalWrite(blue_pin, HIGH);
            digitalWrite(green_pin, LOW);
            break;
        case OFF:
            digitalWrite(red_pin, HIGH);
            digitalWrite(blue_pin, HIGH);
            digitalWrite(green_pin, HIGH);
            break;
    }
}

ISR (SPI_STC_vect)
{
    spi_bus.update_data(SPDR);
}

void setup(){
    pinMode(red_pin, OUTPUT);
    pinMode(blue_pin, OUTPUT);
    pinMode(green_pin, OUTPUT);
    spi_bus.begin();
    LED(RED);
}

void loop(void){
    spi_bus.check_for_data(&controller_switch, &background_func);
}

void controller_switch(){
    switch(spi_bus.recv_packet.command){
        case 7:
            float result = spi_bus.read_float();
            
            spi_bus.add((short) 19);
            spi_bus.send(0);
            if(spi_bus.send_packet.sender_id==2){
                LED(GREEN);
            }
            break;
    }
    
}

void background_func(){

}