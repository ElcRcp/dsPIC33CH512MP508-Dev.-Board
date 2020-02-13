#include "main.h"
#include "sram23k640.h"
#include "eeprom24lc256.h"
#include "ssd1306.h"
#include "ds1338rtc.h"

char text1[25];        //temporary text buffer

#INT_CNIE
void joystick_input()
{
	switch(CNFE){
		case 2:strcpy(text1,"M");
			break;
		case 4:strcpy(text1,"R");
			break;
		case 8:strcpy(text1,"L");
			break;
		case 16:strcpy(text1,"D");
			break;
		case 32:strcpy(text1,"U");
			break;
		default:delay_cycles(1);
			break;
	}
}


void main()
{
   mcu_setup();
   ssd_init();          //Start oled screen (called from include file)
   delay_ms(100);       //give oled some time to wake
   OLED_CLS();          //clear the physical screen
   init_ext_sram();
   init_ext_eeprom();
   
   strcpy(text1,"01234567890123456789");
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

   fprintf(UART1,"Ready...\r\n");
   delay_ms(1000);
   strcpy(text1,">");
   unsigned int8 line_num=0;
   while(TRUE)
   {
      fprintf(UART1,"Heartbeat...\r\n");
      OLED_gotoxy(0,line_num);
   	OLED_text(text1,strlen(text1));
   	if (line_num==0)
   	{
   		OLED_gotoxy(0,7);
   		OLED_putc(' ');
   	}
   	else
   	{
   		OLED_gotoxy(0,line_num-1);
   		OLED_putc(' ');
   	}
   	line_num++;
   	if(line_num>7)
   	{
   		line_num=0;
   	}
      delay_ms(1000);
      //TODO: User Code
   }

}
