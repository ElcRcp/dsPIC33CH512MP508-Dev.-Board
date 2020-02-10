#include <33CH512MP508.h>
#device ICSP=1
#use delay(internal=100000000)

#FUSES NOWDT                 	//No Watch Dog Timer
#FUSES OSCIO                 	//OSC2 is general purpose output
#FUSES CKSFSM                	//Clock Switching is enabled, fail Safe clock monitor is enabled
#FUSES S1_CKSFSM             	//Slave core Clock Switching is enabled, fail Safe clock monitor is enabled

#pin_select U1TX=PIN_D0
#pin_select U1RX=PIN_D1
#use rs232(UART1, baud=115200, errors, stream=UART1)

#pin_select U2TX=PIN_D2
#pin_select U2RX=PIN_D3
#use rs232(UART2, baud=115200, errors, stream=UART2)

#use spi(MASTER, SPI1, MODE=0, BITS=8, stream=SPI_PORT1)
#use spi(MASTER, SPI2, MODE=0, BITS=8, stream=SPI_PORT2)

#use i2c(MASTER, I2C1, FAST, stream=I2C_PORT1)
#use i2c(MASTER, I2C2, FAST, stream=I2C_PORT2)

#use pwm(CCP1,OUTPUT=PIN_D7,FREQUENCY=1000,DUTY=50)

