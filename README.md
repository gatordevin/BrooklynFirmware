# BrooklynFirmware
Repo containing firmware for the main Brooklyn board in addition to all daughter card sub types

## How To Use
### Setup
#### Brooklyn Board
1. Plug the USBTinyISP into your computer
2. Launch the Arduino IDE
3. Under tools change **Board** to Arduino Leonardo
4. If not already selected select the port your programmer shows up to under tools and **Ports**
5. Under **Programmer** select USBTinyISP
6. With no Empire Board slotted push the pogo pins of the USBTinyISP with the pogo pin extensions onto the brooklyn ISP pins behind the vertical USB terminal (Chip should be over the card slot).
7. From tools press the **Burn Bootloader** Button and keep pressure on the pogo pins until it is completed
8. Then unplug the USBTinyISP and plug in the Brooklyn to your cumputer via the vertical USB port on the right side of the board
9. Power the board using the screw terminal on the other end and any 12V battery (Make note of proper polarity).
10. Git Clone this repository containing the firmware you need to flash your board with
11. Navigate to and open the BrooklynFirmware.ino file in the Arduino or VSCode IDE
###### Using Arduino IDE
1. Under tools change **Board** to Arduino Leonardo
2. If not already selected select the port your Arduino shows up to under tools and **Ports**
3. Under **Programmer** select AVRISP mk11
4. From the top menu bar hit the **Upload** button
5. The Brooklyn is now flashed with proper firmware and should be able to communicate with the API and the Empire Boards
###### Using VS Code
**COMING SOON**
#### Empire Board
1. Disconnect 12V power from the flashed Brooklyn Board
2. Push the Empire Board into the left closest slot when looking fromthe USB port side or where you see M1 and M2 printed on the board (Apply a lot of force).
3. Connect the Brooklyn back to 12V
4. Launch the Arduino IDE
5. Under tools change **Board** to Arduino Uno
6. If not already selected select the port your Arduino shows up to under tools and **Ports**
7. Under **Programmer** select Arduino as ISP (ATMega32U4)
8. From tools press the **Burn Bootloader** Button
9. Navigate to and open the EmpireFirmware.ino file in the Arduino or VSCode IDE
###### Using Arduino IDE
1. Under tools change **Board** to Arduino Uno
2. If not already selected select the port your Arduino shows up to under tools and **Ports**
3. Under **Programmer** select Arduino as ISP (ATMega32U4)
4. Hold the **shift** button on your keyboard and hit the **Upload** button on the top menu bar
5. The Empire is now flashed with proper firmware and should be able to communicate with the API and the Empire Boards
6. Note that using the Arduino IDE to flash only allows you to program one chip we may work this issue out but for now that is the case
###### Using VS Code
**COMING SOON**
### Programming
The new firmware includes the use of a Serial, SPIMaster, and SPISlave class which is supported by a Comms Abstract class containing all the lower level protocols and managment required for both sending and receiving packets of data using Arduino. The Serial, SPIMaster, and SPISlave class simply provides the protocol specific means of communication meaning it is possible to create other classes for different types of communication extremly easily by simply extending the Comms abstract class. The abstract class is designed to keep track of things like checksum errors, packet length errors, read timeouts, etc but it is up to the class developer to add in those safety features and safety checks into their class. Currently this is included on the Serial class which is prone to disconnects or missed bytes. The SPI classes however are typicially robust so currently do not include such safety measure but they will in a later commit. Basic Usage of a class should be as follows.

#### Example Program
```
#include "SerialComms.h"


SerialComms computer = SerialComms(9600, 250);

void setup(){
    computer.begin();  
}

int mode = 0;
void loop(){
    while(mode==0){
        mode = computer.check_for_conn(140);
    }
    computer.check_for_data(&controller_switch, &background_func, &reset_board);
}

void controller_switch(){
    switch(computer.recv_packet.command){
        case 1:
            float data = computer.read_float();
            computer.copy_packet_data(computer.recv_packet);
            computer.send(0);
            break;
        case 2:
            long data = computer.read_unsigned_int();
            computer.copy_packet_data(computer.recv_packet);
            computer.send(0);
            break;
        case 3:
            long long data = computer.read_long_long();
            computer.copy_packet_data(computer.recv_packet);
            computer.send(0);
            break;
        case 140:
            computer.close(&reset_board);
            break;
        default:
            computer.copy_packet_data(computer.recv_packet);
            computer.send(0);
            break;
    }
}

void background_func(){
    //Anything you want to run in the background or after data is recieved
}

void reset_board(){
    mode = 0;
}
```

#### Code Block Explabnations
```#include "SerialComms.h"```
This line of code is just specifying a path to a .h file and .cpp file that contain your class and will be used in the program. Typically you see inlclude statments on Arduino done with <> instead of "" but the qoute simply indicate you are referrring to a local path isntead of an Arduino library either would suffice you would just need to include your class as a library in the Arduino IDE.

```SerialComms computer = SerialComms(9600, 250);```
This line of code creates an instance of your class in this case the SerialComms class using any paramters you would like to specify. The SerialComms class has two required paramaters, baudrate and read and write timeout. For the Serial class only one instance can be created at the moment because it is hard code to use the Serial class created by the Arduino.h file. Eventually this class will take in Serial port number as a paramter so you could then use the Serial1 class created by the Arduino.h file as well allowing for communication with multiple computers.

```
void setup(){
    computer.begin();  
}
```
This is the setup function which is required in any Arduino program. It happens only once on bootup of the board and in this case we are simply starting the Serial class with the baudrate we specified above just getting it ready nothing more.

```
int mode = 0;
void loop(){
    while(mode==0){
        mode = computer.check_for_conn(140);
    }
    computer.check_for_data(&controller_switch, &background_func, &reset_board);
}
```
This block of code serves two functions. First it waits in the while loop until the handshake of the 140 byte is complete which is indicated by a change in mode. This can also be used for differernt kinds of handshakes for example if you send a 150 instead just add that as a mode and the Brooklyn could behave differerntly. Once mode is not zero it will then begin checking for data. This check_for_data function takes in three functions to serve as event handlers. The first to serve as a handler for when there is data. The second is run after data is recieved or when no data is avaiable so this is where anything housekeeping realted is located. The last function is for when a large watchdog timer is exceeeded indicated the computer disconnected or heartbeat commands are no longer being sent and that it should go back to idle mode. The reset function will handle all clean up and switching the mode back to zero.

```
switch(computer.recv_packet.command){
    case 1:
        float data = computer.read_float();
        computer.copy_packet_data(computer.recv_packet);
        computer.send(0);
        break;
```
Inside the controller switch function we have a series of switch case statments that allow us to different things depedening on what data is sent. At this point all data that was sent is currently in the computer.recv_packet structure object. We can reference differernt values that were sent by changing the last word from for example command to data_length. This is how any interfacing of data should be followed. If creatingyour own class this same method of data reading and storage should be used to keep things consistent. The computer.read_float() function simply gives you the float at your current index in the data array then adds four to your current read position. Its because of this behavior you can only read a number once before it moves on. This is to account for the varaible size of differernt data types floats are bytes, shorts are two bytes, long longs are eight bytes, etc. By only reading once it allows us to keep track of and know where the next number should begin in the data array. One way around this is sending a signifier byte indicating the type of variable before the start of it but it has not been implemented yet. The computer.copy_packet_data function simply copies data from the structured you passed in over to the send_packet object for whatever class it was called from. In this example im copying the recv data to the send data which will just send a mirror packet back to the computer. Typically you would copy the packet sent back from the empire recived via the SPIMaster class. Finally the computer.send function will send the constructed packet to the computer as a response allowing the API to move on. If this is not run the watchdog timer on the API side will fail. If you are designing your own class using the base abstract class provided your code should function identicially to this block here so that coding style remains the same across communication types.

```
void background_func(){
    //Anything you want to run in the background or after data is recieved
}
```
This function will get called after data is handled or when no data is avaiable. It can be used for housekeeping operations on example is status reports and communication with all the Empire board plugged in so when the API sends a heartbeat the brooklyn can respond with information about the system which si a very useful feature for both debugging and deployment.

```
void reset_board(){
    mode = 0;
}
```
This function is called when modes are switched on a graceful disconnect by the API or triggered as the result of a watchdog timer runoput indicated the computer has most likely disconnected from the board. it will then switch to idle mode and preapre for a reconnect which the API should initiate once plugged back in.

## Packet Structure
9 bytes long not including data

byte 1 - header

byte 2 - destination ID

byte 3 - sender ID

byte 4 - packet ID
byte 5 - sender ID
byte 6 - sender ID
byte 7 - sender ID

byte 8 - command ID

byte 9 - data length

byte 10 - checksum
byte 11 - checksum
byte 12 - checksum
byte 13 - checksum

byte 14-->i - data
