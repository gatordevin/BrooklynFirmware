#include "SerialComms.h"
#define RED 1
#define BLUE 2
#define GREEN 3
#define OFF 4

#define red_pin 7
#define blue_pin 11
#define green_pin A5

SerialComms computer = SerialComms(9600, 250);
int mode = 0;

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

void setup(){
    pinMode(red_pin, OUTPUT);
    pinMode(blue_pin, OUTPUT);
    pinMode(green_pin, OUTPUT);
    LED(OFF);
    computer.begin();
}

void loop(){
    switch(mode){
        case 0:
            LED(BLUE);
            mode = computer.check_for_conn(140);
            break;

        case 140:
            LED(GREEN);
            computer.check_for_data(&controller_switch, &background_func, &reset_board);
            break;

        case 120:
            break;
    }
}

void controller_switch(packet msg){
    switch(msg.command){
        case 78:
            {
                float result = computer.read_float();
                unsigned long result_2 = computer.read_unsigned_int();
                short result_3 = computer.read_short();
                long long result_4 = computer.read_long_long();
                computer.add((float) 200.345);
                computer.send(0);
            }
            break;
        
        case 160:
            LED(RED);
            break;

        case 140:
            computer.close(&reset_board);
            break;
    }
    
}

void background_func(){
    
}

void programmer_switch(){
    
}

void reset_board(){
    mode = 0;
}