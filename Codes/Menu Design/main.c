#include "main.h"
#include "sram23k640.h"
#include "eeprom24lc256.h"
#include "ssd1306.h"
#include "ds1338rtc.h"

void show_time_oled(int8 column, int8 row);
void show_date_oled(int8 column, int8 row);
void show_screen_demo(void);

#include "rtos_tasks.h"


void main()
{
   delay_ms(500);
   mcu_setup();
   size=NORMAL;         //specify font size
   OLED_commands(init_sequence,sizeof(init_sequence));          //Start oled screen (called from include file)
   delay_ms(100);       //give oled some time to wake
   OLED_CLS();          //clear the physical screen
   
   init_ext_sram();

   fprintf(UART_CH2,"Ready...\r\n");
   
   delay_ms(500);  
   print_dev_info();
   
   rtos_run();
   while(TRUE)
   {
      delay_us(1);
   }

}
void show_time_oled(int8 column, int8 row)
{
   unsigned int8 *Zaman;
   Zaman=ds1338_read_time();
   OLED_gotoxy(column,row);
   printf(OLED_putc,"%02u:%02u:%02u",*(Zaman+2),*(Zaman+1),*Zaman);
}

void show_date_oled(int8 column, int8 row)
{
   unsigned int8 *Tarih;
   Tarih=ds1338_read_date();
   OLED_gotoxy(column,row);
   printf(OLED_putc,"%02u/%02u/%02u%02u",*Tarih,*(Tarih+2),*(Tarih+4),*(Tarih+6));
}
