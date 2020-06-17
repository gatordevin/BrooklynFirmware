#include <Arduino.h>
#line 1 "/home/techgarage/BrooklynFirmware/Empire/test/test.ino"
#include <Servo.h>

#define servo_pin A5

Servo servo;

#line 7 "/home/techgarage/BrooklynFirmware/Empire/test/test.ino"
void setup();
#line 11 "/home/techgarage/BrooklynFirmware/Empire/test/test.ino"
void loop();
#line 7 "/home/techgarage/BrooklynFirmware/Empire/test/test.ino"
void setup(){
    servo.attach(servo_pin);
}

void loop(){
    //500 - 
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
