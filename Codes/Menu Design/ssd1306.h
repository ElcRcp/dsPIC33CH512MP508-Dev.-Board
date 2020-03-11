/* 
Now the big limitation..... 
In serial modes (SPI/I2C), this chip provides no ability to read back it's RAM. 
So we have a problem. If we want to write a line across the screen, and leave 
another line that is already there 'undestroyed' where they cross, how can we 
'know' the other line is there?. Basically the host chip needs to have a copy 
of the display memory so it can hold a copy of any graphics and know what is 
going on. Problem is that this is just not possible, on a PIC with limited RAM. 
So this driver works by overwriting for all text writes.... 

But see further down for the 'exception' to this. 
*/ 
//It is most efficiently used by preparing the whole line of text first 
//and then sending this - it then uses a 'burst' transmission, to give very 
//fast updates. It does offer a 'putc' though, but this is slower. 
//It does not properly handle wrapping at the end of the line. 
//The putc function adds support for \n, \r, and \f. 
//Functions: 
//    OLED_CLS(); //clears the screen 
//    OLED_gotoxy(x, y); //goto column/row 0-20 for the column 
//                             //0-7 for the row 
//    OLED_text(*text,  number); 
//                             //This sends 'number' bytes from the array 
//                             //pointed to by 'text', to the display 
//    OLED_putc(c);            //sends 'c' to the display. Beware though 
//                             //if you go beyond the end of the line 
//                             //- you'll get partial characters.... 
//    OLED_textbar(width);     //Displays a bargraph. With width=50 
//                             //you get a 50:50 display of bar/void. 
//Two global variables affect how things are displayed. 
//    size=NORMAL; 
//    size=LARGE;              //switches between showing 21*8 & 10*4 
//    size=DOUBLE_HEIGHT //Gives 21*4 - great for the bargraph 
//    set=TRUE;                //default. Pixels are 'set' when written, so 
//                             //turn on. 
//    set=FALSE;               //all write functions now invert. 
//The CLS will now set the screen white. Text characters print in black 

//Then the second part of the driver is a 'window' driver. With this you can 
//define a small graphic 'window', and draw things into this. This can then be 
//rapidly copied to the display. So you could (for instance), plot a tiny graph, 
//end then draw this on the display. 
//The window must be a multiple of 8 pixels high, and can only be placed 
//at a 'text' location, so you can't put it (say) 12 pixels down the screen, 
//but only 8, 16, 24 etc.. 
//The size of the window determines how much RAM is used. So a 64*16 window 
//uses 128 bytes of RAM (64*16/8). 
//Neat thing is though, that you can draw an image on the window, put this on 
//the screen, and then draw a second image, and put this somewhere else, 
//without using any more memory. At the moment, I have only implemented two 
//functions to draw to this window. 
//If you don't want the graphic ability, if you #define TEXT_ONLY, then only 
//the text mode driver will be loaded. 
//With the graphic driver being used, the following extra functions are 
//available: 
//   clear_window();          //clears all pixels 'black' (if set==TRUE), or white. 
//   set_pixel(x, y);         //sets a pixel at x,y in the window. 
//                            //x=0 to WINDOW_WIDTH-1 (left to right) 
//                            //y=0 to WINDOW_HEIGHT-1 (top to bottom) 
//   line(x1, y1, x2, y2);    //draws a line from x1,y1 to x2, y2 
//   rect(x1, y1, x2, y2);    //draws a rectangle 
//   circle(x, y, radius, fill); //draws a circle 
//   //This draws a circle of radius 'radius' centred at x,y. If 'fill' is 
//   //true this is filled.... 
// 
//   draw_window(x, int8 y);  //This draws the window onto the screen at 
//                            //x=0 to 128, y=0 to 7. 
//What you do is simply draw the shape you want into the window, and then 
//this can be drawn onto the screen. 
//I have not included font drawing, since this takes a lot more space.... 
//'set' again controls whether a pen, or eraser is used. So (for example), if you 
//wanted a 'thick' circle, you could either draw several using the pen, 
//without 'fill' enabled, or could draw one with fill, then change set to 
//false, and draw a smaller one, to give a thick ring. 


#define COMMAND_ONLY 0b00000000 //next byte is a command only 
#define DATA_ONLY 0b01000000 //next byte is data 

//directly from the data sheet - commands - not all used 
#define S_EXTERNALVCC            0x1 
#define S_SWITCHCAPVCC           0x2 
#define S_SETLOWCOLUMN           0x00 
#define S_SETHIGHCOLUMN          0x10 
#define S_MEMORYMODE             0x20 
#define S_COLUMNADDR             0x21 
#define S_PAGEADDR               0x22 
#define S_SETSTARTLINE           0x40 
#define S_ROWADDRESS             0xB0 
#define S_SETCONTRAST            0x81 
#define S_CHARGEPUMP             0x8D 
#define S_SEGREMAP               0xA0 
#define S_DISPLAYALLON_RESUME    0xA4 
#define S_DISPLAYALLON           0xA5 
#define S_NORMALDISPLAY          0xA6 
#define S_INVERTDISPLAY          0xA7 
#define S_SETMULTIPLEX           0xA8 
#define S_DISPLAYOFF             0xAE 
#define S_DISPLAYON              0xAF 
#define S_COMSCANINC             0xC0 
#define S_COMSCANDEC             0xC8 
#define S_SETDISPLAYOFFSET       0xD3 
#define S_SETCOMPINS             0xDA 
#define S_SETVCOMDETECT          0xDB 
#define S_SETDISPLAYCLOCKDIV     0xD5 
#define S_SETPRECHARGE           0xD9 
#define DIV_RATIO                0x80 //recommended ratio 
#define MULTIPLEX                0x3F //and multiplex 
#define INT_VCC                  0x14 

//Font 6*8 - slightly clearer than most fonts this size. 
ROM BYTE font[] = 
{ 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,      // Code for char 
        0x00, 0x00, 0xBE, 0x00, 0x00, 0x00,      // Code for char ! 
        0x00, 0x00, 0x03, 0x00, 0x03, 0x00,      // Code for char " 
        0x50, 0xF8, 0x50, 0xF8, 0x50, 0x00,      // Code for char # 
        0x48, 0x54, 0xFE, 0x54, 0x24, 0x00,      // Code for char $ 
        0x98, 0x58, 0x20, 0xD0, 0xC8, 0x00,      // Code for char % 
        0x60, 0x9C, 0xAA, 0x44, 0x80, 0x00,      // Code for char & 
        0x00, 0x00, 0x00, 0x03, 0x00, 0x00,      // Code for char ' 
        0x00, 0x38, 0x44, 0x82, 0x00, 0x00,      // Code for char ( 
        0x00, 0x82, 0x44, 0x38, 0x00, 0x00,      // Code for char ) 
        0x02, 0x06, 0x03, 0x06, 0x02, 0x00,      // Code for char * 
        0x10, 0x10, 0x7C, 0x10, 0x10, 0x00,      // Code for char + 
        0xA0, 0x60, 0x00, 0x00, 0x00, 0x00,      // Code for char , 
        0x10, 0x10, 0x10, 0x10, 0x10, 0x00,      // Code for char - 
        0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00,      // Code for char . 
        0x80, 0x40, 0x20, 0x10, 0x08, 0x00,      // Code for char / 
        0x7C, 0xA2, 0x92, 0x8A, 0x7C, 0x00,      // Code for char 0 
        0x00, 0x84, 0xFE, 0x80, 0x00, 0x00,      // Code for char 1 
        0xC4, 0xA2, 0x92, 0x92, 0x8C, 0x00,      // Code for char 2 
        0x44, 0x82, 0x92, 0x92, 0x6C, 0x00,      // Code for char 3 
        0x18, 0x14, 0x12, 0xFE, 0x10, 0x00,      // Code for char 4 
        0x9E, 0x92, 0x92, 0x92, 0x62, 0x00,      // Code for char 5 
        0x7C, 0x92, 0x92, 0x92, 0x64, 0x00,      // Code for char 6 
        0x06, 0x02, 0xE2, 0x12, 0x0E, 0x00,      // Code for char 7 
        0x6C, 0x92, 0x92, 0x92, 0x6C, 0x00,      // Code for char 8 
        0x4C, 0x92, 0x92, 0x92, 0x7C, 0x00,      // Code for char 9 
        0xCC, 0xCC, 0x00, 0x00, 0x00, 0x00,      // Code for char : 
        0xAC, 0x6C, 0x00, 0x00, 0x00, 0x00,      // Code for char ; 
        0x00, 0x10, 0x28, 0x44, 0x82, 0x00,      // Code for char < 
        0x48, 0x48, 0x48, 0x48, 0x48, 0x00,      // Code for char = 
        0x00, 0x82, 0x44, 0x28, 0x10, 0x00,      // Code for char > 
        0x04, 0x02, 0xB2, 0x12, 0x0C, 0x00,      // Code for char ? 
        0x7C, 0x82, 0xBA, 0xAA, 0xBC, 0x00,      // Code for char @ 
        0xF8, 0x14, 0x12, 0x14, 0xF8, 0x00,      // Code for char A 
        0xFE, 0x92, 0x92, 0x92, 0x6C, 0x00,      // Code for char B 
        0x7C, 0x82, 0x82, 0x82, 0x44, 0x00,      // Code for char C 
        0xFE, 0x82, 0x82, 0x44, 0x38, 0x00,      // Code for char D 
        0xFE, 0x92, 0x92, 0x82, 0x82, 0x00,      // Code for char E 
        0xFE, 0x12, 0x12, 0x02, 0x02, 0x00,      // Code for char F 
        0x7C, 0x82, 0x92, 0x92, 0xF4, 0x00,      // Code for char G 
        0xFE, 0x10, 0x10, 0x10, 0xFE, 0x00,      // Code for char H 
        0x00, 0x82, 0xFE, 0x82, 0x00, 0x00,      // Code for char I 
        0x60, 0x80, 0x80, 0x80, 0x7E, 0x00,      // Code for char J 
        0xFE, 0x10, 0x18, 0x24, 0xC2, 0x00,      // Code for char K 
        0xFE, 0x80, 0x80, 0x80, 0x80, 0x00,      // Code for char L 
        0xFE, 0x04, 0x38, 0x04, 0xFE, 0x00,      // Code for char M 
        0xFE, 0x04, 0x08, 0x10, 0xFE, 0x00,      // Code for char N 
        0x7C, 0x82, 0x82, 0x82, 0x7C, 0x00,      // Code for char O 
        0xFE, 0x12, 0x12, 0x12, 0x0C, 0x00,      // Code for char P 
        0x7C, 0x82, 0xA2, 0xC2, 0xFC, 0x00,      // Code for char Q 
        0xFE, 0x12, 0x12, 0x12, 0xEC, 0x00,      // Code for char R 
        0x4C, 0x92, 0x92, 0x92, 0x64, 0x00,      // Code for char S 
        0x02, 0x02, 0xFE, 0x02, 0x02, 0x00,      // Code for char T 
        0x7E, 0x80, 0x80, 0x80, 0x7E, 0x00,      // Code for char U 
        0x3E, 0x40, 0x80, 0x40, 0x3E, 0x00,      // Code for char V 
        0xFE, 0x80, 0x70, 0x80, 0xFE, 0x00,      // Code for char W 
        0xC6, 0x28, 0x10, 0x28, 0xC6, 0x00,      // Code for char X 
        0x06, 0x08, 0xF0, 0x08, 0x06, 0x00,      // Code for char Y 
        0xC2, 0xA2, 0x92, 0x8A, 0x86, 0x00,      // Code for char Z 
        0x00, 0xFE, 0x82, 0x82, 0x00, 0x00,      // Code for char [ 
        0x08, 0x10, 0x20, 0x40, 0x80, 0x00,      // Code for char BackSlash 
        0x00, 0x82, 0x82, 0xFE, 0x00, 0x00,      // Code for char ] 
        0x00, 0x08, 0x04, 0x02, 0x04, 0x08,      // Code for char ^ 
        0x80, 0x80, 0x80, 0x80, 0x80, 0x00,      // Code for char _ 
        0x00, 0x00, 0x02, 0x04, 0x00, 0x00,      // Code for char ` 
        0x40, 0xA8, 0xA8, 0xA8, 0xF0, 0x00,      // Code for char a 
        0xFE, 0x88, 0x88, 0x88, 0x70, 0x00,      // Code for char b 
        0x70, 0x88, 0x88, 0x88, 0x10, 0x00,      // Code for char c 
        0x70, 0x88, 0x88, 0x88, 0xFE, 0x00,      // Code for char d 
        0x70, 0xA8, 0xA8, 0xA8, 0x30, 0x00,      // Code for char e 
        0x10, 0xFC, 0x12, 0x12, 0x04, 0x00,      // Code for char f 
        0x90, 0xA8, 0xA8, 0xA8, 0x70, 0x00,      // Code for char g 
        0xFE, 0x10, 0x10, 0x10, 0xE0, 0x00,      // Code for char h 
        0x00, 0x90, 0xF4, 0x80, 0x00, 0x00,      // Code for char i 
        0x40, 0x80, 0x80, 0x90, 0x74, 0x00,      // Code for char j 
        0xFE, 0x20, 0x50, 0x88, 0x00, 0x00,      // Code for char k 
        0x7E, 0x80, 0x80, 0x00, 0x00, 0x00,      // Code for char l 
        0xF8, 0x08, 0x70, 0x08, 0xF0, 0x00,      // Code for char m 
        0xF8, 0x08, 0x08, 0x08, 0xF0, 0x00,      // Code for char n 
        0x70, 0x88, 0x88, 0x88, 0x70, 0x00,      // Code for char o 
        0xF8, 0x28, 0x28, 0x28, 0x10, 0x00,      // Code for char p 
        0x10, 0x28, 0x28, 0xF8, 0x80, 0x00,      // Code for char q 
        0xF8, 0x08, 0x08, 0x08, 0x10, 0x00,      // Code for char r 
        0x90, 0xA8, 0xA8, 0xA8, 0x48, 0x00,      // Code for char s 
        0x08, 0x08, 0xFE, 0x88, 0x88, 0x00,      // Code for char t 
        0x78, 0x80, 0x80, 0x80, 0xF8, 0x00,      // Code for char u 
        0x38, 0x40, 0x80, 0x40, 0x38, 0x00,      // Code for char v 
        0xF8, 0x80, 0x70, 0x80, 0xF8, 0x00,      // Code for char w 
        0x88, 0x50, 0x20, 0x50, 0x88, 0x00,      // Code for char x 
        0x18, 0xA0, 0xA0, 0xA0, 0x78, 0x00,      // Code for char y 
        0x88, 0xC8, 0xA8, 0x98, 0x88, 0x00,      // Code for char z 
        0x00, 0x10, 0x6C, 0x82, 0x00, 0x00,      // Code for char { 
        0x00, 0x00, 0xFE, 0x00, 0x00, 0x00,      // Code for char | 
        0x00, 0x82, 0x6C, 0x10, 0x00, 0x00,      // Code for char } 
        0x00, 0x08, 0x04, 0x08, 0x10, 0x08,      // Code for char ~ 
        0x7C, 0x7C, 0x00, 0x00, 0x00, 0x00,      // Code for char  
//Characters 32 to 127        
//The next eleven characters are above 127, and give the shapes used for 
//the bar graph capability - remove if not needed 
        0x82, 0x82, 0x82, 0x82, 0x82, 0x82,      //top and bottom bars only 128 
        
        0xFE, 0x82, 0x82, 0x82, 0x82, 0x82,      //Open for bar 129 
        0xFE, 0xFE, 0x82, 0x82, 0x82, 0x82,      //second bar 
        0xFE, 0xFE, 0xFE, 0x82, 0x82, 0x82, 
        0xFE, 0xFE, 0xFE, 0xFE, 0x82, 0x82, 
        0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0x82,              
        0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE,      
        //Full block for bar 134 
        
        0x82, 0x82, 0x82, 0x82, 0xFE, 0xFE,      //final one cloing shape 135 
        0xFE, 0x82, 0x82, 0x82, 0xFE, 0xFE,      //single left and double right        
        0xFE, 0xFE, 0x82, 0x82, 0xFE, 0xFE, 
        0xFE, 0xFE, 0xFE, 0x82, 0xFE, 0xFE, 
        0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE,        
        0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE       //New closing point for 0..100 
        //final one closing shape 140 
}; 
//Character 140 
/*
ROM BYTE init_sequence[] =S_DISPLAYOFF,
       S_SETDISPLAYCLOCKDIV, 
       DIV_RATIO,                  
       S_SETMULTIPLEX, 
       MULTIPLEX, 
       S_SETDISPLAYOFFSET, 
       0,                                   // no offset 
       S_SETSTARTLINE, 
       S_CHARGEPUMP, 
       INT_VCC,                             // using internal VCC 
       S_MEMORYMODE,                        //Since byte is vertical writing column by column 
       0,                                   // default horizontal addressing 
       (S_SEGREMAP | 0x1),                  // rotate screen 180 
       S_COMSCANDEC,                        
       S_SETCOMPINS,                  
       0x12, 
       S_SETCONTRAST, 
       0xCF,                                //experiment.... 0xCf for 1306 
       S_SETPRECHARGE, 
       0xF1, 
       S_SETVCOMDETECT, 
       0x40, 
       S_DISPLAYALLON_RESUME, 
       S_NORMALDISPLAY, 
       S_DISPLAYON;                         //switch on OLED    
       */
//Initilalisation sequence                          
void ssd_init()
{
   i2c_start (); 
   i2c_write (SSDADDR); //select the display 
   i2c_write (COMMAND_ONLY); //we are sending a command         
   i2c_write(S_DISPLAYOFF); 
   i2c_stop();
   i2c_start (); 
   i2c_write (SSDADDR); //select the display 
   i2c_write (COMMAND_ONLY); //we are sending a command         
   i2c_write(S_CHARGEPUMP); 
   i2c_write(0x14);
   i2c_stop(); 
   i2c_start (); 
   i2c_write (SSDADDR); //select the display 
   i2c_write (COMMAND_ONLY); //we are sending a command         
   i2c_write(S_MEMORYMODE); 
   i2c_write(0x10);
   i2c_stop();
   i2c_start (); 
   i2c_write (SSDADDR); //select the display 
   i2c_write (COMMAND_ONLY); //we are sending a command         
   i2c_write(0xA1);
   i2c_stop();
   i2c_start (); 
   i2c_write (SSDADDR); //select the display 
   i2c_write (COMMAND_ONLY); //we are sending a command         
   i2c_write(S_COMSCANDEC); 
   i2c_stop();
   i2c_start (); 
   i2c_write (SSDADDR); //select the display 
   i2c_write (COMMAND_ONLY); //we are sending a command         
   i2c_write(S_SETCOMPINS);
   i2c_write(0x12);
   i2c_stop();
   i2c_start (); 
   i2c_write (SSDADDR); //select the display 
   i2c_write (COMMAND_ONLY); //we are sending a command         
   i2c_write(S_SETCONTRAST);
   i2c_write(0x1F);
   i2c_stop();
   i2c_start (); 
   i2c_write (SSDADDR); //select the display 
   i2c_write (COMMAND_ONLY); //we are sending a command         
   i2c_write(S_DISPLAYALLON_RESUME);
   i2c_stop();
   i2c_start (); 
   i2c_write (SSDADDR); //select the display 
   i2c_write (COMMAND_ONLY); //we are sending a command         
   i2c_write(S_NORMALDISPLAY);
   i2c_stop();
   i2c_start (); 
   i2c_write (SSDADDR); //select the display 
   i2c_write (COMMAND_ONLY); //we are sending a command         
   i2c_write(S_DISPLAYON);
   i2c_stop();
}
          
//Initilalisation sequence
#define NORMAL 0 
#define DOUBLE_HEIGHT 1 
#define DOUBLE_WIDTH 2 
#define LARGE DOUBLE_HEIGHT+DOUBLE_WIDTH 
unsigned int8 O_current_col,O_current_row; //where text is currently 'working' 
int8 size=NORMAL; 

//Global flag for drawing mode 
int1 set=TRUE; //allow funstions to set or reset - inverts drawing functions 

#ifndef TEXT_ONLY 
unsigned int8 window_buffer[WINDOW_WIDTH*WINDOW_HEIGHT/8]; 
//so with the example given, 128 bytes of RAM - much more practical on small chips!... 
//This is the 'graphic window' buffer, so not needed for text only 
#endif 

void OLED_commands(rom unsigned int8 * commands, unsigned int8 number) 
//send a multiple command, or commands to the display - number says how many 
//from a ROM buffer 
{ 
   int8 ctr; //counter for the transmission 
   i2c_start (); 
   i2c_write (SSDADDR); //select the display 
   i2c_write (COMMAND_ONLY); //we are sending a command      
   for (ctr=0;ctr<number;ctr++) 
   {   
      I2c_write(commands[ctr]); 
   } 
   i2c_stop(); 
} 

void OLED_data(unsigned int8 * data, unsigned int8 number) 
//send 'number' bytes of data to display - from RAM 
{ 
   int8 ctr; 
   i2c_start (); 
   i2c_write (SSDADDR); //select the display 
   i2c_write (DATA_ONLY); //we are sending data(s) 
   for (ctr=0;ctr<number;ctr++) 
      i2c_write(data[ctr]); //send the byte(s) 
   i2c_stop ();    
} 

void OLED_address(unsigned int8 x, unsigned int8 y) 
{ 
   //routine to move the memory pointers to x,y. 
   //x is 0 to 127 (column), y (row) is 0 to 7 (page only) 
#ifdef SH1106 
   x+=2; 
#endif 
   i2c_start(); 
   i2c_write (SSDADDR); //select the display    
   i2c_write(COMMAND_ONLY); //we are sending command(s) 
   i2c_write(S_ROWADDRESS | y); //select the display row 
   i2c_write(S_SETLOWCOLUMN | (x & 0x0F)); //low col address 
   i2c_write(S_SETHIGHCOLUMN | ((x>>4) & 0x0F)); //high col address 
   i2c_stop(); 
} //also made more efficient 


void OLED_gotoxy(unsigned int8 x, unsigned int8 y) 
{ 
   //text x,y position bases on 8 lines/character and 6 columsn 
   //0 to 20 columns, 0 to 7 rows 
   if (x>(S_LCDWIDTH/6)-1) return; 
   if (y>(S_LCDHEIGHT/8)-1) return; 
   O_current_col=x; //efficient *6 
   O_current_row=y; 
   OLED_address((unsigned int16)x*4+(unsigned int16)x*2,y); //position display 
} 

void OLED_CLS(void) 
{ 
   unsigned int8 row, col; 

   //Just fill the memory with zeros 
   for (row=0;row<S_LCDHEIGHT/8;row++) 
   { 
      OLED_address(0,row); //take the addresses back to 0,0 0,1 etc.. 
      i2c_start(); 
      i2c_write(SSDADDR); //select the display 
      i2c_write(DATA_ONLY); //we are sending data(s) 
      for (col=0;col<S_LCDWIDTH;col++) 
      { 
         if (set) 
            i2c_write (0); //send 1024 zeros 
         else 
            i2c_write(255); //or the inverse 
      } 
      i2c_stop (); 
   } 
   OLED_gotoxy(0,0); //and text back to the top corner    
} 

//Macros to efficiently double bits from a nibble 
#define DOUBLE_BIT(N, S, D) if (bit_test(S,N)) { bit_set(D,(N*2)); bit_set(D,(N*2)+1); } 
#define DOUBLE_B_HIGH(N, S, D) if (bit_test(S,N+4)) { bit_set(D,(N*2)); bit_set(D,(N*2)+1); } 

void invert(unsigned int8 * buffer, unsigned int8 number) //routine to invert 
{//data when'set==FALSE' 
   do 
   { 
      *buffer^=0xFF; 
      buffer++; 
   } while(--number); //invert all the bits in the buffer 
} 

//Change here to allow multiple fonts 
//This routine can be used by multiple output routines 
void FONT_line(ROM unsigned int8 * font_data, int8 count) 
{ 
   //new function to transfer a line of data from the font table. 
   //designed to optimise the handling of double size fonts 
   //Sends one line of 'count' characters from the font to the display, 
   //with doubling of width if necessary. Maximum 12 chars. 
   unsigned int8 cols[24], ctr=0, width, inc_col, tchr, temp=0; 
   if (size & DOUBLE_WIDTH) 
   { 
      width=count*2; 
      inc_col=2; 
   } 
   else 
   { 
      width=count; 
      inc_col=1; 
   } 
      

   for (ctr=0;ctr<width;ctr+=inc_col) //for columns 
   { 
      cols[ctr]=0; 
      tchr=font_data[temp++]; //one byte of character from the font 
      if (size & DOUBLE_HEIGHT) 
      { 
         DOUBLE_BIT(0,tchr,cols[ctr]) 
         DOUBLE_BIT(1,tchr,cols[ctr]) 
         DOUBLE_BIT(2,tchr,cols[ctr]) 
         DOUBLE_BIT(3,tchr,cols[ctr])                
      } //efficently double the bits from the low nibble 
      else 
         cols[ctr]=tchr; 
      if (size & DOUBLE_WIDTH) 
         cols[ctr+1]=cols[ctr]; //duplicate the byte 
   } 
   if (set==FALSE) 
      invert(cols,width); 
   OLED_data(cols,width); 
   //Now if doubling in height repeat usiong the other nibble 
   if (size & DOUBLE_HEIGHT) 
   { 
      temp=0; //back to the start of the font character 
      OLED_address((unsigned int16)O_current_col*4+O_current_col*2,O_current_row+1);  //next row 
      for (ctr=0;ctr<width;ctr+=inc_col) //through the columns again 
      { 
         cols[ctr]=0; 
         tchr=font_data[temp++]; //one byte of character from the font 
         DOUBLE_B_HIGH(0,tchr,cols[ctr]) 
         DOUBLE_B_HIGH(1,tchr,cols[ctr]) 
         DOUBLE_B_HIGH(2,tchr,cols[ctr]) 
         DOUBLE_B_HIGH(3,tchr,cols[ctr]) 
         //efficently double the bits from the high nibble 
         if (size & DOUBLE_WIDTH) 
            cols[ctr+1]=cols[ctr]; //duplicate the byte if double_width 
      } 
      if (set==FALSE) 
         invert(cols,width);          
      //Now send the bytes for the second row 
      OLED_data(cols,width); 
   } 
} 

//Fastest text mode. No support for any control. Standard font only. 
void OLED_text(unsigned int8 * text, unsigned int8 number) 
{ 
   unsigned int8 inc_col; 
   unsigned int16 temp; 
   //size allows double height & double width 
   //Here double height/width 
   //for this I have to do two transfers each of double the amount of data 
   //and reposition between each 
   if (size & DOUBLE_WIDTH) 
   { 
      inc_col=2; 
   } 
   else 
   { 
      inc_col=1; 
   } 
      
   do { 
      temp=(*text)-32; 
      if (temp>108) 
         temp=0; //block illegal characters          
      temp=((unsigned int16)temp*2)+((unsigned int16)temp*4); //efficient *6 
      FONT_line(&font[temp],6); //six characters from the font 
      //Now because I'll be in the wrong position (may be one line down) 
      //have to re-locate 
      O_current_col+=inc_col; 
      OLED_address((unsigned int16)O_current_col*4+O_current_col*2,O_current_row);  //ready for next character 
      text++; //and select the next character 
   } while (--number); //and on to the next character 
} 

void OLED_putc(unsigned int8 chr) 
{ 
   //this is a putc wrapper for the text function - note much slower 
   //than sending the entire line directly 
   if (chr=='\f') 
   { 
      OLED_CLS(); //handle Clear screen (form feed) 
      return; 
   } 
   if (chr=='\r') 
   { 
      OLED_gotoxy(0,O_current_row); //carriage return 
      return; 
   } 
   if (chr=='\n') 
   { 
      if (size==NORMAL) 
         OLED_gotoxy(O_current_col,O_current_row+1); 
      else 
         OLED_gotoxy(O_current_col,O_current_row+2); 
      return; 
   } 
   OLED_text(&chr,1); 
} 

void OLED_textbar(unsigned int8 width) 
{ 
   //This draws a bar graph using text characters 
   int8 ctr; 
   unsigned int8 bar[9]; 
   //graph is now 0 to 100. 
   //prints at current text location. 
   width/=2; 
   width+=2;  //ensure >0 - g1ves 2 to 52 
   if (width>52) 
      width=52; 
   for (ctr=0;ctr<8;ctr++) 
   { 
      if (width>=6) 
      { 
         bar[ctr]=134; //full bar 
         width-=6; 
      } 
      else 
      {        
         bar[ctr]=128+width; //partial bars 
         width=0; 
      } 
   } 
   //now handle the right hand end of the bar 
   bar[8]=135+width; 
   OLED_text(bar,9); 
} 

//Now comes the 'exception' part to the driver 
//Basic line & circle code writing to a small _window_ that can then 
//be burst transmitted to the LED. Neat thing is that the same window 
//can be used multiple times. So (for instance) you could write text on the 
//left of the display, then have a 64*32 window. Draw something into this 
//and display it at 64, 0 (text row) on the display, then draw something 
//different (using the same window), and put this at 64, 4. The location 
//the window is drawn has to be a byte boundary (so 0 to 7, for 0 to 63 
//on the display. 
//The sequence would be clear_window, line, circle etc.. Then draw_window(x,y) 
//If you don't want to use the smace for this, then #define TEXT_ONLY at the start 
//of the code 

#ifndef TEXT_ONLY 
void clear_window(void) 
{ 
   memset(window_buffer,(set)?0:255,WINDOW_WIDTH*WINDOW_HEIGHT/8); //clear the buffer 
} 

//Basic pixel routine 
#inline 
void set_pixel(unsigned int8 x, unsigned int8 y) 
{ 
   unsigned int16 locn; //This can be int8, if buffer is restricted to max 256 bytes 
   //potentially slightly faster. However 1616 is 'generic'. 
   if (x>=WINDOW_WIDTH) return; 
   if (y>=WINDOW_HEIGHT) return; //Limit check - ensures I do not try to write 
   //outside buffer boundaries 
   locn=((y/8)*WINDOW_WIDTH)+x; //location in buffer 
   //handle setting or resetting the pixel according to flag 'set' 
   if (set) 
      bit_set(window_buffer[locn],(y & 7)); //set the bit (to 1) 
   else 
      bit_clear(window_buffer[locn],(y & 7)); //set the bit (to 0) 
} 

//efficient line routine 
void line(unsigned int8 x1, unsigned int8 y1, unsigned int8 x2, unsigned int8 y2) 
{ 
   unsigned int16 i; 
   int1 _plot; 
   signed int16 _dx,_dy; 
   unsigned int16 _ix,_iy,_inc,_plotx,_ploty,_x,_y;    
   //line from X1,Y1 to X2,Y2 
   //first the differences between the coordinate pairs 
   _dx=(signed int16) x2-x1; 
   _dy=(signed int16) y2-y1; 
   // ix & iy are the absolute increments required 
   _ix=(_dx<0)?(-_dx):_dx; 
   _iy=(_dy<0)?(-_dy):_dy; 
   // we must step the longest length (x or y) 
   _inc=(_ix>_iy)?_ix:_iy;    
   // make dx and dy the step required. 
   if (_dx>0) _dx=1;   else if (_dx<0) _dx=-1; 
   if (_dy>0) _dy=1;   else if (_dy<0) _dy=-1; 
   // actual plotting points 
   _plotx=x1; 
   _ploty=y1;    
   // start at 0 
   _x=0; 
   _y=0; 
   // set endpoint 
   set_pixel(_plotx,_ploty);    
   /* we implement Bressenhams algorithm for a line using integer steps 
   and only plotting the point when we get to a new co-ord pair */ 
   for (i=0;i<=_inc;++i) 
   { 
      _x += _ix; 
      _y += _iy; 
      // do not plot yet 
      _plot=FALSE; 
      //if we are at a new pair - set the plot flag and increment 
      //the phsical plotting point 
      if (_x > _inc) 
      { 
         _plot = TRUE; 
         _x -= _inc; 
         _plotx += _dx; 
      } 
      if (_y > _inc) 
      { 
         _plot = TRUE; 
         _y -= _inc; 
         _ploty += _dy; 
      } 
      // now plot the point 
      if (_plot) 
      { 
         set_pixel(_plotx,_ploty); //this automatically handles set/reset 
      } 
   }    
} 

void rect(unsigned int8 x1, unsigned int8 y1, unsigned int8 x2, unsigned int8 y2) 
{ 
   //outline a rectangle 
   line(x1, y1, x2, y1);      //Just draw four sides 
   line(x1, y2, x2, y2); 
   line(x1, y1, x1, y2); 
   line(x2, y1, x2, y2);    
} 

void circle(unsigned int8 x, unsigned int8 y, unsigned int8 radius, int1 fill) 
{ 
   signed int8  a, b, P; 
   a = 0; 
   b = radius; 
   P = 1 - radius; 
   do 
   { 
      if(fill) 
      { 
         line(x-a, y+b, x+a, y+b); 
         line(x-a, y-b, x+a, y-b); 
         line(x-b, y+a, x+b, y+a); 
         line(x-b, y-a, x+b, y-a); //inefficient but easy to code.... 
      } 
      else 
      { 
         set_pixel(a+x, b+y); 
         set_pixel(b+x, a+y); 
         set_pixel(x-a, b+y); 
         set_pixel(x-b, a+y); 
         set_pixel(b+x, y-a); 
         set_pixel(a+x, y-b); 
         set_pixel(x-a, y-b); 
         set_pixel(x-b, y-a);  //othewise draw the octant points 
      } 
      if(P < 0) 
         P += 3 + 2 * a++; 
      else 
         P += 5 + 2 * (a++ - b--); 
    } while(a <= b); 
} 

void draw_window(unsigned int8 x, unsigned int8 y) 
{ 
   unsigned int8 yctr; 
   unsigned int16 transfer=WINDOW_WIDTH; 
   //routine to copy the window to the display. 
   //x is in pixels, y in bytes (0 to 8). Uses burst transmission for each line 
   if ((x+transfer)>=S_LCDWIDTH) 
   { 
      //here the window would go off the edge of the screen... 
      transfer=(S_LCDWIDTH-1)-x; 
   }      
   for (yctr=0;yctr<(WINDOW_HEIGHT/8);yctr++) 
   { 
      if (y+yctr>7) return; //off the end of RAM 
      OLED_address(x,y+yctr); //position to the byte at the start of the line 
      //transfer the line 
      OLED_data(window_buffer+((unsigned int16)yctr*WINDOW_WIDTH),transfer); 
      //again int16 only needed here if buffer>256 bytes 
   } 
} 
#endif   
ROM BYTE SLEEP_sequence[] =S_SETCONTRAST, 
0x00, 
S_DISPLAYOFF; 


ROM BYTE WAKEUP_sequence[] = 
S_DISPLAYON, 
S_SETCONTRAST, 
0x1F; //  0x00 to 0xFF 

