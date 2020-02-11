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

static unsigned int8 INTtoBCD(unsigned int8 ival)
{
	return (((ival / 10) << 4) | (ival % 10));
}

unsigned int8 BCDtoINT(int8 BCD_INPUT)
{
   unsigned int8 temp_in=BCD_INPUT&0xF0;
   temp_in=temp_in>>4;
   temp_in=(temp_in*10)+(BCD_INPUT&0x0F);
   return temp_in;
}

void ds1338_set_time(unsigned int8 Hours, unsigned int8 Minutes, unsigned int8 Seconds)
{
	i2c_start();
	i2c_write(DS1338_WRITE_ADD);
	i2c_write(DS1338_REG_SECONDS);
	i2c_write(INTtoBCD(Seconds));
	i2c_write(INTtoBCD(Minutes));
	i2c_write(INTtoBCD(Hours));
	i2c_stop();
}

void ds1338_set_date(unsigned int8 Day, unsigned int8 Month, unsigned int16 Year)
{
	i2c_start();
	i2c_write(DS1338_WRITE_ADD);
	i2c_write(DS1338_REG_DATE);
	i2c_write(INTtoBCD(Day));
	i2c_write(INTtoBCD(Month));
	i2c_write(INTtoBCD(Year%100));
	i2c_stop();
	delay_us(100);
	i2c_start();
	i2c_write(DS1338_WRITE_ADD);
	i2c_write(DS1338_REG_HIGH_YEAR);
	i2c_write(INTtoBCD(Year/100));
	i2c_stop();

}

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

void ds1338_setup_osf(unsigned int8 inp)
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
	temp_read_value=temp_read_value | (inp<<5);
	delay_us(10);
	i2c_start();
	i2c_write(DS1338_WRITE_ADD);
	i2c_write(DS1338_REG_CONTROL);
	i2c_write(temp_read_value);
	i2c_stop();
}

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

unsigned int8 *  ds1338_read_time(void)
{
	static int8 ds1338_time_array[3]={0,0,0};
	i2c_start();
	i2c_write(DS1338_WRITE_ADD);
	i2c_write(DS1338_REG_SECONDS);
	i2c_stop();
	delay_us(10);
	i2c_start();
	i2c_write(DS1338_READ_ADD);
	ds1338_time_array[0]=i2c_read(1);
	ds1338_time_array[1]=i2c_read(1);
	ds1338_time_array[2]=i2c_read(0);
	i2c_stop();
	return(ds1338_time_array);
}
unsigned int16 *  ds1338_read_date(void)
{
	static int16 ds1338_date_array[3]={0,0,0};
	i2c_start();
	i2c_write(DS1338_WRITE_ADD);
	i2c_write(DS1338_REG_DATE);
	i2c_stop();
	delay_us(10);
	i2c_start();
	i2c_write(DS1338_READ_ADD);
	ds1338_time_array[0]=i2c_read(1);
	ds1338_time_array[1]=i2c_read(1);
	ds1338_time_array[2]=i2c_read(0);
	i2c_stop();
	delay_us(10);
	i2c_start();
	i2c_write(DS1338_WRITE_ADD);
	i2c_write(DS1338_REG_HIGH_YEAR);
	i2c_stop();
	delay_us(10);
	i2c_start();
	i2c_write(DS1338_READ_ADD);
	unsigned int16 temp_read=i2c_read(0);
	i2c_stop();
	ds1338_time_array[2]=(temp_read*100)+ds1338_time_array[2];
	return(ds1338_date_array);
}