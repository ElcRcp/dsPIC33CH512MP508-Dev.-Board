#USE rtos(timer=1,minor_cycle=100ms)

#TASK(rate=200ms,max=10ms,queue=2)
void Mid_Button_Check(){
   if(!input(inp_but_1)){
      strcpy(text1,"M");
   }
}

#TASK(rate=200ms,max=10ms,queue=2)
void Right_Button_Check(){
   if(!input(inp_but_2)){
      strcpy(text1,"R");
   }
}

#TASK(rate=200ms,max=10ms,queue=2)
void Left_Button_Check(){
   if(!input(inp_but_3)){
      strcpy(text1,"L");
   }
}

#TASK(rate=200ms,max=10ms,queue=2)
void Down_Button_Check(){
   if(!input(inp_but_4)){
      strcpy(text1,"D");
   }
}

#TASK(rate=200ms,max=10ms,queue=2)
void Up_Button_Check(){
   if(!input(inp_but_5)){
      strcpy(text1,"U");
   }
}

#TASK(rate=500ms,max=50ms,queue=5)
void Update_date_and_time(){
   show_date_oled(6,3); 
   show_time_oled(6,5);
}

#TASK(rate=1s,max=50ms,queue=5)
void cursor_animation(){
   static unsigned int8 line_num=0;
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
}

#TASK(rate=1s,max=50ms,queue=1)
void Send_Heartbeat(){
   fprintf(UART_CH1,"Heartbeat...\r\n");
   fprintf(UART_CH2,"Heartbeat...\r\n");
}
