# BrooklynFirmware
Repo containing firmware for the main Brooklyn board in addition to all daughter card sub types

### Packet Structure
9 bytes long not including data

byte 1 - header

byte 2 - destination ID

byte 3 - sender ID

byte 4 - command ID

byte 5 - data length

byte 6-->i-4 - data

byte i-3 - packet id LSB

byte i-2 - packet id MSB

byte i-1 - checksum LSB

byte i - checksum MSB

