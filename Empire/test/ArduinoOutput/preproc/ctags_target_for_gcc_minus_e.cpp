# 1 "/home/techgarage/BrooklynFirmware/Empire/test/test.ino"
# 2 "/home/techgarage/BrooklynFirmware/Empire/test/test.ino" 2



Servo servo;

void setup(){
    servo.attach(A5);
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
