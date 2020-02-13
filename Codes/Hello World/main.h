#include <33CH512MP508.h>


#fuses WDT_SW                 //No Watch Dog Timer, enabled in Software
#FUSES OSCIO                  //OSC2 is general purpose output
#FUSES CKSFSM                 //Clock Switching is enabled, fail Safe clock monitor is enabled
#fuses S1_NOIESO              //Slave core Internal External Switch Over mode disabled
#fuses S1_SPI1PINS_PPS        //Slave core SPI1 pins can be assigned with #pin_select
#fuses S1_WDT_SW              //Slave core No Watch Dog Timer, enabled in Software
#fuses SPI2PINS_PPS           //SPI2 pins can be assigned with #pin_select
#fuses NOALTI2C2              //I2C2 mapped to SDA2/SCL2 pins
#fuses NOJTAG                 //JTAG disabled
#fuses FRC_PLL                //Internal Fast RC oscillator with PLL

#use delay(internal=100000000)

#WORD CNCONE=GETENV("SFR:CNCONE")
#WORD CNEN1E=GETENV("SFR:CNEN1E")
#WORD CNFE=GETENV("SFR:CNFE")

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

#define ESP_IS_OFF  //define esp as on or off to select hardware uart2 connection

#pin_select U1TX=uart1_tx
#pin_select U1RX=uart1_rx
#pin_select U1CTS=uart1_cts
#pin_select U1RTS=uart1_rts
#use rs232(UART1, baud=115200, errors, stream=UART1)
//#use rs232(baud=115200,RCV=uart1_rx,XMIT=uart1_tx,CTS=uart1_cts,RTS=uart1_rts,SIMPLEX_MODE,parity=N,UART1,bits=8,ERRORS,stream=USB_Bridge)

#ifdef ESP_IS_ON
   #ifndef ESP_IS_OFF
      #pin_select U2TX=esp_rx
      #pin_select U2RX=esp_tx
      #use rs232(UART2, baud=115200, errors, stream=esp_com)
   #endif
#endif
#ifdef ESP_IS_OFF
   #ifndef ESP_IS_ON
      #pin_select U2TX=uart2_tx
      #pin_select U2RX=uart2_rx
      #pin_select U2CTS=uart2_cts
      #pin_select U2RTS=uart2_rts
      #use rs232(UART2, baud=115200, errors, stream=UART2)
   #endif
#endif

//#use spi(MASTER, SPI1, MODE=0, BITS=8, stream=SPI_PORT1)
#pin_select SCK2OUT=spi2_sck
#pin_select SDI2=spi2_sdi
#use spi(MASTER, SPI2, MODE=0, BITS=8, stream=SPI_PORT2)

#use i2c(MASTER, I2C2, FAST)

#use fast_io(all)
void mcu_setup(void)
{
  set_tris_a(0b11111111);
  set_tris_b(0b1111111110111111);
  set_tris_c(0b1011101011111111);
  set_tris_d(0b1111110011101010);
  set_tris_e(0b1111111100111110);
  SETUP_ADC_PORTS(NO_ANALOGS);
  output_bit(cp2102_rst,1);  //uart-to-usb bridge is active by default
  output_bit(esp_en,0);  //ESP is off by default
  output_bit(relay_ctrl,0);  //relay is not active as default
  output_bit(SRAM_CS,1);
  CNEN1E|=0b0000000000111110;
  CNCONE|=0b1000100000000000;
  CNFE=0;
  enable_interrupts(INT_CNIE);
  enable_interrupts(GLOBAL);
}

/*--- OLED DEFINITIONS ---*/
//Display dimensions - the physical LCD
#define S_LCDWIDTH               128
#define S_LCDHEIGHT              64
#define TEXT_ONLY    //If this is defined, gives a smaller text driver only
#define SSDADDR 0x78 //address for the chip - usually 0x7C or 0x78.
#define SH1106  // Used for 1.3" oled screens
#include <string.h>
