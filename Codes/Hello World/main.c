#include "main.h"
#include "sram23k640.h"
#include "eeprom24lc256.h"


void main()
{
   mcu_setup();

   init_ext_sram();
   init_ext_eeprom();


   while(TRUE)
   {
      //TODO: User Code
   }

}
