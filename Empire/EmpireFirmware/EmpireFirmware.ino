#include <SPI.h>
#include <Servo.h>
#include <AutoPID.h>
#include <Encoder.h>



#define RED 1
#define BLUE 2
#define GREEN 3
#define OFF 4

#define MAX_PWM 2150
#define MAX_PWM 1500
#define MIN_PWM 850

#define red_pin 0
#define blue_pin A2
#define green_pin A3

#define servo_1_pin A4
#define servo_2_pin A5

Servo servo_1;
Servo servo_2;
Servo servos[2] = {servo_1,servo_2};


int servo_1_min_angle = 0;
int servo_1_max_angle = 180;
int servo_1_min_microseconds = 1000;
int servo_1_max_microseconds = 2000;
int servo_2_min_angle = 0;
int servo_2_max_angle = 180;
int servo_2_min_microseconds = 1000;
int servo_2_max_microseconds = 2000;

#define outputMin 0
#define outputMax 255

volatile double newposition = 0;
volatile double newtime = 0;
volatile int vel= 0;

double Kp = .08;
double Ki = 0.00;
double Kd = 0.00;
double velocity = 0;
double setpoint = 0;
double output = 0;
double speed_output = 0;
double previous_pos = 0;
volatile double previous_time = 0;
volatile double encoder_pos = 0;
double input = 0;
int Mspeed = 0;
int Mdir = 0;
int Kz = 0;
int breakOrCoast = 0;
int motorTpr = 0;

Encoder Enc1(2,3);
AutoPID posPID(&encoder_pos, &setpoint, &output, -255.0, 255.0, Kp, Ki, Kd);
AutoPID speedPID(&velocity, &setpoint, &speed_output, -255.0, 255.0, Kp, Ki, Kd);





volatile uint8_t interrupt_buff[100];
uint8_t spi_recv_buff[20];
uint8_t spi_send_buff[100];
uint8_t resp_buff[20];
uint8_t packet_buff[100];

volatile int idx = 0;
volatile int ridx = 0;

uint8_t checksum1 = 0;
uint8_t checksum2 = 0;

#define CMD_CARD_HB 72
#define CMD_GET_CARD_TYPE 3
#define CMD_SET_PWM 9
#define CMD_SET_SERVO_RANGE 11
#define CMD_GET_ENCODER 24
#define CMD_MOTOR_PWM 25
#define CMD_PID_SETPOINT 26
#define CMD_READ_SPEED 27
#define CMD_PID_SPEED 28
#define CMD_PID_CONSTANTS 29
#define CMD_ZERO_ENCODER 30
#define CMD_SET_TPR 23
#define CMD_SUCCESS 91
#define CMD_FAIL 90



int data_array[10];
void decTo256(int n) 
{ 
     
    // array to store octal number 
    
  
    // counter for octal number array 
    int i = 0; 
    while (n != 0) { 
  
        // storing remainder in octal array 
        data_array[i] = n % 255; 
        n = n / 255; 
        i++; 
    }
    
  
    // printing octal number array in reverse order 
   
} 

void Mset(int MotorDirection, int MotorSpeed){
  if(MotorSpeed < 25){
    MotorSpeed = 0;
  }
  if(MotorDirection == 0){
     analogWrite(5,0);
     analogWrite(6,0);
   
  }else if(MotorDirection == 1){
    analogWrite(5,MotorSpeed);
    analogWrite(6,0);
  }else if(MotorDirection == 2){
    analogWrite(5,0);
    analogWrite(6,MotorSpeed);
  }else{
    analogWrite(5,0);
    analogWrite(6,0);
  }
}

double calculateSpeed(){
  newposition = Enc1.read();
  newtime = millis()*0.001;
  vel = (newposition-previous_pos)/(newtime-previous_time);
  previous_pos = newposition;
  previous_time = newtime;
  return vel;
}


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
    if(interrupt_buff[idx]==140){
      LED(GREEN);
    }
    interrupt_buff[idx] = SPDR;
    if(idx==99){
        idx=0;
    }
    idx+=1;
}

uint8_t readByte(){
    while(idx==ridx){}
    uint8_t data = interrupt_buff[ridx];
    if(ridx==99){
        ridx=0;
    }
    ridx+=1;
    return(data);
}

void waitForByte(){
    while(idx==ridx){}
    ridx=idx;
}

void SPISend(uint8_t data){
    SPDR = data;
    waitForByte();
}

bool verifyChecksum(uint8_t recv_buff[]){
    calculateChecksum(recv_buff);
    if(checksum1 == recv_buff[recv_buff[4]+7]){ //compare with checksum one
        if(checksum2 == recv_buff[recv_buff[4]+8]){ //compare with checksum two
            return true; //return true if checksums validate
        }
    }
    return false; //return false if either checksum fails
}

void calculateChecksum(uint8_t data_buff[]){
    int packetSum = 0;
    packetSum += data_buff[0]; //add header to checksum
    packetSum += data_buff[1]; //add controller id to checksum
    packetSum += data_buff[2]; //From ID
    packetSum += data_buff[3]; //add controller command to checksum
    packetSum += data_buff[4]; //add data length to checksum
    for(int i=0;i<data_buff[4];i++){
        packetSum += data_buff[i+5]; //add data bytes to checksum
    }
    packetSum += data_buff[data_buff[4]+5];
    packetSum += data_buff[data_buff[4]+6];
    checksum1 = floor(packetSum / 256);
    checksum2 = packetSum % 256;
}

bool readSPIPacket(){
    spi_recv_buff[0] = 0;
    while(spi_recv_buff[0] != 255){
        spi_recv_buff[0] = readByte(); //header 255
    }
    spi_recv_buff[1] = readByte(); //ID
    spi_recv_buff[2] = readByte(); //FROM ID
    spi_recv_buff[3] = readByte(); //Command
    spi_recv_buff[4] = readByte(); //Data length
    for(int i=0;i<spi_recv_buff[4];i++){
        spi_recv_buff[5+i] = readByte();
    }
    spi_recv_buff[spi_recv_buff[4]+5] = readByte();
    spi_recv_buff[spi_recv_buff[4]+6] = readByte();
    spi_recv_buff[spi_recv_buff[4]+7] = readByte(); //checksum 1
    spi_recv_buff[spi_recv_buff[4]+8] = readByte(); //checksum 2
    return(verifyChecksum(spi_recv_buff)); //return whether data was received succesfully
}

void sendSPIPacket(uint8_t send_buff[]){
    SPISend(send_buff[0]); //Send Header
    SPISend(send_buff[1]); //Send ID
    SPISend(send_buff[2]); //From ID
    SPISend(send_buff[3]); //Send Command
    SPISend(send_buff[4]); //Send Data length
    for(int i=0;i<send_buff[4];i++){
        SPISend(send_buff[5+i]); //Send packet data
    }
    SPISend(send_buff[send_buff[4]+5]);
    SPISend(send_buff[send_buff[4]+6]);
    calculateChecksum(send_buff);
    SPISend(checksum1); //Send Checksum one
    SPISend(checksum2); //Send Checksum two
    clear_packet();
    if(send_buff[0]==255){
      LED(BLUE);
    }
}

uint8_t current_data_packet_pos = 0;
void create_data_packet(){
  uint8_t start_index = 0;
  if(packet_buff[0] == 255){
    spi_send_buff[5] = packet_buff[0];
    spi_send_buff[6] = packet_buff[1];
    spi_send_buff[7] = packet_buff[2]; //From ID
    spi_send_buff[8] = packet_buff[3];
    spi_send_buff[9] = packet_buff[4];
    for(int i=0;i<packet_buff[4];i++){
      spi_send_buff[10+i] = packet_buff[5+i];
    }
    spi_send_buff[packet_buff[4]+10] = packet_buff[packet_buff[4]+5];
    spi_send_buff[packet_buff[4]+11] = packet_buff[packet_buff[4]+6];
    spi_send_buff[packet_buff[4]+12] = packet_buff[packet_buff[4]+7];
    spi_send_buff[packet_buff[4]+13] = packet_buff[packet_buff[4]+8];
    spi_send_buff[4] = 9+packet_buff[4];
    for(int i=0;i<100-spi_send_buff[4];i+=1)
    {
        packet_buff[i]=packet_buff[i+spi_send_buff[4]];
    }
    current_data_packet_pos -= spi_send_buff[4];
  }else{
    spi_send_buff[4] = 0;
  }
  
  
  // for(int i=99;i>99-spi_send_buff[3];i-=1)
  // {
  //     packet_buff[i]=0;
  // }
}
void add_data_hb_buffer(uint8_t byte){
  packet_buff[current_data_packet_pos] = byte;
  current_data_packet_pos += 1;
  if(current_data_packet_pos==100){
    current_data_packet_pos = 0;
  }
}

void setup(){
    pinMode(red_pin, OUTPUT);
    pinMode(blue_pin, OUTPUT);
    pinMode(green_pin, OUTPUT);
    pinMode(MISO,OUTPUT);

   
    previous_time = millis();
    servo_1.attach(servo_1_pin);
    servo_2.attach(servo_2_pin);
    SPCR |= _BV(SPE);
    SPCR &= ~(_BV(MSTR)); //Arduino is Slave
    SPCR |= _BV(SPIE);      //we not using SPI.attachInterrupt() why?
    LED(BLUE);
    sei();
//    add_data_hb_buffer(255);
//    add_data_hb_buffer(0);
//    add_data_hb_buffer(2);
//    add_data_hb_buffer(4);
//    add_data_hb_buffer(0);
//    add_data_hb_buffer(0);
//    add_data_hb_buffer(0);
//    add_data_hb_buffer(1);
//    add_data_hb_buffer(3);
//    add_data_hb_buffer(255);
//    add_data_hb_buffer(0);
//    add_data_hb_buffer(2);
//    add_data_hb_buffer(4);
//    add_data_hb_buffer(0);
//    add_data_hb_buffer(0);
//    add_data_hb_buffer(0);
//    add_data_hb_buffer(1);
//    add_data_hb_buffer(3);
//    add_data_hb_buffer(255);
//    add_data_hb_buffer(0);
//    add_data_hb_buffer(2);
//    add_data_hb_buffer(4);
//    add_data_hb_buffer(0);
//    add_data_hb_buffer(0);
//    add_data_hb_buffer(0);
//    add_data_hb_buffer(1);
//    add_data_hb_buffer(3);
}

long convertToPWM(long angle, long minAngle, long maxAngle, long minPWM, long maxPWM){
  long valuePWM = map(angle,minAngle,maxAngle,minPWM,maxPWM);
  return valuePWM;
}

int ToDec(uint8_t lsb, uint8_t msb){
  int bigNumber = lsb + (msb * 255);

  return bigNumber;
}
int ToDecNeg(int lsb, int msb, int neg){
  int bigNumber = lsb + (msb * 255);
  if(neg == 0){
    bigNumber = bigNumber*-1;
  }
  return bigNumber;
}
int negative_check(int x){
  if(abs(x) == x){
    return 0;
  }else{
    return 1;
  }
}
void integralZone(double setpoint, double in,  int zone){
  if(abs(setpoint-in) > zone){
    posPID.setGains(Kp,0.00,Kd);
  }else{
    posPID.setGains(Kp,Ki,Kd);
  }
  
}

void clear_data() {
  for(int i = 0; i < 10; i++) {
    data_array[i] = 0;
  }
} 

void response_packet(bool success, int destination_id, int data_len = 0) {
  resp_buff[0] = 255; // header
  resp_buff[1] = spi_recv_buff[2]; // destination id
  resp_buff[2] = spi_recv_buff[1]; //sender id
  if(success) { // command byte used to signal success or fail
    resp_buff[3] = CMD_SUCCESS; 
  } else {
    resp_buff[3] = CMD_FAIL;
  }
  resp_buff[4] = data_len; // data length

  for(int i = 0; i < data_len; i++) {
    resp_buff[i+5] = data_array[i];
    data_array[i] = 0;
  }

  resp_buff[5+data_len] = spi_recv_buff[spi_recv_buff[4]+5];
  resp_buff[6+data_len] = spi_recv_buff[spi_recv_buff[4]+6];
}

void create_id(int num){
  spi_send_buff[spi_send_buff[4]+5] = num%255;
  spi_send_buff[spi_send_buff[4]+6] = (int) floor(num/255);
}

void clear_packet(){
  for(int i=0;i<100;i++){
    spi_send_buff[i] = 0;
  }
}

void loop(){
    while(idx==ridx){
      
    }
    if(readSPIPacket()){
        switch(spi_recv_buff[3]){
            case CMD_ZERO_ENCODER:
//                LED(GREEN);
                Enc1.write(0);
                spi_send_buff[0] = 255;
                spi_send_buff[1] = 0;
                spi_send_buff[2] = 2;
                spi_send_buff[3] = CMD_ZERO_ENCODER;
                spi_send_buff[4] = 0;
                spi_send_buff[5] = spi_send_buff[5];
                spi_send_buff[6] = spi_send_buff[6];
                spi_send_buff[7] = spi_send_buff[7];
                spi_send_buff[8] = spi_send_buff[8];
                sendSPIPacket(spi_send_buff);
                break;
            case CMD_SET_TPR:
              motorTpr = ToDec(spi_recv_buff[5], spi_recv_buff[6]);
              spi_send_buff[0] = 255;
              spi_send_buff[1] = 0;
              spi_send_buff[2] = 2;
              spi_send_buff[3] = CMD_SET_TPR;
              spi_send_buff[4] = 2;
              spi_send_buff[5] = spi_recv_buff[8];
              spi_send_buff[6] = spi_recv_buff[69];
              sendSPIPacket(spi_send_buff);
              break;
            
            case CMD_GET_CARD_TYPE:
                data_array[0] = 1; // CARD TYPE FOR EMPIRE ID 1

//                response_packet(true, 0, 1);
                spi_send_buff[0] = 255;
                spi_send_buff[1] = 0;
                spi_send_buff[2] = 2;
                spi_send_buff[3] = CMD_GET_CARD_TYPE;
                spi_send_buff[4] = 1;
                spi_send_buff[5] = data_array[0];
                spi_send_buff[8] = spi_recv_buff[5];
                spi_send_buff[9] = spi_recv_buff[6];
                spi_send_buff[10] = spi_recv_buff[7];
                spi_send_buff[11] = spi_recv_buff[8];
                
                sendSPIPacket(spi_send_buff);
                break;
              
            case CMD_CARD_HB:
                spi_send_buff[1] = 0;
                spi_send_buff[2] = 2;
                spi_send_buff[3] = CMD_CARD_HB;
                create_data_packet();
                spi_send_buff[spi_send_buff[4]+5] = spi_recv_buff[spi_recv_buff[4]+5];
                spi_send_buff[spi_send_buff[4]+6] = spi_recv_buff[spi_recv_buff[4]+6];
                
                sendSPIPacket(spi_send_buff);
                break;               
  
            case CMD_GET_ENCODER:
//                LED(GREEN);  
                encoder_pos = Enc1.read();
                decTo256(abs(encoder_pos));
                if(abs(encoder_pos) < 256){
                  data_array[1] = 0; 
                }
                data_array[2] = negative_check(encoder_pos);
              
//                response_packet(true, 1, 3);
                spi_send_buff[0] = 255;
                spi_send_buff[1] = 0;
                spi_send_buff[2] = 2;
                spi_send_buff[3] = CMD_GET_ENCODER; 
                spi_send_buff[4] = 3;
                spi_send_buff[5] = (uint8_t) abs(encoder_pos)%255;
                spi_send_buff[6] = (int) abs(encoder_pos)/255;
                spi_send_buff[7] = negative_check(encoder_pos);
                spi_send_buff[8] = spi_recv_buff[5];
                spi_send_buff[9] = spi_recv_buff[6];
                spi_send_buff[10] = spi_recv_buff[7];
                spi_send_buff[11] = spi_recv_buff[8];
                sendSPIPacket(spi_send_buff);
                break;

            case CMD_MOTOR_PWM:
                LED(GREEN);
                Mset(spi_recv_buff[5], spi_recv_buff[6]);
                sendSPIPacket(spi_recv_buff);
                break;

            case CMD_PID_CONSTANTS:
//                LED(GREEN);
                Kp = (ToDec(spi_recv_buff[5], spi_recv_buff[6]))/(double)1000;
                Ki = (ToDec(spi_recv_buff[7], spi_recv_buff[8]))/(double)1000;
                Kd = (ToDec(spi_recv_buff[9], spi_recv_buff[10]))/(double)1000;
                Kz = spi_recv_buff[11];
                //spi_send_buff[1] = 0;
                //spi_send_buff[2] = 2;
                //spi_send_buff[3] = 5;
                //spi_send_buff[4] = 7;
                posPID.setGains(Kp,Ki,Kd);
                
//                response_packet(true, 1);
                sendSPIPacket(spi_recv_buff);
                break;

            case CMD_PID_SETPOINT:
//                 spi_send_buff[1] = 0;
//                 spi_send_buff[2] = 2;
//                 spi_send_buff[3] = 5;
//                 spi_send_buff[4] = 3;
                encoder_pos = Enc1.read();
                setpoint = ToDecNeg(spi_recv_buff[5], spi_recv_buff[6], spi_recv_buff[7]);
//                setpoint = setpoint * (1.55555555556);
                int currentpoint = (int) (encoder_pos * (1.55555555556));
                integralZone(setpoint, encoder_pos, Kz);
                posPID.run();
                if(abs(output) != output){
                  int temp = output*-1;
                  Mset(1,temp);
                }else{
                  Mset(2,output);
                }
                decTo256(encoder_pos);
                if(abs(encoder_pos) < 256){
                  data_array[1] = 0; 
                }
                data_array[2] = negative_check(encoder_pos);
                spi_send_buff[0] = 255;
                spi_send_buff[1] = 0;
                spi_send_buff[2] = 2;
                spi_send_buff[3] = CMD_PID_SETPOINT;
                spi_send_buff[4] = 3;
                spi_send_buff[5] = (uint8_t) abs(encoder_pos)%255;
                spi_send_buff[6] = (int) abs(encoder_pos)/255;
                spi_send_buff[7] = negative_check(encoder_pos);
                spi_send_buff[8] = spi_send_buff[8];
                spi_send_buff[9] = spi_send_buff[9];
//                response_packet(true, 1, 3);
                sendSPIPacket(spi_send_buff);
                break;
                           
            case CMD_READ_SPEED:
                LED(GREEN);
                velocity = calculateSpeed();
                
                decTo256(abs(velocity));
                if(abs(velocity) < 256){
                  data_array[1] = 0; 
                }
                data_array[2] = negative_check(velocity);
                
                response_packet(true, 1, 3);
                sendSPIPacket(resp_buff);
                break;

            case CMD_PID_SPEED:
                LED(GREEN);                
                velocity = calculateSpeed();
            
                setpoint = spi_recv_buff[5];
                speedPID.run();
                if(abs(speed_output) != speed_output){
                  speed_output = speed_output*-1;
                  Mset(1,speed_output);
                }else{
                  Mset(2,speed_output);
                }
                
                data_array[0] = abs(velocity);
                data_array[1] = negative_check(velocity);
            
                response_packet(true, 1, 2);
                sendSPIPacket(spi_send_buff);
                break;
                
            case CMD_SET_PWM:
                if(spi_recv_buff[5]==0){
                    servos[spi_recv_buff[5]].writeMicroseconds(convertToPWM(ToDec(spi_recv_buff[6], spi_recv_buff[7]),servo_1_min_angle,servo_1_max_angle,servo_1_min_microseconds,servo_1_max_microseconds));
                }
                if(spi_recv_buff[5]==1){
                    servos[spi_recv_buff[5]].writeMicroseconds(convertToPWM(ToDec(spi_recv_buff[6], spi_recv_buff[7]),servo_2_min_angle,servo_2_max_angle,servo_2_min_microseconds,servo_2_max_microseconds));
                }
                response_packet(true, 1);
                sendSPIPacket(resp_buff);
                break;
                
            
            
            case CMD_SET_SERVO_RANGE:
                int servo_min_angle = ToDec(spi_recv_buff[6], spi_recv_buff[7]);
                int servo_max_angle = ToDec(spi_recv_buff[8], spi_recv_buff[9]);
                int servo_min_microseconds = ToDec(spi_recv_buff[10], spi_recv_buff[11]);
                int servo_max_microseconds = ToDec(spi_recv_buff[12], spi_recv_buff[13]);
                if(spi_recv_buff[5]==0){
                    servo_1_min_angle = servo_min_angle;
                    servo_1_max_angle = servo_max_angle;
                    servo_1_min_microseconds = servo_min_microseconds;
                    servo_1_max_microseconds = servo_max_microseconds;
                   
                }
                if(spi_recv_buff[5]==1){
                    servo_2_min_angle = servo_min_angle;
                    servo_2_max_angle = servo_max_angle;
                    servo_2_min_microseconds = servo_min_microseconds;
                    servo_2_max_microseconds = servo_max_microseconds;
                }
                response_packet(true, 1);
                sendSPIPacket(resp_buff);
                break;
                
             
            default:
                LED(BLUE);
                break;
        }
    }else{
        LED(RED);
    }
}
