#USE rtos(timer=1,minor_cycle=100ms)

#TASK(rate=200ms,max=10ms,queue=2)
void Mid_Button_Check(){
   if(!input(inp_but_1)){
      is_mid_but_pressed=1;
   }
}

#TASK(rate=200ms,max=10ms,queue=2)
void Right_Button_Check(){
   if(!input(inp_but_2)){
      is_right_but_pressed=1;
   }
}

#TASK(rate=200ms,max=10ms,queue=2)
void Left_Button_Check(){
   if(!input(inp_but_3)){
     is_left_but_pressed=1;
   }
}

#TASK(rate=200ms,max=10ms,queue=2)
void Down_Button_Check(){
   if(!input(inp_but_4)){
      is_down_but_pressed=1;
   }
}

#TASK(rate=200ms,max=10ms,queue=2)
void Up_Button_Check(){
   if(!input(inp_but_5)){
      is_up_but_pressed=1;
   }
}

//!#TASK(rate=500ms,max=50ms,queue=5)
//!void Update_date_and_time(){
//!   show_date_oled(6,3); 
//!   show_time_oled(6,5);
//!}

#TASK(rate=500ms,max=50ms,queue=5)
void Screen_Handler(){
   static int8 line_select=0,col_select=0;
   if(screen_page==0){
      OLED_CLS();
      show_date_oled(6,3);
      show_time_oled(6,5);
      if(is_mid_but_pressed){
         screen_page=1;
         line_select=0;
         col_select=0;
      }
      
   }
   else if(screen_page==1){
      OLED_CLS();
      OLED_gotoxy(col_select,line_select);
      OLED_putc("->");
      OLED_gotoxy(2,0);
      OLED_putc("Set Date");
      OLED_gotoxy(2,1);
      OLED_putc("Set Time");
      OLED_gotoxy(2,2);
      OLED_putc("Invert Screen");
      OLED_gotoxy(2,3);
      OLED_putc("Wifi Settings");
      OLED_gotoxy(2,4);
      OLED_putc("Toggle Relay");
      OLED_gotoxy(2,5);
      OLED_putc("Cancel");
   }
}

#TASK(rate=1s,max=50ms,queue=5)
void Send_Heartbeat(){
   fprintf(UART_CH2,"Heartbeat...\r\n");
   fprintf(UART_CH1,"Heartbeat...\r\n");
}
