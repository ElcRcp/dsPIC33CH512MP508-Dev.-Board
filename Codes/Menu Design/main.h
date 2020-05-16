#include <33CH512MP508.h>
#include <stdio.h>
#include <string.h>

#fuses WDT_SW                 //No Watch Dog Timer, enabled in Software
#FUSES OSCIO                  //OSC2 is general purpose output
#fuses NOALTI2C2              //I2C2 mapped to SDA2/SCL2 pins
#fuses FRC_PLL                //Internal Fast RC oscillator with PLL

#use delay(internal=100M)

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
#define sram_cs PIN_D9
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

// GLOBAL VARIABLES
unsigned int1 is_mid_but_pressed=0,is_right_but_pressed=0,is_left_but_pressed=0,is_down_but_pressed=0,is_up_but_pressed=0;
unsigned int1 invert_screen_color=0;
unsigned int8 screen_page=0;
unsigned int8 timeSetHour=12,timeSetMinute=30,timeSetSeconds=30,dateSetDay=15,dateSetMonth=6,dateSetYearLow=0,dateSetYearHigh=0;
unsigned int16 dateSetYear=0;


// UART1 Setup
#pin_select U1TX=uart1_tx
#pin_select U1RX=uart1_rx
//!#pin_select U1CTS=uart1_cts
//!#pin_select U1RTS=uart1_rts
#use rs232(UART1, baud=115200, errors, stream=UART_CH1)
//!#use rs232(baud=115200,RCV=uart1_rx,XMIT=uart1_tx,CTS=uart1_cts,RTS=uart1_rts,FLOW_CONTROL_MODE,parity=N,bits=8,ERRORS,stream=UART_CH1)


// UART2 Setup
#pin_select U2TX=uart2_tx
#pin_select U2RX=uart2_rx
#pin_select U2CTS=uart2_cts
#pin_select U2RTS=uart2_rts
//!      #use rs232(baud=115200,RCV=uart2_rx,XMIT=uart2_tx,CTS=uart2_cts,RTS=uart2_rts,FLOW_CONTROL_MODE,parity=N,bits=8,ERRORS,stream=UART_CH2)
#use rs232(baud=115200,RCV=uart2_rx,XMIT=uart2_tx,parity=N,bits=8,ERRORS,stream=UART_CH2)


// SPI SETUP
#pin_select SCK2OUT=spi2_sck
#pin_select SDI2=spi2_sdi
#use spi(MASTER, SPI2, MODE=0, BITS=8, BAUD=1000000, FORCE_HW, stream=SPI_PORT2)


// I2C SETUP
#use i2c(MASTER, I2C2, FAST=400000, FORCE_HW, NOINIT)

#use fast_io(all)
void mcu_setup(void)
{
  setup_wdt(WDT_OFF);
  disable_dmt();
  i2c_init(TRUE);
  setup_timer1(TMR_DISABLED);
  setup_msi(MSI_SLAVE_DISABLE);
  SETUP_ADC_PORTS(NO_ANALOGS);
  setup_comparator_dac(COMP_COMMON_DAC_DISABLE);
  SETUP_CURRENT_SOURCE(CURRENT_SOURCE_DISABLED);
  setup_qei(QEI_DISABLED);

  set_tris_a(0b11111111);
  set_tris_b(0b1111111110111111);
  set_tris_c(0b1011101011111111);
  set_tris_d(0b1111110011101010);
  set_tris_e(0b1111111100111110);

  output_bit(cp2102_rst,0);
  output_bit(esp_en,0);  //ESP is off by default
  output_bit(relay_ctrl,0);  //relay is not active as default
  output_bit(sram_cs,1);

}

/*--- OLED DEFINITIONS ---*/
//Display dimensions - the physical LCD
#define S_LCDWIDTH               128
#define S_LCDHEIGHT              64
#define TEXT_ONLY    //If this is defined, gives a smaller text driver only
#define SSDADDR 0x78 //address for the chip - usually 0x7C or 0x78.
#define SH1106  // Used for 1.3" oled screens
#include <string.h>

void clear_buttons()
{
   is_mid_but_pressed=0;
   is_right_but_pressed=0;
   is_left_but_pressed=0;
   is_down_but_pressed=0;
   is_up_but_pressed=0;
}
