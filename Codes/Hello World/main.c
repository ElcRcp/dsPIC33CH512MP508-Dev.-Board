#include "main.h"
#include "sram23k640.h"
#include "eeprom24lc256.h"
#include "ssd1306.h"
#include "ds1338rtc.h"


void main()
{
   mcu_setup();
   ssd_init();          //Start oled screen (called from include file)
   delay_ms(100);       //give oled some time to wake
   OLED_CLS();          //clear the physical screen
   init_ext_sram();
   init_ext_eeprom();
   char text1[25];        //temporary text buffer 
   strcpy(text1,"012345678901234567890");
   OLED_gotoxy(0,0);
   OLED_text(text1,strlen(text1));
   strcpy(text1,"1");
   OLED_gotoxy(0,1);
   OLED_text(text1,strlen(text1));
   strcpy(text1,"2");
   OLED_gotoxy(0,2);
   OLED_text(text1,strlen(text1));
   strcpy(text1,"3");
   OLED_gotoxy(0,3);
   OLED_text(text1,strlen(text1));
   strcpy(text1,"4");
   OLED_gotoxy(0,4);
   OLED_text(text1,strlen(text1));
   strcpy(text1,"5");
   OLED_gotoxy(0,5);
   OLED_text(text1,strlen(text1));
   strcpy(text1,"6");
   OLED_gotoxy(0,6);
   OLED_text(text1,strlen(text1));
   strcpy(text1,"7");
   OLED_gotoxy(0,7);
   OLED_text(text1,strlen(text1));

   fprintf(UART1,"Ready...\r\n");
   delay_ms(1000);
   while(TRUE)
   {
      fprintf(UART1,"Heartbeat...\r\n");
      delay_ms(1000);
      //TODO: User Code
   }

}
