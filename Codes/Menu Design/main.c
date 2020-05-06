#include "main.h"
//!#include "sram23k640.h"
#include "eeprom24lc256.h"
#include "ssd1306.h"
#include "ds1338rtc.h"

char text1='/';        //temporary text buffer
char text2=':';

void show_time_oled(int8 column, int8 row);
void show_date_oled(int8 column, int8 row);
void show_screen_demo(void);

#include "rtos_tasks.h"

void main()
{
   mcu_setup();

   ssd_init();          //Start oled screen (called from include file)
   delay_ms(100);       //give oled some time to wake
   OLED_CLS();          //clear the physical screen
   size=NORMAL;         //specify font size
//!   init_ext_sram();
   init_ext_eeprom();


   fprintf(UART_CH1,"Ready...\r\n");

   delay_ms(1000);
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
   printf(OLED_putc,"%02u",*(Zaman+2));
   printf(OLED_putc,":");
   //OLED_text(text2,strlen(text2));
   printf(OLED_putc,"%02u",*(Zaman+1));
   printf(OLED_putc,":");
   //OLED_text(text2,strlen(text2));
   printf(OLED_putc,"%02u",*Zaman);
}

void show_date_oled(int8 column, int8 row)
{
   unsigned int8 *Tarih;
   Tarih=ds1338_read_date();
   OLED_gotoxy(column,row);
   printf(OLED_putc,"%02u",*Tarih);
   printf(OLED_putc,"/");
   //OLED_text(text1,strlen(text1));
   printf(OLED_putc,"%02u",*(Tarih+2));
   printf(OLED_putc,"/");
   //OLED_text(text1,strlen(text1));
   printf(OLED_putc,"%02u",*(Tarih+4));
   printf(OLED_putc,"%02u",*(Tarih+6));
}
