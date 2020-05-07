#define SPI_XFER_SRAM(c)   spi_xfer(SPI_PORT2, c)
#define SPI_READ_SRAM()    SPI_XFER_SRAM(0xFF)
                                      
//Check to see if SRAM is in Byte mode and Sets to Byte mode if not
void init_ext_sram(void)
{
   int8 status;
   
   output_high(SRAM_CS);
   delay_ms(100);
   
   output_low(SRAM_CS);
   SPI_XFER_SRAM(0x05);                 //sends read status command
   status=SPI_READ_SRAM();              //reads status of SRAM
   output_high(SRAM_CS);
   if(status!=0)                    //checks to see if SRAM is in Byte mode
   {
      output_low(SRAM_CS);
      SPI_XFER_SRAM(0x01);              //sends write status command
      SPI_XFER_SRAM(0x00);              //sends value to set SRAM in Byte mode
      output_low(SRAM_CS);
   }
}

// Writes a byte of data to specified address
void write_ext_sram(int16 address,int8 data)
{
   output_low(SRAM_CS);             
   SPI_XFER_SRAM(0x02);                 //sends write command
   SPI_XFER_SRAM(make8(address,1));     //sends MS Byte of address
   SPI_XFER_SRAM(make8(address,0));     //sends LS Byte of address
   SPI_XFER_SRAM(data);                 //writes the byte of data
   output_high(SRAM_CS);
}

// Reads a byte of data from specified address
int8 read_ext_sram(int16 address)
{
   int8 data;
   
   output_low(SRAM_CS);
   SPI_XFER_SRAM(0x03);                 //sends read command
   SPI_XFER_SRAM(make8(address,1));     //sends MS Byte of address
   SPI_XFER_SRAM(make8(address,0));     //sends LS Byte of address
   data=SPI_READ_SRAM();                //reads the byte of data
   output_high(SRAM_CS);
   
   return(data);
}
