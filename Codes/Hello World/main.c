#include "main.h"
//!#include "sram23k640.h"
#include "eeprom24lc256.h"
#include "ssd1306.h"
#include "ds1338rtc.h"

char text1[25];        //temporary text buffer
char text2[5]; 

//!#INT_CNIE
//!void joystick_input()
//!{
//!   switch(CNFE){
//!      case 2:strcpy(text1,"M");
//!         break;
//!      case 4:strcpy(text1,"R");
//!         break;
//!      case 8:strcpy(text1,"L");
//!         break;
//!      case 16:strcpy(text1,"D");
//!         break;
//!      case 32:strcpy(text1,"U");
//!         break;
//!      default:delay_cycles(1);
//!         break;
//!   }
//!}

void show_time_oled(int8 column, int8 row);
void show_date_oled(int8 column, int8 row);
void show_screen_demo(void);

#include "rtos_tasks.h"

void main()
{
   mcu_setup();
   
/* ------ Used to set time for once ----- */   
//!   ds1338_set_date(16,2,2020);
//!   ds1338_set_time(4,27,30);
//!   ds1338_setup_out(0);
//!   ds1338_setup_sqwe(0);
//!   ds1338_setup_rate_select(0);
/*----------------------------------------*/

   ssd_init();          //Start oled screen (called from include file)
   delay_ms(100);       //give oled some time to wake
   OLED_CLS();          //clear the physical screen
   size=NORMAL;         //specify font size
//!   init_ext_sram();
   init_ext_eeprom();   
   
   strcpy(text2,":");
   strcpy(text1,"01234567890123456789");
   
   show_screen_demo();

   //fprintf(UART_CH1,"Ready...\r\n");

   delay_ms(1000);
   strcpy(text1,">");
   rtos_run();
   while(TRUE)
   {
//!      fprintf(UART_CH1,"Heartbeat...\r\n");
//!      delay_ms(1000);
      
   }

}
void show_time_oled(int8 column, int8 row)
{  
   unsigned int8 *Zaman;
   Zaman=ds1338_read_time();
   OLED_gotoxy(column,row);
   printf(OLED_putc,"%02u",*(Zaman+2));
   OLED_text(text2,strlen(text2));
   printf(OLED_putc,"%02u",*(Zaman+1));
   OLED_text(text2,strlen(text2));
   printf(OLED_putc,"%02u",*Zaman);
}

void show_date_oled(int8 column, int8 row)
{
   unsigned int8 *Tarih; 
   Tarih=ds1338_read_date();
   OLED_gotoxy(column,row);
   printf(OLED_putc,"%02u",*Tarih);
   OLED_text(text2,strlen(text2));
   printf(OLED_putc,"%02u",*(Tarih+2));
   OLED_text(text2,strlen(text2));
   printf(OLED_putc,"%02u",*(Tarih+4));
   printf(OLED_putc,"%02u",*(Tarih+6));
}

void show_screen_demo(void){
   OLED_gotoxy(1,0);
   OLED_text(text1,strlen(text1));
   strcpy(text1,"1");
   OLED_gotoxy(1,1);
   OLED_text(text1,strlen(text1));
   strcpy(text1,"2");
   OLED_gotoxy(1,2);
   OLED_text(text1,strlen(text1));
   strcpy(text1,"3");
   OLED_gotoxy(1,3);
   OLED_text(text1,strlen(text1));
   strcpy(text1,"4");
   OLED_gotoxy(1,4);
   OLED_text(text1,strlen(text1));
   strcpy(text1,"5");
   OLED_gotoxy(1,5);
   OLED_text(text1,strlen(text1));
   strcpy(text1,"6");
   OLED_gotoxy(1,6);
   OLED_text(text1,strlen(text1));
   strcpy(text1,"7");
   OLED_gotoxy(1,7);
   OLED_text(text1,strlen(text1));  
}
