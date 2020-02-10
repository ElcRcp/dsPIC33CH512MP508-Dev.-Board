# Custom Made dsPIC33CH512MP508 v0.2 Development Board 
# Software Version 0.1

- Compiler: CCS C v5.091
- Schematics : Proteus 8.9

# Features

 - Battery backed RTC unit
 - 128x64 OLED screen for interface
 - 5 onboard input buttons
 - Additional RAM and EEPROM units
 - Fully controllable wireless module
 - USB powered
 - Selectable analog referance source
 - Selectable EEPROM i2c address
 - Selectable UART2 connection source
 - Dedicated UART1 connection and re-programmable uart-to-usb chip 
 - Programmable USB host option
 - All microcontroller pins are mapped out

# Onboard Devices 

 - Microcontroller : dsPIC33CH512MP508
 - SRAM :            23K640
 - EEPROM :          24LC256
 - Real Time Clock : DS1338Z-33+
 - Screen :          OLED 0.96" (I2C)
 - Relay :           HK23F-DC5V-SHG
 - USB Bridge Chip : CP2102-GMR
 - USB Host Chip :   VNC2-32Q

# Dedicate Microcontroller Pins

 - PE0         : O CP2102 Reset Pin
 - PE1         : I Input Button 1
 - PC14 (RP62) : O Connected to ESP RX pin
 - PC15 (RP63) : I Connected to ESP TX pin
 - PE2         : I Input Button 2
 - PE3         : I Input Button 3
 - PE4         : I Input Button 4
 - PE5         : I Input Button 5
 - PE6         : O Relay Control
 - PE7         : O ESP Enable and reset control
 - PC8  (RP56) : C SPI 2nd. channel SCK pin
 - PC9  (RP57) : C SPI 2nd. channel SDI pin
 - PD9         : O 23K640 SRAM SPI CS pin 
 - PD8         : C SPI 2nd. channel SDO pin
 - PB5  (RP37) : C IIC 2nd. channel SDA pin
 - PB6  (RP38) : C IIC 2nd. channel SCL pin
 - PC5  (RP53) : C UART 2nd. channel CTS pin
 - PC10 (RP58) : C UART 2nd. channel RTS pin
 - PC11 (RP59) : C UART 2nd. channel RX pin
 - PD4  (RP68) : C UART 2nd. channel TX pin
 - PD3  (RP67) : C UART 1st. channel CTS pin
 - PD2  (RP66) : C UART 1st. channel RTS pin
 - PD1  (RP65) : C UART 1st. channel RX pin
 - PD0  (RP64) : C UART 1st. channel TX pin
 - PB8         : I ICSP data pin
 - PB9         : I ICSP clock pin

 - *O : Output pin
 - *I : Input pin
 - *C : Communication pin

# Jumper Sets

 - UART2      : Connects default UART2 pins to VNC2 (USB HOST) chip or external source
 - VNC2_PRG   : Sets VNC2 chip to default connection mode or to program mode
 - EEPROM_ADD : Changes EEPROM I2C address. Default is 000.
 - AN_SRC     : Connects (Analog) A_VDD and A_VSS to VDD and VSS or to an external source.

# Occupied I2C Addresses

 - 0xD0 : DS1338Z-33+ 8 bit read address
 - 0xD1 : DS1338Z-33+ 8 bit write address
 - 0xA0 : 24LC256 8 bit read address by default (remapable)
 - 0xA1 : 24LC256 8 bit write address by default (remapable)
 - 0x78 : 0.96" Oled Screen 8 bit read address
 - 0x79 : 0.96" Oled Screen 8 bit write address

# Known Issues 

 - VNC_PRG_TM port won't program the VNCII chip.
 - To program VNCII chip, a jumper set from from EXT to VNC2 on UART2 jumper set will be needed.
 - UART1 is dedicated to uart-to-usb bridge chip and cannot be used by any other means
 - UART2 is shared by 3 sources and can be used by only 1 of them, wireless module, USB host and external input. USB host and external inputs are connected to same lines and can be selected via UART2 jumper set. Wireless module is connected to different lines end either UART2 can be assigned to these pins or a software uart channel can be set.
