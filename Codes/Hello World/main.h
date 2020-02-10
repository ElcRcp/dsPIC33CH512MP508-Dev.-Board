#include <33CH512MP508.h>
#use delay(internal=100000000)

#FUSES NOWDT                  //No Watch Dog Timer
#FUSES OSCIO                  //OSC2 is general purpose output
#FUSES CKSFSM                 //Clock Switching is enabled, fail Safe clock monitor is enabled
#FUSES S1_CKSFSM              //Slave core Clock Switching is enabled, fail Safe clock monitor is enabled

#define cp2102_rst PIN_E0
#define inp_but_1 PIN_E1
#define inp_but_2 PIN_E2
#define inp_but_3 PIN_E3
#define inp_but_4 PIN_E4
#define inp_but_5 PIN_E5
#define esp_rx PIN_C14
#define esp_tx PIN_C15
#define esp_en PIN_E7
#define relay_ctrl PIN_E6
#define spi2_sdi PIN_C9
#define spi2_sdo PIN_D8
#define spi2_sck PIN_C8
#define SRAM_CS PIN_D9
#define i2c2_sda PIN_B5
#define i2c2_scl PIN_B6
#define uart2_cts PIN_C5
#define uart2_rts PIN_C10
#define uart2_rx PIN_C11
#define uart2_tx PIN_D4
#define uart1_cts PIN_D3
#define uart1_rts PIN_D2
#define uart1_rx PIN_D1
#define uart1_tx PIN_D0

#pin_select U1TX=uart1_tx
#pin_select U1RX=uart1_rx
#pin_select U1CTS=uart1_cts
#pin_select U1RTS=uart1_rts
#use rs232(UART1, baud=115200, errors, stream=UART1)

#pin_select U2TX=uart2_tx
#pin_select U2RX=uart2_rx
#pin_select U2CTS=uart2_cts
#pin_select U2RTS=uart2_rts
#use rs232(UART2, baud=115200, errors, stream=UART2)

//#use spi(MASTER, SPI1, MODE=0, BITS=8, stream=SPI_PORT1)
#pin_select SCK2OUT=spi2_sck
#pin_select SDI2=spi2_sdi
#use spi(MASTER, SPI2, MODE=0, BITS=8, stream=SPI_PORT2)

#use i2c(MASTER, I2C2, FAST)

#use pwm(CCP1,OUTPUT=PIN_D7,FREQUENCY=1000,DUTY=50)

void mcu_setup(void)
{
  use_fast_io(all);
  set_tris_a(0b11111111);
  set_tris_b(0b1111111110111111);
  set_tris_c(0b1011101011111111);
  set_tris_d(0b1111110011101010);
  set_tris_e(0b1111111100111110);
}
