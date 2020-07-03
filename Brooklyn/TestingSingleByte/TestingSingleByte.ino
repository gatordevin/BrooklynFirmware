#include <SPI.h>
#define SS  1

#define RED 1
#define BLUE 2
#define GREEN 3
#define OFF 4

#define red_pin 7
#define blue_pin 11
#define green_pin A5

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
uint8_t ss[] = {A0, A1, 0, 1, 10, 9, 12, 4};
void setup(){
    Serial.begin(1000000);
    pinMode(red_pin, OUTPUT);
    pinMode(blue_pin, OUTPUT);
    pinMode(green_pin, OUTPUT);
    pinMode(SS, OUTPUT);
    pinMode(MOSI, OUTPUT);
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV16);
    digitalWrite(SS, HIGH);
    LED(RED);
    for(int i=0;i<8;i++){
        pinMode(ss[i], OUTPUT);
        digitalWrite(ss[i], HIGH);
    }
}

void loop(void){
    LED(RED);
    digitalWrite(SS, LOW);
    uint8_t resp = SPI.transfer(27);
    digitalWrite(SS, HIGH);
    LED(GREEN);
}
