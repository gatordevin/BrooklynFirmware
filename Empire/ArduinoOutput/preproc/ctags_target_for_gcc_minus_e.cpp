# 1 "/home/techgarage/BrooklynFirmware/Empire/test/test.ino"
# 2 "/home/techgarage/BrooklynFirmware/Empire/test/test.ino" 2
# 12 "/home/techgarage/BrooklynFirmware/Empire/test/test.ino"
uint8_t resp;

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


# 39 "/home/techgarage/BrooklynFirmware/Empire/test/test.ino" 3
extern "C" void __vector_17 /* SPI Serial Transfer Complete */ (void) __attribute__ ((signal,used, externally_visible)) ; void __vector_17 /* SPI Serial Transfer Complete */ (void)

# 40 "/home/techgarage/BrooklynFirmware/Empire/test/test.ino"
{
    resp = 
# 41 "/home/techgarage/BrooklynFirmware/Empire/test/test.ino" 3
          (*(volatile uint8_t *)((0x2E) + 0x20))
# 41 "/home/techgarage/BrooklynFirmware/Empire/test/test.ino"
              ;
    LED(2);
}

void setup(){
    pinMode(0, 0x1);
    pinMode(A2, 0x1);
    pinMode(A3, 0x1);
    pinMode(MISO,0x1);
    
# 50 "/home/techgarage/BrooklynFirmware/Empire/test/test.ino" 3
   (*(volatile uint8_t *)((0x2C) + 0x20)) 
# 50 "/home/techgarage/BrooklynFirmware/Empire/test/test.ino"
        |= 
# 50 "/home/techgarage/BrooklynFirmware/Empire/test/test.ino" 3
           (1 << (6))
# 50 "/home/techgarage/BrooklynFirmware/Empire/test/test.ino"
                   ;
    
# 51 "/home/techgarage/BrooklynFirmware/Empire/test/test.ino" 3
   (*(volatile uint8_t *)((0x2C) + 0x20)) 
# 51 "/home/techgarage/BrooklynFirmware/Empire/test/test.ino"
        &= ~(
# 51 "/home/techgarage/BrooklynFirmware/Empire/test/test.ino" 3
             (1 << (4))
# 51 "/home/techgarage/BrooklynFirmware/Empire/test/test.ino"
                      ); //Arduino is Slave
    
# 52 "/home/techgarage/BrooklynFirmware/Empire/test/test.ino" 3
   (*(volatile uint8_t *)((0x2E) + 0x20)) 
# 52 "/home/techgarage/BrooklynFirmware/Empire/test/test.ino"
        = 0x67; //test value
    
# 53 "/home/techgarage/BrooklynFirmware/Empire/test/test.ino" 3
   (*(volatile uint8_t *)((0x2C) + 0x20)) 
# 53 "/home/techgarage/BrooklynFirmware/Empire/test/test.ino"
        |= 
# 53 "/home/techgarage/BrooklynFirmware/Empire/test/test.ino" 3
           (1 << (7))
# 53 "/home/techgarage/BrooklynFirmware/Empire/test/test.ino"
                    ; //we not using SPI.attachInterrupt() why?
    
# 54 "/home/techgarage/BrooklynFirmware/Empire/test/test.ino" 3
   __asm__ __volatile__ ("sei" ::: "memory")
# 54 "/home/techgarage/BrooklynFirmware/Empire/test/test.ino"
        ;
    LED(1);
}

void loop(void){

}
