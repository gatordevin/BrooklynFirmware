# 1 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino"
# 2 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino" 2
# 3 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino" 2
# 4 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino" 2
# 5 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino" 2
# 23 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino"
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




double Kp = .08;
double Ki = 0.00;
double Kd = 0.00;
double velocity = 0;
double setpoint = 0;
double output = 0;
double speed_output = 0;
double previous_pos = 0;
double previous_time = 0;
double encoder_pos = 0;
double input = 0;
int Mspeed = 0;
int Mdir = 0;
int Kz = 0;

Encoder Enc1(2,3);
AutoPID posPID(&encoder_pos, &setpoint, &output, -255.0, 255.0, Kp, Ki, Kd);
AutoPID speedPID(&velocity, &setpoint, &speed_output, -255.0, 255.0, Kp, Ki, Kd);





volatile uint8_t interrupt_buff[100];
uint8_t spi_recv_buff[20];
uint8_t spi_send_buff[20];
volatile int idx = 0;
volatile int ridx = 0;

uint8_t checksum1 = 0;
uint8_t checksum2 = 0;
# 82 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino"
int data_array[3];
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
  double newposition = Enc1.read();
  double newtime = millis();
  double vel = (newposition-previous_pos)/(newtime/previous_time);
  previous_pos = newposition;
  previous_time = newtime;
  return vel;
}


void LED(uint8_t color){
    switch (color){
        case 1:
            digitalWrite(0, 0x0);
            digitalWrite(A2, 0x1);
            digitalWrite(A3, 0x1);
            break;
        case 2:
            digitalWrite(0, 0x1);
            digitalWrite(A2, 0x0);
            digitalWrite(A3, 0x1);
            break;
        case 3:
            digitalWrite(0, 0x1);
            digitalWrite(A2, 0x1);
            digitalWrite(A3, 0x0);
            break;
        case 4:
            digitalWrite(0, 0x1);
            digitalWrite(A2, 0x1);
            digitalWrite(A3, 0x1);
            break;
    }
}


# 158 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino" 3
extern "C" void __vector_17 /* SPI Serial Transfer Complete */ (void) __attribute__ ((signal,used, externally_visible)) ; void __vector_17 /* SPI Serial Transfer Complete */ (void)

# 159 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino"
{
    interrupt_buff[idx] = 
# 160 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino" 3
                         (*(volatile uint8_t *)((0x2E) + 0x20))
# 160 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino"
                             ;
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
    
# 183 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino" 3
   (*(volatile uint8_t *)((0x2E) + 0x20)) 
# 183 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino"
        = data;
    waitForByte();
}

bool verifyChecksum(uint8_t recv_buff[]){
    calculateChecksum(recv_buff);
    if(checksum1 == recv_buff[recv_buff[3]+4]){ //compare with checksum one
        if(checksum2 == recv_buff[recv_buff[3]+5]){ //compare with checksum two
            return true; //return true if checksums validate
        }
    }
    return false; //return false if either checksum fails
}

void calculateChecksum(uint8_t data_buff[]){
    int packetSum = 0;
    packetSum += data_buff[0]; //add header to checksum
    packetSum += data_buff[1]; //add controller id to checksum
    packetSum += data_buff[2]; //add controller command to checksum
    packetSum += data_buff[3]; //add data length to checksum
    for(int i=0;i<data_buff[3];i++){
        packetSum += data_buff[i+4]; //add data bytes to checksum
    }
    checksum1 = floor(packetSum / 256);
    checksum2 = packetSum % 256;
}

bool readSPIPacket(){
    spi_recv_buff[0] = 0;
    while(spi_recv_buff[0] != 255){
        spi_recv_buff[0] = readByte(); //header 255
    }
    spi_recv_buff[1] = readByte(); //ID
    spi_recv_buff[2] = readByte(); //Command
    spi_recv_buff[3] = readByte(); //Data length
    for(int i=0;i<spi_recv_buff[3];i++){
        spi_recv_buff[4+i] = readByte();
    }
    spi_recv_buff[spi_recv_buff[3]+4] = readByte(); //checksum 1
    spi_recv_buff[spi_recv_buff[3]+5] = readByte(); //checksum 2
    return(verifyChecksum(spi_recv_buff)); //return whether data was received succesfully
}

void sendSPIPacket(uint8_t send_buff[]){
    SPISend(send_buff[0]); //Send Header
    SPISend(send_buff[1]); //Send ID
    SPISend(send_buff[2]); //Send Command
    SPISend(send_buff[3]); //Send Data length
    for(int i=0;i<send_buff[3];i++){
        SPISend(send_buff[4+i]); //Send packet data
    }
    calculateChecksum(send_buff);
    SPISend(checksum1); //Send Checksum one
    SPISend(checksum2); //Send Checksum two
}

void setup(){
    pinMode(0, 0x1);
    pinMode(A2, 0x1);
    pinMode(A3, 0x1);
    pinMode(MISO,0x1);
    servo_1.attach(A4);
    servo_2.attach(A5);
    
# 246 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino" 3
   (*(volatile uint8_t *)((0x2C) + 0x20)) 
# 246 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino"
        |= 
# 246 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino" 3
           (1 << (6))
# 246 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino"
                   ;
    
# 247 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino" 3
   (*(volatile uint8_t *)((0x2C) + 0x20)) 
# 247 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino"
        &= ~(
# 247 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino" 3
             (1 << (4))
# 247 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino"
                      ); //Arduino is Slave
    
# 248 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino" 3
   (*(volatile uint8_t *)((0x2C) + 0x20)) 
# 248 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino"
        |= 
# 248 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino" 3
           (1 << (7))
# 248 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino"
                    ; //we not using SPI.attachInterrupt() why?
    LED(2);
    
# 250 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino" 3
   __asm__ __volatile__ ("sei" ::: "memory")
# 250 "/home/techgarage/BrooklynFirmware/Empire/EmpireFirmware/EmpireFirmware.ino"
        ;
}

long convertToPWM(long angle, long minAngle, long maxAngle, long minPWM, long maxPWM){
  long valuePWM = map(angle,minAngle,maxAngle,minPWM,maxPWM);
  return valuePWM;
}

int ToDec(uint8_t lsb, uint8_t msb){
  int bigNumber = lsb + (msb * 255);

  return bigNumber;
}
int negitive_check(int x){
  if(((x)>0?(x):-(x)) == x){
    return 0;
  }else{
    return 1;
  }
}
void integralZone(double setpoint, double in, int zone){
  if(((setpoint-in)>0?(setpoint-in):-(setpoint-in)) > zone){
    posPID.setGains(Kp,0.00,Kd);
  }else{
    posPID.setGains(Kp,Ki,Kd);
  }

}


void loop(){
    if(readSPIPacket()){
        spi_send_buff[0] = 255; //Set serial send header
        switch(spi_recv_buff[2]){
          case 4:
                spi_send_buff[1] = 1;
                spi_send_buff[2] = 5;
                spi_send_buff[3] = 0;

                sendSPIPacket(spi_recv_buff);
                break;
            case 3:
                spi_send_buff[1] = 0;
                spi_send_buff[2] = 3;
                spi_send_buff[3] = 1;
                spi_send_buff[4] = 1; //CARD TYPE FOR EMPIRE ID 1
                sendSPIPacket(spi_recv_buff);
                break;
             case 24:

                LED(3);
                spi_send_buff[1] = 1;
                spi_send_buff[2] = 5;
                spi_send_buff[3] = 3;

                encoder_pos = Enc1.read();
                decTo256(((encoder_pos)>0?(encoder_pos):-(encoder_pos)));
                if(((encoder_pos)>0?(encoder_pos):-(encoder_pos)) < 256){
                  data_array[1] = 0;
                }
                spi_send_buff[4] = data_array[0];
                spi_send_buff[5] = data_array[1];
                spi_send_buff[6] = negitive_check(encoder_pos);
                sendSPIPacket(spi_send_buff);
                break;
            case 25:


                LED(3);
                spi_send_buff[1] = 1;
                spi_send_buff[2] = 5;
                spi_send_buff[3] = 3;
                encoder_pos = Enc1.read();
                decTo256(((encoder_pos)>0?(encoder_pos):-(encoder_pos)));
                if(((encoder_pos)>0?(encoder_pos):-(encoder_pos)) < 256){
                  data_array[1] = 0;
                }
                spi_send_buff[4] = data_array[0];
                spi_send_buff[5] = data_array[1];
                spi_send_buff[6] = negitive_check(encoder_pos);

                Mset(spi_recv_buff[4], spi_recv_buff[5]);


                sendSPIPacket(spi_send_buff);
                break;

            case 29:
                LED(3);
                spi_send_buff[1] = 1;
                spi_send_buff[2] = 5;
                spi_send_buff[3] = 7;
                Kp = (ToDec(spi_recv_buff[4], spi_recv_buff[5]))/(double)1000;
                Ki = (ToDec(spi_recv_buff[6], spi_recv_buff[7]))/(double)1000;
                Kd = (ToDec(spi_recv_buff[8], spi_recv_buff[9]))/(double)1000;
                Kz = spi_recv_buff[10];
                posPID.setGains(Kp,Ki,Kd);




                sendSPIPacket(spi_recv_buff);
                break;

            case 26:
                LED(3);
                spi_send_buff[1] = 1;
                spi_send_buff[2] = 5;
                spi_send_buff[3] = 3;

                encoder_pos = Enc1.read();
                setpoint = ToDec(spi_recv_buff[4], spi_recv_buff[5]);
                integralZone(setpoint,encoder_pos, Kz);
                posPID.run();
                if(((output)>0?(output):-(output)) != output){
                  output = output*-1;
                  Mset(1,output);
                }else{
                  Mset(2,output);
                }
                decTo256(((encoder_pos)>0?(encoder_pos):-(encoder_pos)));
                if(((encoder_pos)>0?(encoder_pos):-(encoder_pos)) < 256){
                  data_array[1] = 0;
                }
                //spi_send_buff[4] = output;
                spi_send_buff[4] = data_array[0];
                spi_send_buff[5] = data_array[1];
                spi_send_buff[6] = negitive_check(encoder_pos);

                sendSPIPacket(spi_send_buff);
                break;


            case 27:
                LED(3);
                spi_send_buff[1] = 1;
                spi_send_buff[2] = 5;
                spi_send_buff[3] = 2;

                velocity = calculateSpeed();

                spi_send_buff[4] = ((velocity)>0?(velocity):-(velocity));
                spi_send_buff[5] = negitive_check(velocity);

                sendSPIPacket(spi_send_buff);
                break;

            case 28:
                LED(3);
                spi_send_buff[1] = 1;
                spi_send_buff[2] = 5;
                spi_send_buff[3] = 2;

                velocity = calculateSpeed();


                setpoint = spi_recv_buff[4];
                speedPID.run();
                if(((speed_output)>0?(speed_output):-(speed_output)) != speed_output){
                  speed_output = speed_output*-1;
                  Mset(1,speed_output);
                }else{
                  Mset(2,speed_output);
                }

                spi_send_buff[4] = ((velocity)>0?(velocity):-(velocity));
                spi_send_buff[5] = negitive_check(velocity);


                sendSPIPacket(spi_send_buff);
                break;



            case 9:
                if(spi_recv_buff[4]==0){
                    servos[spi_recv_buff[4]].writeMicroseconds(convertToPWM(ToDec(spi_recv_buff[5], spi_recv_buff[6]),servo_1_min_angle,servo_1_max_angle,servo_1_min_microseconds,servo_1_max_microseconds));
                }
                if(spi_recv_buff[4]==1){
                    servos[spi_recv_buff[4]].writeMicroseconds(convertToPWM(ToDec(spi_recv_buff[5], spi_recv_buff[6]),servo_2_min_angle,servo_2_max_angle,servo_2_min_microseconds,servo_2_max_microseconds));
                }
                sendSPIPacket(spi_recv_buff);
                break;

            case 11:
                int servo_min_angle = ToDec(spi_recv_buff[5], spi_recv_buff[6]);
                int servo_max_angle = ToDec(spi_recv_buff[7], spi_recv_buff[8]);
                int servo_min_microseconds = ToDec(spi_recv_buff[9], spi_recv_buff[10]);
                int servo_max_microseconds = ToDec(spi_recv_buff[11], spi_recv_buff[12]);
                if(spi_recv_buff[4]==0){
                    servo_1_min_angle = servo_min_angle;
                    servo_1_max_angle = servo_max_angle;
                    servo_1_min_microseconds = servo_min_microseconds;
                    servo_1_max_microseconds = servo_max_microseconds;

                }
                if(spi_recv_buff[4]==1){
                    servo_2_min_angle = servo_min_angle;
                    servo_2_max_angle = servo_max_angle;
                    servo_2_min_microseconds = servo_min_microseconds;
                    servo_2_max_microseconds = servo_max_microseconds;
                }
                sendSPIPacket(spi_recv_buff);
                break;

            default:
                break;
        }
    }else{
        LED(1);
    }
}
