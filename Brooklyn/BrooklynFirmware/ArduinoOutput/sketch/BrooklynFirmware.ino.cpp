#line 1 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
// ArduinoISP
// Copyright (c) 2008-2011 Randall Bohn
// If you require a license, see
// http://www.opensource.org/licenses/bsd-license.php
//
// This sketch turns the Arduino into a AVRISP using the following Arduino pins:
//
// Pin 10 is used to reset the target microcontroller.
//
// By default, the hardware SPI pins MISO, MOSI and SCK are used to communicate
// with the target. On all Arduinos, these pins can be found
// on the ICSP/SPI header:
//
//               MISO °. . 5V (!) Avoid this pin on Due, Zero...
//               SCK   . . MOSI
//                     . . GND
//
// On some Arduinos (Uno,...), pins MOSI, MISO and SCK are the same pins as
// digital pin 11, 12 and 13, respectively. That is why many tutorials instruct
// you to hook up the target to these pins. If you find this wiring more
// practical, have a define USE_OLD_STYLE_WIRING. This will work even when not
// using an Uno. (On an Uno this is not needed).
//
// Alternatively you can use any other digital pin by configuring
// software ('BitBanged') SPI and having appropriate defines for PIN_MOSI,
// PIN_MISO and PIN_SCK.
//
// IMPORTANT: When using an Arduino that is not 5V tolerant (Due, Zero, ...) as
// the programmer, make sure to not expose any of the programmer's pins to 5V.
// A simple way to accomplish this is to power the complete system (programmer
// and target) at 3V3.
//
// Put an LED (with resistor) on the following pins:
// 9: Heartbeat   - shows the programmer is running
// 8: Error       - Lights up if something goes wrong (use red if that makes sense)
// 7: Programming - In communication with the slave
//

#include "Arduino.h"
//#undef SERIAL


#define PROG_FLICKER true

// Configure SPI clock (in Hz).
// E.g. for an ATtiny @ 128 kHz: the datasheet states that both the high and low
// SPI clock pulse must be > 2 CPU cycles, so take 3 cycles i.e. divide target
// f_cpu by 6:
//     #define SPI_CLOCK            (128000/6)
//
// A clock slow enough for an ATtiny85 @ 1 MHz, is a reasonable default:

#define SPI_CLOCK 		(1000000/6)


// Select hardware or software SPI, depending on SPI clock.
// Currently only for AVR, for other architectures (Due, Zero,...), hardware SPI
// is probably too fast anyway.

#if defined(ARDUINO_ARCH_AVR)



#endif
#if SPI_CLOCK > (F_CPU / 128)
#define USE_HARDWARE_SPI
#endif
// Configure which pins to use:

// The standard pin configuration.
#ifndef ARDUINO_HOODLOADER2

// Uncomment following line to use the old Uno style wiring
// (using pin 11, 12 and 13 instead of the SPI header) on Leonardo, Due...

// #define USE_OLD_STYLE_WIRING

#ifdef USE_OLD_STYLE_WIRING

#define PIN_MOSI	11
#define PIN_MISO	12
#define PIN_SCK		13

#endif

// HOODLOADER2 means running sketches on the ATmega16U2 serial converter chips
// on Uno or Mega boards. We must use pins that are broken out:
#else


#endif

// By default, use hardware SPI pins:
#ifndef PIN_MOSI
#define PIN_MOSI 	MOSI
#endif

#ifndef PIN_MISO
#define PIN_MISO 	MISO
#endif

#ifndef PIN_SCK
#define PIN_SCK 	SCK
#endif

#define RED 1
#define BLUE 2
#define GREEN 3
#define OFF 4

#define PROGRAMMER 1
#define CONTROLLER 2

#define red_pin 7
#define blue_pin 11
#define green_pin A5

// Force bitbanged SPI if not using the hardware SPI pins:
#if (PIN_MISO != MISO) ||  (PIN_MOSI != MOSI) || (PIN_SCK != SCK)
#undef USE_HARDWARE_SPI
#endif


// Configure the serial port to use.
//
// Prefer the USB virtual serial port (aka. native USB port), if the Arduino has one:
//   - it does not autoreset (except for the magic baud rate of 1200).
//   - it is more reliable because of USB handshaking.
//
// Leonardo and similar have an USB virtual serial port: 'Serial'.
// Due and Zero have an USB virtual serial port: 'SerialUSB'.
//
// On the Due and Zero, 'Serial' can be used too, provided you disable autoreset.
// To use 'Serial': #define SERIAL Serial

// #ifdef SERIAL_PORT_USBVIRTUAL
// #define SERIAL SERIAL_PORT_USBVIRTUAL
// #else
// #define SERIAL Serial
// #endif
#define SERIAL Serial

// Configure the baud rate:

#define BAUDRATE	19200
// #define BAUDRATE	115200
// #define BAUDRATE	1000000


#define HWVER 2
#define SWMAJ 1
#define SWMIN 18

// STK Definitions
#define STK_OK      0x10
#define STK_FAILED  0x11
#define STK_UNKNOWN 0x12
#define STK_INSYNC  0x14
#define STK_NOSYNC  0x15
#define CRC_EOP     0x20 //ok it is a space...

void pulse(int pin, int times);

#ifdef USE_HARDWARE_SPI
#include "SPI.h"
#define SPI_MODE0 0x00
#else



class SPISettings {
  public:
    // clock is in Hz
    SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode) : clock(clock) {
      (void) bitOrder;
      (void) dataMode;
    };

  private:
    uint32_t clock;

    friend class BitBangedSPI;
};

class BitBangedSPI {
  public:
    void begin() {
      digitalWrite(PIN_SCK, LOW);
      digitalWrite(PIN_MOSI, LOW);
      pinMode(PIN_SCK, OUTPUT);
      pinMode(PIN_MOSI, OUTPUT);
      pinMode(PIN_MISO, INPUT);
    }

    void beginTransaction(SPISettings settings) {
      pulseWidth = (500000 + settings.clock - 1) / settings.clock;
      if (pulseWidth == 0)
        pulseWidth = 1;
    }

    void end() {}

    uint8_t transfer (uint8_t b) {
      for (unsigned int i = 0; i < 8; ++i) {
        digitalWrite(PIN_MOSI, (b & 0x80) ? HIGH : LOW);
        digitalWrite(PIN_SCK, HIGH);
        delayMicroseconds(pulseWidth);
        b = (b << 1) | digitalRead(PIN_MISO);
        digitalWrite(PIN_SCK, LOW); // slow pulse
        delayMicroseconds(pulseWidth);
      }
      return b;
    }

  private:
    unsigned long pulseWidth; // in microseconds
};

static BitBangedSPI SPIbit;

#endif


#line 224 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
void LED(uint8_t color);
#line 261 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
void setup();
#line 305 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
void reset_target(bool reset);
#line 310 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
void loop(void);
#line 343 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
bool readSerialPacket();
#line 363 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
void sendSerialPacket(uint8_t send_buff[]);
#line 376 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
uint8_t readByte();
#line 382 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
void CopySerToSPI();
#line 388 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
void CopySPIToSer();
#line 394 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
uint8_t SPISend(uint8_t SSpin, uint8_t data);
#line 402 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
bool SPISendPacket(uint8_t SSpin);
#line 417 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
bool SPIRecvPacket(uint8_t SSpin);
#line 435 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
bool verifyChecksum(uint8_t recv_buff[]);
#line 445 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
void calculateChecksum(uint8_t data_buff[]);
#line 458 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
uint8_t getch();
#line 462 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
void fill(int n);
#line 479 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
uint8_t spi_transaction(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
#line 486 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
void empty_reply();
#line 496 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
void breply(uint8_t b);
#line 507 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
void get_version(uint8_t c);
#line 526 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
void set_parameters();
#line 553 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
void start_pmode();
#line 583 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
void start_cmode();
#line 588 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
void end_pmode();
#line 599 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
void end_cmode();
#line 603 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
void universal();
#line 611 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
void flash(uint8_t hilo, unsigned int addr, uint8_t data);
#line 617 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
void commit(unsigned int addr);
#line 626 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
unsigned int current_page();
#line 643 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
void write_flash(int length);
#line 654 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
uint8_t write_flash_pages(int length);
#line 673 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
uint8_t write_eeprom(unsigned int length);
#line 690 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
uint8_t write_eeprom_chunk(unsigned int start, unsigned int length);
#line 701 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
void program_page();
#line 726 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
uint8_t flash_read(uint8_t hilo, unsigned int addr);
#line 733 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
char flash_read_page(int length);
#line 744 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
char eeprom_read_page(int length);
#line 755 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
void read_page();
#line 771 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
void read_signature();
#line 792 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
void avrisp();
#line 888 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
void controller_switch();
#line 224 "/home/techgarage/BrooklynFirmware/Brooklyn/BrooklynFirmware/BrooklynFirmware.ino"
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

uint8_t reset_pins[] = {A2, A3, 3, 2, 13, 5, 8, 6};
uint8_t ss[] = {A1, A0, 0, 1, 10, 9, 12, 4};
uint8_t RESET = 0;

uint8_t ser_recv_buff[20];
uint8_t ser_send_buff[20];
uint8_t spi_recv_buff[20];
uint8_t spi_send_buff[20];

uint8_t checksum1 = 0;
uint8_t checksum2 = 0;

void setup() {
  SERIAL.begin(1000000);
  pinMode(red_pin, OUTPUT);
  pinMode(blue_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);
  LED(BLUE);
  for(int i=0;i<8;i++){
        pinMode(ss[i], OUTPUT);
        digitalWrite(ss[i], HIGH);
    }
}

int error = 0;
int pmode = 0;
// address for reading and writing, set by 'U' command
unsigned int here;
uint8_t buff[256]; // global block storage

#define beget16(addr) (*addr * 256 + *(addr+1) )
typedef struct param {
  uint8_t devicecode;
  uint8_t revision;
  uint8_t progtype;
  uint8_t parmode;
  uint8_t polling;
  uint8_t selftimed;
  uint8_t lockbytes;
  uint8_t fusebytes;
  uint8_t flashpoll;
  uint16_t eeprompoll;
  uint16_t pagesize;
  uint16_t eepromsize;
  uint32_t flashsize;
}
parameter;

parameter param;

// this provides a heartbeat on pin 9, so you can tell the software is running.
uint8_t hbval = 128;
int8_t hbdelta = 8;

static bool rst_active_high;

void reset_target(bool reset) {
  digitalWrite(RESET, ((reset && rst_active_high) || (!reset && !rst_active_high)) ? HIGH : LOW);
}
uint8_t resp = 0;
uint8_t mode = 0;
void loop(void) {
  if(mode == 0){
    LED(BLUE);
    switch(getch()){
      case 120:
        resp = getch();
        RESET = reset_pins[resp];
        mode = PROGRAMMER;
        LED(RED);
        break;
      case 140:
        start_cmode();
        SERIAL.write(140);
        mode = CONTROLLER;
        LED(GREEN);
        break;
      case '0':
        RESET = reset_pins[0];
        mode = PROGRAMMER;
        LED(RED);
        break;
    }
  }
  switch(mode){
    case PROGRAMMER:
      avrisp();
      break;
    case CONTROLLER:
      controller_switch();
      break;
  }
}

bool readSerialPacket(){
    ser_recv_buff[0] = 0;
    while(ser_recv_buff[0] != 255){
        ser_recv_buff[0] = readByte(); //header 255
        if(ser_recv_buff[0]==170){
          return false;
        }
    }
    ser_recv_buff[1] = readByte(); //controller ID
    ser_recv_buff[2] = readByte(); //controller command
    ser_recv_buff[3] = readByte(); //data length
    for(int i=0;i<ser_recv_buff[3];i++){
        ser_recv_buff[i+4] = readByte(); //data bytes
    }
    ser_recv_buff[ser_recv_buff[3]+4] = readByte(); //checksum 1
    ser_recv_buff[ser_recv_buff[3]+5] = readByte(); //checksum 2
    
    return(verifyChecksum(ser_recv_buff)); //return whether data was received succesfully
}

void sendSerialPacket(uint8_t send_buff[]){
    Serial.write(send_buff[0]); //send header 255
    Serial.write(send_buff[1]); //send ID
    Serial.write(send_buff[2]); //send command
    Serial.write(send_buff[3]); //send data length
    for(int i=0;i<send_buff[3];i++){
        Serial.write(send_buff[i+4]); //send data bytes
    }
    calculateChecksum(send_buff);
    Serial.write(checksum1); //send checksum 1
    Serial.write(checksum2); //send checksum 2
}

uint8_t readByte(){
  while(!Serial.available()){} //wait until serial avaiable
  uint8_t data = Serial.read(); //read byte
  return(data);
}

void CopySerToSPI(){
    for(int i=0;i<20;i++){
        spi_send_buff[i] = ser_recv_buff[i]; //copy data
    }
}

void CopySPIToSer(){
    for(int i=0;i<20;i++){
        ser_send_buff[i] = spi_recv_buff[i]; //copy data
    }
}

uint8_t SPISend(uint8_t SSpin, uint8_t data){
    digitalWrite(SSpin, LOW);
    uint8_t resp = SPI.transfer(data);
    digitalWrite(SSpin, HIGH);
    delayMicroseconds(100);
    return resp;
}

bool SPISendPacket(uint8_t SSpin){
    SPISend(SSpin,spi_send_buff[0]); //Header
    SPISend(SSpin,spi_send_buff[1]); //ID
    SPISend(SSpin,spi_send_buff[2]); //Command
    SPISend(SSpin,spi_send_buff[3]); //Data Length
    for(int i=0;i<spi_send_buff[3];i++){
        SPISend(SSpin,spi_send_buff[4+i]);
    }
    calculateChecksum(spi_send_buff);
    SPISend(SSpin, checksum1);
    SPISend(SSpin, checksum2);

    return(SPIRecvPacket(SSpin));
}

bool SPIRecvPacket(uint8_t SSpin){
    delayMicroseconds(300);
    spi_recv_buff[0] = SPISend(SSpin,0); //read header
    spi_recv_buff[1] = SPISend(SSpin,0); //read id
    spi_recv_buff[2] = SPISend(SSpin,0); //read cmd
    spi_recv_buff[3] = SPISend(SSpin,0); //read data length
    for(int i=0;i<spi_recv_buff[3];i++){
        spi_recv_buff[4+i] = SPISend(SSpin,0); //read data
    }
    spi_recv_buff[spi_recv_buff[3]+4] = SPISend(SSpin,0);
    spi_recv_buff[spi_recv_buff[3]+5] = SPISend(SSpin,0);

    if(verifyChecksum(spi_recv_buff)==true){
        return true;
    }
    return false;
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

uint8_t getch() {
  while (!Serial.available());
  return Serial.read();
}
void fill(int n) {
  for (int x = 0; x < n; x++) {
    buff[x] = getch();
  }
}

#define PTIME 30
void pulse(int pin, int times) {
  do {
    digitalWrite(pin, HIGH);
    delay(PTIME);
    digitalWrite(pin, LOW);
    delay(PTIME);
  } while (times--);
}


uint8_t spi_transaction(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
  SPI.transfer(a);
  SPI.transfer(b);
  SPI.transfer(c);
  return SPI.transfer(d);
}

void empty_reply() {
  if (CRC_EOP == getch()) {
    Serial.print((char)STK_INSYNC);
    Serial.print((char)STK_OK);
  } else {
    error++;
    Serial.print((char)STK_NOSYNC);
  }
}

void breply(uint8_t b) {
  if (CRC_EOP == getch()) {
    Serial.print((char)STK_INSYNC);
    Serial.print((char)b);
    Serial.print((char)STK_OK);
  } else {
    error++;
    Serial.print((char)STK_NOSYNC);
  }
}

void get_version(uint8_t c) {
  switch (c) {
    case 0x80:
      breply(HWVER);
      break;
    case 0x81:
      breply(SWMAJ);
      break;
    case 0x82:
      breply(SWMIN);
      break;
    case 0x93:
      breply('S'); // serial programmer
      break;
    default:
      breply(0);
  }
}

void set_parameters() {
  // call this after reading parameter packet into buff[]
  param.devicecode = buff[0];
  param.revision   = buff[1];
  param.progtype   = buff[2];
  param.parmode    = buff[3];
  param.polling    = buff[4];
  param.selftimed  = buff[5];
  param.lockbytes  = buff[6];
  param.fusebytes  = buff[7];
  param.flashpoll  = buff[8];
  // ignore buff[9] (= buff[8])
  // following are 16 bits (big endian)
  param.eeprompoll = beget16(&buff[10]);
  param.pagesize   = beget16(&buff[12]);
  param.eepromsize = beget16(&buff[14]);

  // 32 bits flashsize (big endian)
  param.flashsize = buff[16] * 0x01000000
                    + buff[17] * 0x00010000
                    + buff[18] * 0x00000100
                    + buff[19];

  // AVR devices have active low reset, AT89Sx are active high
  rst_active_high = (param.devicecode >= 0xe0);
}

void start_pmode() {

  // Reset target before driving PIN_SCK or PIN_MOSI

  // SPI.begin() will configure SS as output, so SPI master mode is selected.
  // We have defined RESET as pin 10, which for many Arduinos is not the SS pin.
  // So we have to configure RESET as output here,
  // (reset_target() first sets the correct level)
  reset_target(true);
  pinMode(RESET, OUTPUT);
  SPI.begin();
  SPI.beginTransaction(SPISettings(SPI_CLOCK, MSBFIRST, SPI_MODE0));

  // See AVR datasheets, chapter "SERIAL_PRG Programming Algorithm":

  // Pulse RESET after PIN_SCK is low:
  digitalWrite(PIN_SCK, LOW);
  delay(20); // discharge PIN_SCK, value arbitrarily chosen
  reset_target(false);
  // Pulse must be minimum 2 target CPU clock cycles so 100 usec is ok for CPU
  // speeds above 20 KHz
  delayMicroseconds(100);
  reset_target(true);

  // Send the enable programming command:
  delay(50); // datasheet: must be > 20 msec
  spi_transaction(0xAC, 0x53, 0x00, 0x00);
  pmode = 1;
}

void start_cmode(){
  pinMode(MOSI, OUTPUT);
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK);
}
void end_pmode() {
  SPI.end();
  // We're about to take the target out of reset so configure SPI pins as input
  // pinMode(MOSI, INPUT);
  reset_target(false);
  pinMode(RESET, INPUT);
  pmode = 0;
  RESET = 0;
  mode = 0;
}

void end_cmode(){
  SPI.end();
  mode = 0;
}
void universal() {
  uint8_t ch;

  fill(4);
  ch = spi_transaction(buff[0], buff[1], buff[2], buff[3]);
  breply(ch);
}

void flash(uint8_t hilo, unsigned int addr, uint8_t data) {
  spi_transaction(0x40 + 8 * hilo,
                  addr >> 8 & 0xFF,
                  addr & 0xFF,
                  data);
}
void commit(unsigned int addr) {
  if (PROG_FLICKER) {
  }
  spi_transaction(0x4C, (addr >> 8) & 0xFF, addr & 0xFF, 0);
  if (PROG_FLICKER) {
    delay(PTIME);
  }
}

unsigned int current_page() {
  if (param.pagesize == 32) {
    return here & 0xFFFFFFF0;
  }
  if (param.pagesize == 64) {
    return here & 0xFFFFFFE0;
  }
  if (param.pagesize == 128) {
    return here & 0xFFFFFFC0;
  }
  if (param.pagesize == 256) {
    return here & 0xFFFFFF80;
  }
  return here;
}


void write_flash(int length) {
  fill(length);
  if (CRC_EOP == getch()) {
    Serial.print((char) STK_INSYNC);
    Serial.print((char) write_flash_pages(length));
  } else {
    error++;
    Serial.print((char) STK_NOSYNC);
  }
}

uint8_t write_flash_pages(int length) {
  int x = 0;
  unsigned int page = current_page();
  while (x < length) {
    if (page != current_page()) {
      commit(page);
      page = current_page();
    }
    flash(LOW, here, buff[x++]);
    flash(HIGH, here, buff[x++]);
    here++;
  }

  commit(page);

  return STK_OK;
}

#define EECHUNK (32)
uint8_t write_eeprom(unsigned int length) {
  // here is a word address, get the byte address
  unsigned int start = here * 2;
  unsigned int remaining = length;
  if (length > param.eepromsize) {
    error++;
    return STK_FAILED;
  }
  while (remaining > EECHUNK) {
    write_eeprom_chunk(start, EECHUNK);
    start += EECHUNK;
    remaining -= EECHUNK;
  }
  write_eeprom_chunk(start, remaining);
  return STK_OK;
}
// write (length) bytes, (start) is a byte address
uint8_t write_eeprom_chunk(unsigned int start, unsigned int length) {
  // this writes byte-by-byte, page writing may be faster (4 bytes at a time)
  fill(length);
  for (unsigned int x = 0; x < length; x++) {
    unsigned int addr = start + x;
    spi_transaction(0xC0, (addr >> 8) & 0xFF, addr & 0xFF, buff[x]);
    delay(45);
  }
  return STK_OK;
}

void program_page() {
  char result = (char) STK_FAILED;
  unsigned int length = 256 * getch();
  length += getch();
  char memtype = getch();
  // flash memory @here, (length) bytes
  if (memtype == 'F') {
    write_flash(length);
    return;
  }
  if (memtype == 'E') {
    result = (char)write_eeprom(length);
    if (CRC_EOP == getch()) {
      Serial.print((char) STK_INSYNC);
      Serial.print(result);
    } else {
      error++;
      Serial.print((char) STK_NOSYNC);
    }
    return;
  }
  Serial.print((char)STK_FAILED);
  return;
}

uint8_t flash_read(uint8_t hilo, unsigned int addr) {
  return spi_transaction(0x20 + hilo * 8,
                         (addr >> 8) & 0xFF,
                         addr & 0xFF,
                         0);
}

char flash_read_page(int length) {
  for (int x = 0; x < length; x += 2) {
    uint8_t low = flash_read(LOW, here);
    Serial.print((char) low);
    uint8_t high = flash_read(HIGH, here);
    Serial.print((char) high);
    here++;
  }
  return STK_OK;
}

char eeprom_read_page(int length) {
  // here again we have a word address
  int start = here * 2;
  for (int x = 0; x < length; x++) {
    int addr = start + x;
    uint8_t ee = spi_transaction(0xA0, (addr >> 8) & 0xFF, addr & 0xFF, 0xFF);
    Serial.print((char) ee);
  }
  return STK_OK;
}

void read_page() {
  char result = (char)STK_FAILED;
  int length = 256 * getch();
  length += getch();
  char memtype = getch();
  if (CRC_EOP != getch()) {
    error++;
    Serial.print((char) STK_NOSYNC);
    return;
  }
  Serial.print((char) STK_INSYNC);
  if (memtype == 'F') result = flash_read_page(length);
  if (memtype == 'E') result = eeprom_read_page(length);
  Serial.print(result);
}

void read_signature() {
  if (CRC_EOP != getch()) {
    error++;
    Serial.print((char) STK_NOSYNC);
    return;
  }
  Serial.print((char) STK_INSYNC);
  uint8_t high = spi_transaction(0x30, 0x00, 0x00, 0x00);
  Serial.print((char) high);
  uint8_t middle = spi_transaction(0x30, 0x00, 0x01, 0x00);
  Serial.print((char) middle);
  uint8_t low = spi_transaction(0x30, 0x00, 0x02, 0x00);
  Serial.print((char) low);
  Serial.print((char) STK_OK);
}
//////////////////////////////////////////
//////////////////////////////////////////


////////////////////////////////////
////////////////////////////////////
void avrisp() {
  uint8_t ch = getch();
  switch (ch) {
    case '0': // signon
      error = 0;
      empty_reply();
      break;
    case '1':
      if (getch() == CRC_EOP) {
        Serial.print((char) STK_INSYNC);
        Serial.print("AVR ISP");
        Serial.print((char) STK_OK);
      }
      else {
        error++;
        Serial.print((char) STK_NOSYNC);
      }
      break;
    case 'A':
      get_version(getch());
      break;
    case 'B':
      fill(20);
      set_parameters();
      empty_reply();
      break;
    case 'E': // extended parameters - ignore for now
      fill(5);
      empty_reply();
      break;
    case 'P':
      if (!pmode)
        start_pmode();
      empty_reply();
      break;
    case 'U': // set address (word)
      here = getch();
      here += 256 * getch();
      empty_reply();
      break;

    case 0x60: //STK_PROG_FLASH
      getch(); // low addr
      getch(); // high addr
      empty_reply();
      break;
    case 0x61: //STK_PROG_DATA
      getch(); // data
      empty_reply();
      break;

    case 0x64: //STK_PROG_PAGE
      program_page();
      break;

    case 0x74: //STK_READ_PAGE 't'
      read_page();
      break;

    case 'V': //0x56
      universal();
      break;
    case 'Q': //0x51
      error = 0;
      end_pmode();
      empty_reply();
      break;

    case 0x75: //STK_READ_SIGN 'u'
      read_signature();
      break;

    // expecting a command, not CRC_EOP
    // this is how we can get back in sync
    case CRC_EOP:
      error++;
      Serial.print((char) STK_NOSYNC);
      break;

    // anything else we will return STK_UNKNOWN
    default:
      error++;
      if (CRC_EOP == getch())
        Serial.print((char)STK_UNKNOWN);
      else
        Serial.print((char)STK_NOSYNC);
  }
}

#define CMD_HB 72
#define CMD_GET_ENCODER 24
#define CMD_SET_PWM 9
#define CMD_SET_SERVO_RANGE 11
#define CMD_GET_CARD_TYPE 3


void controller_switch(){
  if(readSerialPacket()){
        ser_send_buff[0] = 255; //Set serial send header
        switch(ser_recv_buff[2]){
            case CMD_HB:
                ser_send_buff[1] = 0;
                ser_send_buff[2] = 72;
                ser_send_buff[3] = 0; 
                sendSerialPacket(ser_send_buff);
                break;
            
            case CMD_GET_ENCODER: //In the case of an encoder request we copy over the request from the computer to the spi and send it to
                CopySerToSPI(); //the intended daughter card whcih was specificed in the packet
                if(SPISendPacket(ss[ser_recv_buff[1]-2])){ //The if statement then verifiwes the data was transferred properly by checking the checksum
                    CopySPIToSer();                         //If there was transfer success we can decide what to do which in this case is relay the information from the daugfhter card to the computer
                    sendSerialPacket(ser_send_buff);
                }else{
                    LED(RED);   //If it failed we can do somethign different like specify the error message as a cehcksum error and the computer will decide wether it wants to ask for that data again
                    ser_send_buff[1] = 0; //In the case of a checksum error we most likely would if its a different error such as encoder being out of range or somethign liek that we can solve it before asking again
                    ser_send_buff[2] = 2; //You can manually set the send buffer by cahnging these three values which sepcifiy the destination the command and the length of data in the packet
                    ser_send_buff[3] = 0; //Destiantion for computer is 0 destination for brooklyn is 1 and all empire cards are 2-10
                    sendSerialPacket(ser_recv_buff);
                }
                
                break;

            case CMD_SET_SERVO_RANGE: //In the case of an encoder request we copy over the request from the computer to the spi and send it to
                CopySerToSPI(); //the intended daughter card whcih was specificed in the packet
                if(SPISendPacket(ss[ser_recv_buff[1]-2])){ //The if statement then verifiwes the data was transferred properly by checking the checksum
                    CopySPIToSer();                         //If there was transfer success we can decide what to do which in this case is relay the information from the daugfhter card to the computer
                    sendSerialPacket(ser_send_buff);
                }else{
                    LED(RED);   //If it failed we can do somethign different like specify the error message as a cehcksum error and the computer will decide wether it wants to ask for that data again
                    ser_send_buff[1] = 0; //In the case of a checksum error we most likely would if its a different error such as encoder being out of range or somethign liek that we can solve it before asking again
                    ser_send_buff[2] = 2; //You can manually set the send buffer by cahnging these three values which sepcifiy the destination the command and the length of data in the packet
                    ser_send_buff[3] = 0; //Destiantion for computer is 0 destination for brooklyn is 1 and all empire cards are 2-10
                    sendSerialPacket(ser_recv_buff);
                }
                
                break;

            case CMD_GET_CARD_TYPE: //In the case of an encoder request we copy over the request from the computer to the spi and send it to
                CopySerToSPI(); //the intended daughter card whcih was specificed in the packet
                if(SPISendPacket(ss[ser_recv_buff[1]-2])){ //The if statement then verifiwes the data was transferred properly by checking the checksum
                    CopySPIToSer();                         //If there was transfer success we can decide what to do which in this case is relay the information from the daugfhter card to the computer
                    sendSerialPacket(ser_send_buff);
                }else{
                    LED(RED);   //If it failed we can do somethign different like specify the error message as a cehcksum error and the computer will decide wether it wants to ask for that data again
                    ser_send_buff[1] = 0; //In the case of a checksum error we most likely would if its a different error such as encoder being out of range or somethign liek that we can solve it before asking again
                    ser_send_buff[2] = 2; //You can manually set the send buffer by cahnging these three values which sepcifiy the destination the command and the length of data in the packet
                    ser_send_buff[3] = 0; //Destiantion for computer is 0 destination for brooklyn is 1 and all empire cards are 2-10
                    sendSerialPacket(ser_send_buff);
                }
                
                break;
            
            default:
                CopySerToSPI(); //the intended daughter card whcih was specificed in the packet
                if(SPISendPacket(ss[ser_recv_buff[1]-2])){ //The if statement then verifiwes the data was transferred properly by checking the checksum
                    CopySPIToSer();                         //If there was transfer success we can decide what to do which in this case is relay the information from the daugfhter card to the computer
                    sendSerialPacket(ser_send_buff);
                }else{
                    LED(RED);   //If it failed we can do somethign different like specify the error message as a cehcksum error and the computer will decide wether it wants to ask for that data again
                    ser_send_buff[1] = 0; //In the case of a checksum error we most likely would if its a different error such as encoder being out of range or somethign liek that we can solve it before asking again
                    ser_send_buff[2] = 2; //You can manually set the send buffer by cahnging these three values which sepcifiy the destination the command and the length of data in the packet
                    ser_send_buff[3] = 0; //Destiantion for computer is 0 destination for brooklyn is 1 and all empire cards are 2-10
                    sendSerialPacket(ser_send_buff);
                }
                
                break;
        }
    }else{
        if(ser_recv_buff[0]==170){
          end_cmode();
        }
    }
}
