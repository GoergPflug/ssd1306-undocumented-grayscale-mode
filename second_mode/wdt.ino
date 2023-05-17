static volatile unsigned char vsync_flag=0;
unsigned char mux_0_countdown=0;
#include <stdlib.h>
#include <stdfix.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#define DISABLE_OFAST
///////////////////////////////////////////////////////////////////////////////
//Gfx-Api Configuration
///////////////////////////////////////////////////////////////////////////////
//#define ENABLE_CONSOLE_EXTENDED_COLOR
//#define ENABLE_CONSOLE_BIT7_BRIGHTNESS
#define ENABLE_CONSOLE_BIT7_INVERT
#define CONSOLE_ENABLE_PRINTF
//#define ENABLE_ARDUINO_SUPPORT
#define WIRE_SCREEN_ADDRESS 0x3C
///#define ENABLE_WIRE
//#define ENABLE_FONT_BASIC
#define CONSOLE_ENABLE_CHESSBOARD_DITHER
#define CONSOLE_SIZE_Y 8
#define CONSOLE_LINE_START 0
#define ENABLE_CONSOLE
//#define CONSOLE_ENABLE_CURSOR
//#define CONSOLE_ENABLE_CURSOR_BLINK
#define CONSOLE_CURSOR_BLINK_MASK 0x10
#define DISPLAYFUNC Display
#define PIXEL_CALLBACK pix

unsigned char pix( char x,  char y)
{
    if(y>32)y-=32,x-=64;
    if(x<0)return 0;
    if(x>64)x-=64;
    if(x<y)return 64;
    return 0;
  
}

#include "tiny_multi_os.h"
#include <avr/wdt.h>
static u8 console[128];



static volatile u8 flag=0;

ISR(WDT_vect)
{
  flag++;
  vsync_flag=1;
}
void MainTask() {
  int i;
  wdt_reset(); // Reset Watchdog Timer
WDTCSR = (1 << WDCE) | (1 << WDE); //Watchdog Change Enable
WDTCSR =0;// (1 << WDP3) | (1 << WDP0); //Time-out = 8 s
WDTCSR |= (1 << WDIE); //Watchdog  Mode = Interrupt Mode




  GfxApiAttachConsole(console);
  GfxApiSetInvert(0);
 // GfxApiSetFreq(0xff);
 // printf("24c512 test\n");
  
  int cnt = -30;
    long long a, b;
    u8 rd[2560];
   GfxApiSetFreq(0xf0);
 //eeprom_i2c_read(0x50 << 1, 0, rd, 2);
   // eeprom_i2c_write(0x50 << 1, 0, 99);
  
  for(u8 c=0;c<20;c++){
  GfxApiConsoleSetAttribute(0);
  if(c&1)GfxApiConsoleSetAttribute(1);
 // printf(".%d",rand());i=0;
  }
   
  for (;; cnt++)
  {
   
   i=0;
    TestCmd(1); 
    for(;;)
    {
    TestCmd((flag>>5)&1); 
     
    Display(0, 0, 0, console);
    }
   for(;;){
 
   flag=0;
    while(!flag);
    }
  }
}
