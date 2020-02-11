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
   char text1[15];        //temporary text buffer 
   strcpy(text1,"Test Screen");
   OLED_gotoxy(0,0);
   OLED_text(text1,strlen(text1));
   while(TRUE)
   {
      //TODO: User Code
   }

}
