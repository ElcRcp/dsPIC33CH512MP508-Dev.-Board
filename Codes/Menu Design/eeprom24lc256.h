///////////////////////////////////////////////////////////////////////////
////   Library for a 24LC256 serial EEPROM                             ////
////                                                                   ////
////   init_ext_eeprom();    Call before the other functions are used  ////
////                                                                   ////
////   write_ext_eeprom(a, d);  Write the byte d to the address a      ////
////                                                                   ////
////   d = read_ext_eeprom(a);   Read the byte d from the address a    ////
////                                                                   ////
////   The main program may define eeprom_sda                          ////
////   and eeprom_scl to override the defaults below.                  ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

#define EEPROM_ADDRESS long int
#define EEPROM_SIZE   32768


//!void write_exteeprom_string(unsigned int8 hi_addr, unsigned int8 low_addr, char data[], unsigned int8 data_len){
//!   
//!   i2c_start();
//!   i2c_write(0xa0);
//!   i2c_write(hi_addr);
//!   i2c_write(low_addr);
//!   for(unsigned int8 x=0;x<data_len;x++){
//!      i2c_write(data[x]);
//!      delay_ms(1);
//!   }
//!   i2c_stop();
//!}


void print_exteeprom_string(unsigned int8 hi_addr, unsigned int8 low_addr, unsigned int8 data_len){
   
   i2c_start();
   i2c_write(0xa0);
   i2c_write(hi_addr);
   i2c_write(low_addr);
   i2c_start();
   i2c_write(0xa1);
   for(unsigned int8 x=0;x<data_len;x++)
   {
      fprintf(UART_CH2,"%c",i2c_read(1));
      delay_ms(1);
   }
   i2c_read(0);
   i2c_stop();

}

void print_dev_info()
{
   print_exteeprom_string(0x00,0x00,36);  //Device Name
   print_exteeprom_string(0x01,0x00,36);  //Device Details
   print_exteeprom_string(0x03,0x00,5);   //Device Version
}

void write_ext_eeprom(unsigned int8 address, BYTE data)
{
   int8 status;
   i2c_start();
   i2c_write(0xa0);
   i2c_write(address>>8);
   i2c_write(address);
   i2c_write(data);
   i2c_stop();
   i2c_start();
   status=i2c_write(0xa0);
   while(status==1) {
      i2c_start();
      status=i2c_write(0xa0);
   }
   i2c_stop();
}


BYTE read_ext_eeprom(unsigned int16 address) {
   BYTE data;
   i2c_start();
   i2c_write(0xa0);
   i2c_write(address>>8);
   i2c_write(address);
   i2c_start();
   i2c_write(0xa1);
   data=i2c_read(0);
   i2c_stop();
   return(data);
}


//!void ext_eep_write_dev_info()
//!{
//!   char dev_name[]="dsPIC33CH512MP508 Development Board\n";
//!   char dev_det[]="This is a Test Device Made By Recep\n";
//!   char dev_ver[]="v0.2\n";
//!   write_exteeprom_string(0x00,0x00,dev_name,36);
//!   write_exteeprom_string(0x01,0x00,dev_det,36);
//!   write_exteeprom_string(0x03,0x00,dev_ver,5);
//!}
