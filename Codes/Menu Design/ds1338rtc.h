// Registers
#define DS1338_REG_SECONDS 0x00
#define DS1338_REG_MINUTES 0x01
#define DS1338_REG_HOURS 0x02
#define DS1338_REG_DAY 0x03
#define DS1338_REG_DATE 0x04
#define DS1338_REG_MONTH 0x05
#define DS1338_REG_YEAR 0x06
#define DS1338_REG_CONTROL 0x07
#define DS1338_REG_HIGH_YEAR 0x08
#define DS1338_REG_RAM_BEGIN 0x09
#define DS1338_REG_RAM_END 0x3F
#define DS1338_WRITE_ADD 0xD0
#define DS1338_READ_ADD 0xD1


// This function converts integer input to BCD value to save in rtc
static unsigned int8 INTtoBCD(unsigned int8 ival)
{
   return (((ival / 10) << 4) | (ival % 10));
}

// This function converts BCD value to integer to work with
unsigned int8 BCDtoINT(int8 BCD_INPUT)
{
   unsigned int8 temp_in=BCD_INPUT&0xF0;
   temp_in=temp_in>>4;
   temp_in=(temp_in*10)+(BCD_INPUT&0x0F);
   return temp_in;
}

// Set time, inputs are hours minutes and seconds
int1  ds1338_set_time(unsigned int8 Hours, unsigned int8 Minutes, unsigned int8 Seconds)
{
   if((Hours>23)||(Minutes>59)||(Seconds>59))
   {
      return(0);
   }
   i2c_start();
   i2c_write(DS1338_WRITE_ADD);
   i2c_write(DS1338_REG_SECONDS);
   i2c_write(INTtoBCD(Seconds));
   i2c_write(INTtoBCD(Minutes));
   i2c_write(INTtoBCD(Hours));
   i2c_stop();
   return(1);
}

// Set date, with days, month and year
int1 ds1338_set_date(unsigned int8 Day, unsigned int8 Month, unsigned int16 Year)
{
   if((Day>31)||(Month>12)||(Year>9999))
   {
      return(0);
   }
   i2c_start();
   i2c_write(DS1338_WRITE_ADD);
   i2c_write(DS1338_REG_DATE);
   i2c_write(INTtoBCD(Day));
   i2c_write(INTtoBCD(Month));
   i2c_write(INTtoBCD(Year%100));
   i2c_stop();
   delay_us(10);
   i2c_start();
   i2c_write(DS1338_WRITE_ADD);
   i2c_write(DS1338_REG_HIGH_YEAR);
   i2c_write(INTtoBCD(Year/100));
   i2c_stop();
   return(1);
}

// setup sqw pin output level
void ds1338_setup_out(unsigned int8 inp)
{
   if (inp>1)
   {
      inp=1;
   }
   i2c_start();
   i2c_write(DS1338_WRITE_ADD);
   i2c_write(DS1338_REG_CONTROL);
   i2c_stop();
   delay_us(10);
   i2c_start();
   i2c_write(DS1338_READ_ADD);
   unsigned int8 temp_read_value=i2c_read(0);
   i2c_stop();
   temp_read_value=temp_read_value | (inp<<7);
   delay_us(10);
   i2c_start();
   i2c_write(DS1338_WRITE_ADD);
   i2c_write(DS1338_REG_CONTROL);
   i2c_write(temp_read_value);
   i2c_stop();
}

// Check Oscillator failure bit
int1 ds1338_check_osf(void)
{
   i2c_start();
   i2c_write(DS1338_WRITE_ADD);
   i2c_write(DS1338_REG_CONTROL);
   i2c_stop();
   delay_us(10);
   i2c_start();
   i2c_write(DS1338_READ_ADD);
   unsigned int8 temp_read_value=i2c_read(0);
   i2c_stop();
   return(bit_test(temp_read_value,5));
}

// Setup square wave output function on or off
void ds1338_setup_sqwe(unsigned int8 inp)
{
   if (inp>1)
   {
      inp=1;
   }
   i2c_start();
   i2c_write(DS1338_WRITE_ADD);
   i2c_write(DS1338_REG_CONTROL);
   i2c_stop();
   delay_us(10);
   i2c_start();
   i2c_write(DS1338_READ_ADD);
   unsigned int8 temp_read_value=i2c_read(0);
   i2c_stop();
   temp_read_value=temp_read_value | (inp<<4);
   delay_us(10);
   i2c_start();
   i2c_write(DS1338_WRITE_ADD);
   i2c_write(DS1338_REG_CONTROL);
   i2c_write(temp_read_value);
   i2c_stop();
}

// Setup square wave output frequency
void ds1338_setup_rate_select(unsigned int8 inp)
{
   inp=inp & 0b00000011;
   i2c_start();
   i2c_write(DS1338_WRITE_ADD);
   i2c_write(DS1338_REG_CONTROL);
   i2c_stop();
   delay_us(10);
   i2c_start();
   i2c_write(DS1338_READ_ADD);
   unsigned int8 temp_read_value=i2c_read(0);
   i2c_stop();
   temp_read_value=temp_read_value | inp;
   delay_us(10);
   i2c_start();
   i2c_write(DS1338_WRITE_ADD);
   i2c_write(DS1338_REG_CONTROL);
   i2c_write(temp_read_value);
   i2c_stop();   
}

// Read time values from rtc and output to an 8 bit array
// [0] is seconds, [1] is minutess, [2] is hours, 
unsigned int8 *  ds1338_read_time(void)
{
   static unsigned int8 ds1338_time_array[3]={0,0,0};
   i2c_start();
   i2c_write(DS1338_WRITE_ADD);
   i2c_write(DS1338_REG_SECONDS);
   i2c_stop();
   delay_us(10);
   i2c_start();
   i2c_write(DS1338_READ_ADD);
   ds1338_time_array[0]=BCDtoINT(i2c_read(1));
   ds1338_time_array[1]=BCDtoINT(i2c_read(1));
   ds1338_time_array[2]=BCDtoINT(i2c_read(0));
   i2c_stop();
   return(ds1338_time_array);
}

// Read date values from rtc and output to an 16 bit array
// [0] is days, [1] is month, [2] is high byte of the year and [3] is for low byte of the year
unsigned int8 *  ds1338_read_date(void)
{
   static unsigned int16 ds1338_date_array[4]={0,0,0,0};
   i2c_start();
   i2c_write(DS1338_WRITE_ADD);
   i2c_write(DS1338_REG_DATE);
   i2c_stop();
   delay_us(10);
   i2c_start();
   i2c_write(DS1338_READ_ADD);
   ds1338_date_array[0]=BCDtoINT(i2c_read(1));
   ds1338_date_array[1]=BCDtoINT(i2c_read(1));
   ds1338_date_array[3]=BCDtoINT(i2c_read(0));
   i2c_stop();
   delay_us(10);
   i2c_start();
   i2c_write(DS1338_WRITE_ADD);
   i2c_write(DS1338_REG_HIGH_YEAR);
   i2c_stop();
   delay_us(10);
   i2c_start();
   i2c_write(DS1338_READ_ADD);
   ds1338_date_array[2]=BCDtoINT(i2c_read(0));
   i2c_stop();
   return(ds1338_date_array);
}

// 55 Byte data can be kept in ds1338's battery backed ram
// Ram address range is 0x09 to 0x3F
int1 ds1338_write_ram(unsigned int8 Write_Address, int8 * Data_Array, unsigned int8 Data_Length)
{
   if((Write_Address>0x3F)||(Write_Address<0x09))
   {
      return(0);
   }
   if((Data_Length>55) || (Data_Length>(0x3F-Write_Address)))
   {
      return(0);
   }

   i2c_start();
   i2c_write(DS1338_WRITE_ADD);
   i2c_write(Write_Address);
   for(unsigned int8 i=0;i<Data_Length;i++)
   {
      i2c_write(Data_Array[i]);
   }
   i2c_stop();
   return(1);
}

// Ram address range is 0x09 to 0x3F total of 55 byte
int1 ds1338_read_ram(unsigned int8 Read_Address, int8 * Data_Array, unsigned int8 Data_Length)
{
   if((Read_Address>0x3F)||(Read_Address<0x09))
   {
      return(0);
   }
   if((Data_Length>55) || (Data_Length>(0x3F-Read_Address)))
   {
      return(0);
   }
   int1 ack_bit=0;
   i2c_start();
   i2c_write(DS1338_WRITE_ADD);
   i2c_write(Read_Address);
   i2c_stop();
   delay_us(10);
   i2c_start();
   i2c_write(DS1338_READ_ADD);
   for(unsigned int8 i=0;i<Data_Length;i++)
   {
      if((Data_Length-i)!=1)
      {
         ack_bit=1;
      }
      else
      {
         ack_bit=0;
      }
      Data_Array[i]=i2c_read(ack_bit);
   }
   i2c_stop();
   return(1);
}
