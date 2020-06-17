#include <Servo.h>

#define servo_pin A5

Servo servo;

void setup(){
    servo.attach(servo_pin);
}

void loop(){
    //500 - 2400
    servo.writeMicroseconds(2500);
    // for(int i = 0; i<=3000;  i+=100){
    //     servo.writeMicroseconds(i);
    //     delay(4000);
    // }
    // for(int i = 3000; i>=0;  i-=100){
    //     servo.writeMicroseconds(i);
    //     delay(4000);
    // }
}