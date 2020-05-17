#define UART_BUFFER_LENGHT 200
char Uart1RcvBuff[UART_BUFFER_LENGHT];
int1 uart_1_rcv_flg=0;

#INT_RDA
void RDA_isr() 
{
   if(esp_bridge_flag==1)
   {
      fprintf(UART_CH2,"%c",fgetc(UART_CH1));
   }
   else
   {
      static int8 ctr1=0;
      int8 chr1=fgetc(UART_CH1);
      if (chr1==13) 
      { 
         //If we have an end of line 
         Uart1RcvBuff[ctr1++]=chr1; //null terminate string 
         Uart1RcvBuff[ctr1]='\0'; //null terminate string 
         ctr1=0; //clear the counter 
         uart_1_rcv_flg=1;
      } 
      else 
      { 
         Uart1RcvBuff[ctr1++]=chr1; //add character to string 
         if (ctr1>=UART_BUFFER_LENGHT-1) {
            ctr1--; //ensure buffer can not overflow 
         }
      }
   }
}

char Uart2RcvBuff[UART_BUFFER_LENGHT];
int1 uart_2_rcv_flg=0;

#INT_RDA2
void RDA2_isr() 
{
   if(esp_bridge_flag==1)
   {
      fprintf(UART_CH1,"%c",fgetc(UART_CH2));
   }
   else
   {
   static int8 ctr2=0;
   int8 chr2=fgetc(UART_CH2);
   if (chr2==13) 
   { 
      //If we have an end of line 
      Uart2RcvBuff[ctr2++]=chr2; //null terminate string 
      Uart2RcvBuff[ctr2]='\0'; //null terminate string 
      ctr2=0; //clear the counter 
      uart_2_rcv_flg=1;
   } 
   else 
   { 
      Uart2RcvBuff[ctr2++]=chr2; //add character to string 
      if (ctr2>=UART_BUFFER_LENGHT-1) {
         ctr2--; //ensure buffer can not overflow 
      }
   }
   }
}
