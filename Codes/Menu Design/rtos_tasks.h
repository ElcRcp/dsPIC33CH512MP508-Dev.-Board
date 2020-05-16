#USE rtos(timer=1,minor_cycle=50ms)

#TASK(rate=150ms,max=10ms,queue=2)
void Mid_Button_Check(){
   if(!input(inp_but_1)){
      is_mid_but_pressed=1;
   }
}
//!
#TASK(rate=150ms,max=10ms,queue=2)
void Right_Button_Check(){
   if(!input(inp_but_2)){
      is_right_but_pressed=1;
   }
}
//!
#TASK(rate=150ms,max=10ms,queue=2)
void Left_Button_Check(){
   if(!input(inp_but_3)){
     is_left_but_pressed=1;
   }
}
//!
#TASK(rate=150ms,max=10ms,queue=2)
void Down_Button_Check(){
   if(!input(inp_but_4)){
      is_down_but_pressed=1;
   }
}
//!
#TASK(rate=150ms,max=10ms,queue=2)
void Up_Button_Check(){
   if(!input(inp_but_5)){
      is_up_but_pressed=1;
   }
}

#TASK(rate=500ms,max=50ms,queue=5)
void Screen_Handler(){
   static int8 line_select=0,col_select=0;
   if(screen_page==0){
      show_date_oled(6,3);
      show_time_oled(6,5);
      if(is_mid_but_pressed){
         screen_page=2;
         line_select=0;
         col_select=0;
         is_mid_but_pressed=0;
         OLED_CLS();
      }
      
   }
   else if(screen_page==2){
      if(is_down_but_pressed){
         if(line_select==6){
            line_select=5;
         }
         line_select++;
         OLED_CLS();
      }else if(is_up_but_pressed){
         if(line_select==0){
            line_select=1;
         }
         line_select--;
         OLED_CLS();
      }
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
      OLED_gotoxy(2,6);
      OLED_putc("Cancel");
      if(is_mid_but_pressed){
         switch(line_select){
            case 0:screen_page=3;
                  unsigned int8 *dateArraySet;
                  dateArraySet=ds1338_read_date();
                  dateSetDay=*(dateArraySet+0);
                  dateSetMonth=*(dateArraySet+2);
                  dateSetYearHigh=*(dateArraySet+4);
                  dateSetYearLow=*(dateArraySet+6);
                  dateSetYear=(dateSetYearHigh*100)+dateSetYearLow;
                  line_select=2;
                  col_select=0;
                  break;
            case 1:screen_page=4;
                  unsigned int8 *timeArraySet;
                  timeArraySet=ds1338_read_time();
                  timeSetHour=*(timeArraySet+2);
                  timeSetMinute=*(timeArraySet+1);
                  timeSetSeconds=*(timeArraySet+0);
                  line_select=2;
                  col_select=0;
                  break;
            case 2:set=!set;
                  break;
            case 3:screen_page=1;
                  line_select=0;
                  col_select=0;
                  break;
            case 4:output_toggle(relay_ctrl);
                  break;
            case 6:screen_page=0;
                  line_select=0;
                  col_select=0;
                  break;
            default:break;
         }
         OLED_CLS();
      }
      clear_buttons();
   }
   else if(screen_page==3){
      static int8 setDateRowSelect=4;
      if(is_down_but_pressed){
         if(line_select==5){
            line_select=5;
         }
         else if((line_select==2)){
            if(setDateRowSelect==4){
               line_select++;
               line_select++;
            }
            else if(setDateRowSelect==6){
               if(dateSetDay==1){
                  dateSetDay=32;
               }
               dateSetDay--;
            }
            else if(setDateRowSelect==9){
               if(dateSetMonth==1){
                  dateSetMonth=13;
               }
               dateSetMonth--;
            }
            else if(setDateRowSelect==12){
               if(dateSetYear==2020){
                  dateSetYear=10000;
               }
               dateSetYear--;
            }
         }
         else{
            line_select++;
         }
         OLED_CLS();
      }
      else if(is_up_but_pressed){
         if(line_select==4){
            line_select--;
            line_select--;
         }
         else if((line_select==2)){
            if(setDateRowSelect==4){
               line_select=2;
            }
            else if(setDateRowSelect==6){
               dateSetDay++;
               if(dateSetDay==32){
                  dateSetDay=1;
               }
            }
            else if(setDateRowSelect==9){
               dateSetMonth++;
               if(dateSetMonth==13){
                  dateSetMonth=1;
               }
            }
            else if(setDateRowSelect==12){
               dateSetYear++;
               if(dateSetYear==10000){
                  dateSetYear=2020;
               }
            }
         }
         else{
            line_select--;
         }  
         OLED_CLS();
      }
      else if(is_right_but_pressed){
         if(line_select==2){
            if(setDateRowSelect==4){
               setDateRowSelect=6;
            }
            else if(setDateRowSelect==6){
               setDateRowSelect=9;
            }
            else if(setDateRowSelect==9){
               setDateRowSelect=12;
            }
         }
         OLED_CLS();
      }
      else if(is_left_but_pressed){
         if(line_select==2){
            if(setDateRowSelect==6){
               setDateRowSelect=4;
            }
            else if(setDateRowSelect==9){
               setDateRowSelect=6;
            }
            else if(setDateRowSelect==12){
               setDateRowSelect=9;
            }
         }
         OLED_CLS();
      }
      OLED_gotoxy(col_select,line_select);
      OLED_putc("->");
      OLED_gotoxy(6,2);
      printf(OLED_putc,"%02u/%02u/%04Lu",dateSetDay,dateSetMonth,dateSetYear);
      OLED_gotoxy(setDateRowSelect,3);
      OLED_putc("^^");
      OLED_gotoxy(6,4);
      OLED_putc("Save Date");
      OLED_gotoxy(6,5);
      OLED_putc("Cancel");
      if(is_mid_but_pressed){
         switch(line_select){
            case 2:setDateRowSelect=4;
                  break;
            case 4:ds1338_set_date(dateSetDay,dateSetMonth,dateSetYear); 
                  screen_page=0;
                  break;
            case 5:screen_page=2;
                  line_select=0;
                  col_select=0;
                  break;
            default:break;
         }
         OLED_CLS();
      }
      clear_buttons();
   }
   else if(screen_page==4){
   static int8 setTimeRowSelect=4;
      if(is_down_but_pressed){
         if(line_select==5){
            line_select=5;
         }
         else if((line_select==2)){
            if(setTimeRowSelect==4){
               line_select++;
               line_select++;
            }
            else if(setTimeRowSelect==6){
               if(timeSetHour==0){
                  timeSetHour=24;
               }
               timeSetHour--;
            }
            else if(setTimeRowSelect==9){
               if(timeSetMinute==0){
                  timeSetMinute=60;
               }
               timeSetMinute--;
            }
            else if(setTimeRowSelect==12){
               if(timeSetSeconds==0){
                  timeSetSeconds=60;
               }
               timeSetSeconds--;
            }
         }
         else{
            line_select++;
         }
         OLED_CLS();
      }
      else if(is_up_but_pressed){
         if(line_select==4){
            line_select--;
            line_select--;
         }
         else if((line_select==2)){
            if(setTimeRowSelect==4){
               line_select=2;
            }
            else if(setTimeRowSelect==6){
               timeSetHour++;
               if(timeSetHour==24){
                  timeSetHour=0;
               }
            }
            else if(setTimeRowSelect==9){
               timeSetMinute++;
               if(timeSetMinute==60){
                  timeSetMinute=0;
               }
            }
            else if(setTimeRowSelect==12){
               timeSetSeconds++;
               if(timeSetSeconds==60){
                  timeSetSeconds=0;
               }
            }
         }
         else{
            line_select--;
         } 
         OLED_CLS();
      }
      else if(is_right_but_pressed){
         if(line_select==2){
            if(setTimeRowSelect==4){
               setTimeRowSelect=6;
            }
            else if(setTimeRowSelect==6){
               setTimeRowSelect=9;
            }
            else if(setTimeRowSelect==9){
               setTimeRowSelect=12;
            }
         }
         OLED_CLS();
      }
      else if(is_left_but_pressed){
         if(line_select==2){
            if(setTimeRowSelect==6){
               setTimeRowSelect=4;
            }
            else if(setTimeRowSelect==9){
               setTimeRowSelect=6;
            }
            else if(setTimeRowSelect==12){
               setTimeRowSelect=9;
            }
         }
         OLED_CLS();
      }
      OLED_gotoxy(col_select,line_select);
      OLED_putc("->");
      OLED_gotoxy(6,2);
      printf(OLED_putc,"%02u:%02u:%02u",timeSetHour,timeSetMinute,timeSetSeconds);
      OLED_gotoxy(setTimeRowSelect,3);
      OLED_putc("^^");
      OLED_gotoxy(6,4);
      OLED_putc("Save Time");
      OLED_gotoxy(6,5);
      OLED_putc("Cancel");
      if(is_mid_but_pressed){
         switch(line_select){
            case 2:setTimeRowSelect=4;
                  break;
            case 4:ds1338_set_time(timeSetHour,timeSetMinute,timeSetSeconds); 
                  screen_page=0;
                  break;
            case 5:screen_page=2;
                  line_select=0;
                  col_select=0;
                  break;
            default:break;
         }
         OLED_CLS();
      }
      clear_buttons();
   }
   else if(screen_page==1){    
      if(is_down_but_pressed){
         if(line_select==6){
            line_select=5;
         }
         line_select++;
         OLED_CLS();
      }else if(is_up_but_pressed){
         if(line_select==0){
            line_select=1;
         }
         line_select--;
         OLED_CLS();
      }
      OLED_gotoxy(col_select,line_select);
      OLED_putc("->");

      OLED_gotoxy(2,6);
      OLED_putc("Cancel");
      if(is_mid_but_pressed){
         switch(line_select){
         case 6:screen_page=2;
               line_select=0;
               col_select=0;
               break;
         default:break;
         }
         OLED_CLS();
      }
      clear_buttons();
   }
}

#TASK(rate=1s,max=50ms,queue=5)
void Send_Heartbeat(){
   static unsigned int16 hb_ct=0;
   fprintf(UART_CH2,"%Lu Heartbeat...\r\n",hb_ct++);
   fprintf(UART_CH1,"Heartbeat...\r\n");
}
