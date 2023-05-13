#define ENABLE_NEWMODE
#define _TUBE1BIT
//#define ENABLE_BARGRAPH 
#define TEMP_E 3
#define _SEGMENTED_FLIP_BIT 0x1
static unsigned char error_level_vfd;
//#define ENABLE_VFD_GRID
/* CPKI AttinyGfxApi & TinyMultiOs, Preview Version 0.9.3b1
see
https://www.youtube.com/watch?v=WNJQXsJqSbM
Copyright (c) 2002
Görg Pflug & CPKI Gmbh, www.cpki.de . All rights reserved.
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
All advertising materials mentioning features or use of this software must display the following acknowledgement: “This product includes software developed by the CPKI Gmbh, Görg Pflug and its contributors.”
Neither the name of the Cpki GmbH nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE Görg Pflug, CPKI Gmbh AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE

    
*/
//#ifdef ENABLE_BARGRAPH
        
//#endif


static unsigned char matrix_vfd[16];

//#define ENABLE_MATRIX_VFD


#define ENABLE_SEGMENTED



#ifndef _API_DEFINED
  #define _API_DEFINED
  #define CONSOLE_LINE_END (CONSOLE_LINE_START+(CONSOLE_SIZE_Y*8))


  #ifndef F_CPU 
    #define F_CPU 16000000
    #warning "CPU Clock: assuming 16mhz"
  #endif
  #include <util/delay.h>
  #include <avr/wdt.h>
  #include <avr/sleep.h>
  #include <avr/io.h>
  #include <util/delay.h>
  #include <stdint.h>
  #include <stdbool.h>
  #include <avr/io.h>
  #include <avr/pgmspace.h>
  #include <avr/boot.h>
  #include <avr/interrupt.h>
  #include <string.h>
  #include <avr/power.h>
  #include <util/delay.h>
  
  #define s8 signed char
  #define u8 unsigned char
  #define s16 signed int
  #define u16 unsigned int
  #define swapu8(x,y) { u8 tmp=x;x=y;y=tmp; }
  
  #define SSD1306_ADDRESS 0x78



static u8 _engine_flags_and_frame_counter=0;
  
// Attiny85
/*
  #define I2C_PIN PINB
  #define I2C_PORT PORTB
  #define I2C_DDR DDRB
  #define PIN_SDA  0
  #define PIN_SCL 2
  /**//* // Mega2560 port 20,21 */
  #define I2C_PORT PORTD
  #define I2C_PIN PIND
  #define I2C_DDR DDRD
  #define PIN_SDA  1
  #define PIN_SCL 0
/*/
#define I2C_PORT PORTB
#define I2C_DDR DDRB
#define PIN_SDA  5
#define PIN_SCL 4
*/



// helper function, Calculate Skip Counter from x,y coord
static inline int GfxApiPosition(unsigned char x, unsigned char y)
{
  return x*64+y;
}

  #ifdef ENABLE_LINEDRAWING
u8 _gfx_linepos= 0;
    #ifdef ENABLE_TRIANGLES
u8 _gfx_tripos=0;
    #endif
    #ifdef ENABLE_CIRCLES
u8 _gfx_circlepos=0;
    #endif


    #ifndef NR_TRIS
      #define NR_TRIS 0
    #endif
    #ifndef NR_CIRCLES
      #define NR_CIRCLES 0
    #endif
static unsigned char _gfx_points_of_lines[4 * NR_LINES+ 8 * NR_TRIS+4*NR_CIRCLES];

// Start storage of Line Points
static void GfxApiBeginLines()
{
  _gfx_linepos=0;
}
  #ifdef ENABLE_TRIANGLES
static void GfxApiBeginTriangles()
{
  _gfx_tripos=0;
}
    #endif
    #ifdef ENABLE_CIRCLES
static void GfxApiBeginCircles()
{
  _gfx_circlepos=0;
}
    #endif

    #define swapu8(x,y) { u8 tmp=x;x=y;y=tmp; }
void reorder_lines()
{
  u8 i;
  for(i=0;i<_gfx_linepos;i+=4)
  {
    if(_gfx_points_of_lines[i]>_gfx_points_of_lines[i+2])
    {
      swapu8(_gfx_points_of_lines[i],_gfx_points_of_lines[i+2]);
      swapu8(_gfx_points_of_lines[i+1],_gfx_points_of_lines[i+3]);
    }
  }
}

static void GfxApiStoreLinePoint(unsigned char x1, unsigned char y1)
{
  _gfx_points_of_lines[_gfx_linepos] = x1;
  _gfx_points_of_lines[_gfx_linepos+1] = y1;
  _gfx_linepos += 2;
}

    #ifdef ENABLE_CIRCLES
static void GfxApiStoreCircle(unsigned char x1, unsigned char y1,unsigned char radius,unsigned char pattern)
{
      #ifndef ENABLE_TRIANGLES
        #define _gfx_tripos 0a
      #endif
  _gfx_points_of_lines[_gfx_circlepos+_gfx_linepos+_gfx_tripos] = x1;
  _gfx_points_of_lines[_gfx_circlepos+1+_gfx_linepos+_gfx_tripos] = y1;
  _gfx_points_of_lines[_gfx_circlepos+2+_gfx_linepos+_gfx_tripos] = radius;
  _gfx_points_of_lines[_gfx_circlepos+3+_gfx_linepos+_gfx_tripos] = pattern;
  _gfx_circlepos += 4;

      #ifndef ENABLE_TRIANGLES
        #undef _gfx_tripos
      #endif
}
    #endif
    #ifdef ENABLE_TRIANGLES
static void GfxApiStoreTrianglePoint(unsigned char x1, unsigned char y1)
{
  _gfx_points_of_lines[_gfx_linepos+_gfx_tripos] = x1;
  _gfx_points_of_lines[_gfx_linepos+1+_gfx_tripos] = y1;
  _gfx_tripos += 2;
}
    #endif
  #endif
  #ifdef ENABLE_LINEDRAWING
static unsigned char _cur_seg=0;
  #endif
  
void ApiIntToHex(u16 in, u8 *out);
u8 ApiCharToFontIndex(u8 c);

typedef struct GfxApiSprite
{
  u16 SkipCounter;
  u16 readpos_byte;
  u8 sprite_height;
  u8 Color;
}GfxApiSprite;

typedef struct GfxApiBargraph
{
  u8 bargraph_type[8];
  u8 bargraph_value[8];
  s8 bargraph_falloff[8];
  u8 bargraph_falloff_change[8];
}GfxApiBargraph;


static u8 GfxApiReadSprite(GfxApiSprite *s);

typedef struct GfxApiCompressedLayer
{
  u16 SkipCounter;
  s8 PixelValue;
  u8 Bitpos;
  u16 BytePos;
} GfxApiCompressedLayer;

static void os_gfx_start_display_transfer();
static void os_i2c_start (void);

static void delay5nop()
{
  asm volatile ("nop");
  asm volatile ("nop");
  asm volatile ("nop");
  asm volatile ("nop");
  asm volatile ("nop");
}

#define os_i2c_scl_low()  do{ I2C_DDR |= (1<<PIN_SCL);} while (0)
#define os_i2c_sda_low()  do{ I2C_DDR |= (1<<PIN_SDA);} while(0)
#define os_i2c_sda_high() do {I2C_DDR &= ~(1<<PIN_SDA);} while(0)
#define os_i2c_scl_high()   do{ I2C_DDR &= ~(1<<PIN_SCL); } while(0)

#define os_i2c_scl_low_ppull() do{ I2C_PORT &= ~(1<<PIN_SCL);} while (0)
#define os_i2c_scl_high_ppull() do{ I2C_PORT |= (1<<PIN_SCL);} while (0)

#define os_i2c_sda_low_ppull() do{ I2C_PORT &= ~(1<<PIN_SDA);} while (0)
#define os_i2c_sda_high_ppull() do{ I2C_PORT |= (1<<PIN_SDA);} while (0)
/*
//deactivate push pull bus driving....for both scl and sda not needed on the ssd1306
#define os_i2c_scl_low_ppull os_i2c_scl_low
#define os_i2c_scl_high_ppull os_i2c_scl_high

#define os_i2c_sda_low_ppull os_i2c_sda_low
#define os_i2c_sda_high_ppull os_i2c_sda_high
//*/


/*//*   // bus in ppull mode...
#define os_i2c_scl_low os_i2c_scl_low_ppull
#define os_i2c_scl_high os_i2c_scl_high_ppull

#define os_i2c_sda_low os_i2c_sda_low_ppull
#define os_i2c_sda_high os_i2c_sda_high_ppull
//*/

void os_i2c_init (void);
static void os_i2c_write (const u8 *buf, u8 len);

#define __POS_IDX 3
#define __POS_FLIPB 0
#define __POS_SEG 0

                 //01234567
#define DITHER_A 0b11100000
#define DITHER_B 0b00000111

#define DITHER_C (~(DITHER_A|DITHER_B))


const u8 dither_temp3[] __attribute__((progmem)) = 
{

  
0x19,0xc0,0x2a,
//frame:1
0x86,0x20,0x95,
//frame:2
0x60,0x1f,0x40,
 
};

static u8 frame_counter_3;

const u8 cathode_image[] __attribute__((progmem)) = {
    0,0,~0b0000000,0,    //0
    0b0000000, 0b0001000,~0,0,
    0b0000000, 0b0001100,~0,0,
    0b0000000, 0b0011100,~0,0,
    
    0b0001000, 0b0000000,~0,0,
    0b0001000, 0b0010000,~0,0,
    0b0001000, 0b0010100,~0,0,
    0b0011000, 0b0100100,~0,0,
    
    0b0011000, 0b1100110,~0,0,
    0b0011100, 0b1100011,~0,0,
    0b0111100, 0b1000011,~0,0,
    0b0111110, 0b1000001,~0,0,
    
};

static u8 segment[32];
static s8 cathode_level[8];

const u8 sbox128[] __attribute__((progmem)) = {
119,35,88,0,97,108,27,20,82,58,29,101,21,57,105,107,51,98,1,45,8,11,52,25,36,116,65,28,19,24,76,6,64,31,3,72,67,7,39,113,23,4,90,125,112,66,22,40,104,84,54,124,2,117,70,61,60,5,44,48,86,18,13,34,85,59,80,110,89,115,69,43,33,122,14,118,123,30,10,81,87,92,46,32,9,53,127,56,15,49,103,37,74,73,91,77,50,100,38,55,95,17,47,109,42,96,121,62,26,83,99,78,16,41,126,120,106,68,63,12,79,94,111,71,114,75,93,102,
};
#include "/current/vfd_simulator/xxx.h"
//#include "/current/vfd_simulator/dg10s.h"

const u8 _segmented_font[] __attribute__((progmem)) = {
#define A 1
#define B 2
#define C 4
#define D 8
#define E 16
#define F 32
#define G 64
#define H 128
#define I 256

    0, // space
    H+F+G+C+B+A,
    D,
    H+G+E+C+A, //2
    H+G+B+A+E,
    F+E+D, //4
    H+F+E+B+A, //5
    H+F+E+B+A+C,
    H+G+B,
    H+F+G+E+C+B+A, //8
    H+F+G+E+B+A,  //9

    F+G+H+E+C+B, //A
    F+C+A+B+E,
    H+F+C+A,  // c
    G+C+A+B+E,
    H+F+C+A+E,  // e
    H+F+C+E,  // f
    H+F+E+B+A+C,
    F+C+E+G+B,
    E,
    A+B+G,
    C+F+D+E,
    F+C+A,
    F+G+H+E+C+B+D, //M
    C+E+B,
    H+F+G+C+B+A,
    C+F+G+E+H, // p
    F+H+G+B,
    C+E,
    F+E+H+B+A,
    H+D,
    F+C+A+B+G,
    C+A+B+G,
    F+C+A+B+G+D,
    F+C+B+G+E,
    F+E+G+B,    
    H+G+E+C+A, //2
    
#undef A
#undef B
#undef C
#undef D
#undef E
#undef F
#undef G
#undef H
#undef I




};
//#include "/current/dg12.h"

const u8 iv17_pattern[] __attribute__((progmem)) = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,244,95,0,0,0,0,0,128,191,251,5,0,0,0,0,240,255,223,15,0,0,0,0,252,109,123,123,0,0,0,0,223,255,189,223,0,0,0,128,253,110,239,253,3,0,0,192,223,223,251,255,15,0,0,176,247,107,255,219,31,0,0,248,254,255,191,255,93,0,0,108,191,253,247,123,127,0,0,254,251,239,254,254,219,0,0,255,191,253,239,239,255,0,128,111,247,219,190,251,237,1,128,253,255,255,235,190,187,7,192,223,109,251,255,239,254,3,224,255,255,223,125,255,255,14,224,171,254,123,239,119,219,15,176,223,111,247,251,222,255,27,240,183,254,190,255,127,119,31,248,221,251,251,255,239,255,31,120,255,223,110,191,253,254,63,216,255,253,255,247,255,91,59,252,183,223,237,127,251,255,63,252,253,247,191,255,175,239,127,252,255,191,237,183,253,254,126,222,127,251,127,255,95,239,119,252,246,191,251,191,251,253,126,254,127,239,255,255,190,191,255,222,223,123,251,255,255,239,125,254,253,255,191,127,215,189,247,191,111,191,251,255,255,255,253,251,251,247,255,255,118,221,251,255,255,126,255,255,255,251,254,190,221,247,251,255,255,254,223,254,255,222,254,191,237,191,255,246,219,247,175,255,255,247,123,124,255,253,255,255,255,190,255,254,255,127,183,255,253,127,91,252,187,223,255,219,255,255,119,252,254,253,255,255,223,246,125,220,239,255,238,254,253,255,55,248,127,183,255,191,223,127,63,248,251,255,255,251,251,237,63,248,222,255,173,126,191,255,47,208,255,91,255,223,247,255,29,240,255,254,255,254,255,253,23,224,239,255,219,251,182,223,14,192,253,191,255,254,255,255,5,192,95,235,254,223,126,123,7,128,255,255,191,255,239,223,3,128,253,255,251,251,255,247,1,0,255,110,253,223,182,127,1,0,218,255,183,251,255,126,0,0,252,255,255,255,237,183,0,0,248,219,246,223,255,63,0,0,208,254,127,251,253,14,0,0,160,255,255,191,175,11,0,0,192,223,118,247,251,3,0,0,0,250,223,127,127,0,0,0,0,252,255,253,95,0,0,0,0,208,246,239,23,0,0,0,0,128,255,126,1,0,0,0,0,0,168,23,0,0,0,0,0,0,128,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};
const u8 in19_pattern[] __attribute__((progmem)) = {
#include "/current/in19_4x5.h"
};

  #ifdef ENABLE_CONSOLE
    #define FONT
  #endif
  #ifdef ENABLE_SECOND_CONSOLE
    #define FONT
  #endif
  #ifdef FONT
const u8 os_font[] __attribute__((progmem)) = {
    #ifdef REPLACE_FONT
      #include REPLACE_FONT
    #else
      #ifdef ENABLE_FONT_BASIC
// source https://raw.githubusercontent.com/dhepper/font8x8/master/font8x8_basic.h , public domain
0,0,0,0,0,0,0,0,0,0,6,95,95,6,0,0,0,3,3,0,3,3,0,0,20,127,127,20,127,127,20,0,36,46,107,107,58,18,0,0,70,102,48,24,12,102,98,0,48,122,79,93,55,122,72,0,4,7,3,0,0,0,0,0,0,28,62,99,65,0,0,0,0,65,99,62,28,0,0,0,8,42,62,28,28,62,42,8,8,8,62,62,8,8,0,0,0,128,224,96,0,0,0,0,8,8,8,8,8,8,0,0,0,0,96,96,0,0,0,0,96,48,24,12,6,3,1,0,62,127,113,89,77,127,62,0,64,66,127,127,64,64,0,0,98,115,89,73,111,102,0,0,34,99,73,73,127,54,0,0,24,28,22,83,127,127,80,0,39,103,69,69,125,57,0,0,60,126,75,73,121,48,0,0,3,3,113,121,15,7,0,0,54,127,73,73,127,54,0,0,6,79,73,105,63,30,0,0,0,0,102,102,0,0,0,0,0,128,230,102,0,0,0,0,8,28,54,99,65,0,0,0,36,36,36,36,36,36,0,0,0,65,99,54,28,8,0,0,2,3,81,89,15,6,0,0,62,127,65,93,93,31,30,0,124,126,19,19,126,124,0,0,65,127,127,73,73,127,54,0,28,62,99,65,65,99,34,0,65,127,127,65,99,62,28,0,65,127,127,73,93,65,99,0,65,127,127,73,29,1,3,0,28,62,99,65,81,115,114,0,127,127,8,8,127,127,0,0,0,65,127,127,65,0,0,0,48,112,64,65,127,63,1,0,65,127,127,8,28,119,99,0,65,127,127,65,64,96,112,0,127,127,14,28,14,127,127,0,127,127,6,12,24,127,127,0,28,62,99,65,99,62,28,0,65,127,127,73,9,15,6,0,30,63,33,113,127,94,0,0,65,127,127,9,25,127,102,0,38,111,77,89,115,50,0,0,3,65,127,127,65,3,0,0,127,127,64,64,127,127,0,0,31,63,96,96,63,31,0,0,127,127,48,24,48,127,127,0,67,103,60,24,60,103,67,0,7,79,120,120,79,7,0,0,71,99,113,89,77,103,115,0,0,127,127,65,65,0,0,0,1,3,6,12,24,48,96,0,0,65,65,127,127,0,0,0,8,12,6,3,6,12,8,0,128,128,128,128,128,128,128,128,0,0,3,7,4,0,0,0,32,116,84,84,60,120,64,0,65,127,63,72,72,120,48,0,56,124,68,68,108,40,0,0,48,120,72,73,63,127,64,0,56,124,84,84,92,24,0,0,72,126,127,73,3,2,0,0,152,188,164,164,248,124,4,0,65,127,127,8,4,124,120,0,0,68,125,125,64,0,0,0,96,224,128,128,253,125,0,0,65,127,127,16,56,108,68,0,0,65,127,127,64,0,0,0,124,124,24,56,28,124,120,0,124,124,4,4,124,120,0,0,56,124,68,68,124,56,0,0,132,252,248,164,36,60,24,0,24,60,36,164,248,252,132,0,68,124,120,76,4,28,24,0,72,92,84,84,116,36,0,0,0,4,62,127,68,36,0,0,60,124,64,64,60,124,64,0,28,60,96,96,60,28,0,0,60,124,112,56,112,124,60,0,68,108,56,16,56,108,68,0,156,188,160,160,252,124,0,0,76,100,116,92,76,100,0,0,8,8,62,119,65,65,0,0,0,0,0,119,119,0,0,0,65,65,119,62,8,8,0,0,2,3,1,3,2,3,1,0,0,0,0,0,0,0,0,0,
        #define FONT_ASCII
        #define HAVE_FONT
      #endif
      #ifndef HAVE_FONT  // minimal font 0-9,A-Z
  0,0,0,0,0,0,0,0,0,60,98,82,74,70,60,0,0,0,68,66,126,64,64,
  0,0,100,82,82,82,82,76,0,0,36,66,66,74,74,52,0,0,48,40,36,
  126,32,32,0,0,46,74,74,74,74,50,0,0,60,74,74,74,74,48,0,0,
  2,2,98,18,10,6,0,0,52,74,74,74,74,52,0,0,12,82,82,82,82,60,0,0,
  124,18,18,18,18,124,0,0,126,74,74,74,74,52,0,0,60,66,66,66,66,
  36,0,0,126,66,66,66,36,24,0,0,126,74,74,74,74,66,0,0,126,10,
  10,10,10,2,0,0,60,66,66,82,82,52,0,0,126,8,8,8,8,126,0,0,0,
  66,66,126,66,66,0,0,48,64,64,64,64,62,0,0,126,8,8,20,34,
  64,0,0,126,64,64,64,64,64,0,0,126,4,8,8,4,126,0,0,126,4,8,16,
  32,126,0,0,60,66,66,66,66,60,0,0,126,18,18,18,18,12,0,0,60,66,82,98,
  66,60,0,0,126,18,18,18,50,76,0,0,36,74,74,74,74,48,0,2,2,2,126,2,2,
  2,0,0,62,64,64,64,64,62,0,0,30,32,64,64,32,30,0,0,62,64,32,32,64,
  62,0,0,66,36,24,24,36,66,0,2,4,8,112,8,4,2,0,0,66,98,82,74,70,66,0,
        #undef HAVE_FONT
        #undef FONT 
      #endif  

      #ifdef ENABLE_USERFONT
        #include "userfont.h"
      #endif
    #endif
};
  #endif

static void os_i2c_start (void)
{
  #ifndef ENABLE_WIRE
  os_i2c_scl_high();
  asm volatile("nop");
  asm volatile("nop");
  asm volatile("nop");
  asm volatile("nop");          
  os_i2c_sda_low();
  asm volatile("nop");
  asm volatile("nop");
  asm volatile("nop");
  asm volatile("nop");            
  os_i2c_scl_low();
  asm volatile("nop");
  asm volatile("nop");
  asm volatile("nop");
  asm volatile("nop");
  os_i2c_sda_high();
  #else

  #endif
}

static void os_i2c_stop (void)
{
  #ifndef ENABLE_WIRE
  asm volatile("nop");
  asm volatile("nop");
  asm volatile("nop");
  asm volatile("nop");
  os_i2c_sda_low();
  asm volatile("nop");
  asm volatile("nop");
  asm volatile("nop");
  asm volatile("nop");

  os_i2c_scl_high();
  asm volatile("nop");
  asm volatile("nop");
  asm volatile("nop");
  asm volatile("nop");
  os_i2c_sda_high();
  asm volatile("nop");
  asm volatile("nop");
  asm volatile("nop");
  asm volatile("nop");
  #else
  Wire.endTransmission();
  #endif
}



static u8 os_i2c_write_byte (u8 byte)
{
  #ifndef ENABLE_WIRE
  //USIDR = byte;
  //USISR = 7;
  u8 bitpos=0x80;
  
  do {
    os_i2c_scl_low();
    if(byte&bitpos)
      os_i2c_sda_high();
    else
      os_i2c_sda_low();
    
    os_i2c_scl_high();
    bitpos>>=1;

  } while (bitpos);
    os_i2c_scl_low();
 
  os_i2c_sda_high();               
  os_i2c_scl_high();
  asm volatile("nop");
  asm volatile("nop");
  asm volatile("nop");
  u8 ack=(I2C_PIN & (1<<PIN_SDA))?1:0;
  os_i2c_scl_low();
  os_i2c_sda_low();
  return ack;
  #else
  Wire.write(byte);
  #endif
}

static inline u8 os_i2c_read_byte (u8 ack)
{
  #ifndef ENABLE_WIRE
  os_i2c_scl_low();
  os_i2c_sda_high();
  
  u8 in=0;
  u8 bitpos=0x80;
  do {
    os_i2c_scl_high();
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
     
    if (I2C_PIN & (1 << PIN_SDA))
      in |= bitpos;
    
    bitpos>>=1;
    os_i2c_scl_low(); 
    asm volatile("nop");
    asm volatile("nop");
    
  } while (bitpos);
  
  if(ack)os_i2c_sda_low();
  
  os_i2c_scl_high();

  #else
xxx Wire.write(byte);
  #endif
  return in;
}


void os_i2c_init (void)
{
  #ifndef ENABLE_WIRE
  
  I2C_PORT = 0;//_BV(PIN_SDA) | _BV(PIN_SCL);
  //I2C_DDR  = _BV(PIN_SDA) | _BV(PIN_SCL);
  os_i2c_scl_high();
               asm volatile("nop");
               asm volatile("nop");
               asm volatile("nop");
               asm volatile("nop");
               asm volatile("nop");
               asm volatile("nop");
  os_i2c_sda_high();
               asm volatile("nop");
               asm volatile("nop");
               asm volatile("nop");
               asm volatile("nop");
               asm volatile("nop");
               asm volatile("nop");

  #else
  Wire.begin();
  Wire.setClock(800000);
  #endif
}

static void os_i2c_write (const u8 *buf, u8 len)
{
  #ifndef ENABLE_WIRE
  os_i2c_start();
  os_i2c_write_byte(SSD1306_ADDRESS);
  while (len--)
    os_i2c_write_byte(*buf++);
  os_i2c_stop();
  #else
  Wire.beginTransmission(WIRE_SCREEN_ADDRESS);
  while (len--)
    Wire.write(*buf++);
  Wire.endTransmission();
  #endif
}

// 24c512 support
static void eeprom_connect(u8 address, u16 from_addr)
{
  for(;;)
  {
    os_i2c_start();
    u8 ack=os_i2c_write_byte(address);
    if(!ack)break;
    os_i2c_stop();
//    about 150 loops when called in sequence
  } 
  os_i2c_write_byte(from_addr>>8);
  os_i2c_write_byte(from_addr);

}
void eeprom_i2c_write(u8 address, u16 from_addr, u8 data) {
 eeprom_connect(address,from_addr);
  os_i2c_write_byte(data);
  
  os_i2c_stop();
  
  /*
   Wire.beginTransmission(EEPROM_I2C_ADDRESS);
   Wire.write((int)(address >> 8));   // MSB
   Wire.write((int)(address & 0xFF)); // LSB
   
   
   Wire.write(val);
   Wire.endTransmission();

   delay(5);
  
  
  Wire.beginTransmission(address);
  Wire.send(from_addr);
  Wire.send(data);
  Wire.endTransmission();*/
}

static void eeprom_i2c_read(u8 address, int from_addr,u8 *dst,u8 len) {
  eeprom_connect(address,from_addr);
/*  //i2c restart
  os_i2c_sda_high();
  os_i2c_scl_high();
  asm volatile("nop");
  asm volatile("nop");
  os_i2c_sda_low();
  os_i2c_scl_low();
*/
  
  
   //*/
  os_i2c_stop();
  os_i2c_start();
  os_i2c_write_byte(address|1);
  
  u8 i;
  for(i=len;i;i--)
    *dst=os_i2c_read_byte(1),dst++;

  os_i2c_scl_low();  // scl must be lowered after read, read lowers scl at its start, this saves cycles because the ack can be "processed" (=ignored) by processing the outer for loop.
  os_i2c_stop();
  
  /*
  Wire.beginTransmission(address);
  Wire.send(from_addr);
  Wire.endTransmission();

  Wire.requestFrom(address, 1);
  if(Wire.available())
  return Wire.receive();
  else
  return 0xFF;*/
}


/*
FUSES =
{
  .low = LFUSE_DEFAULT,
  .high = HFUSE_DEFAULT,//(FUSE_BOOTSZ0 & FUSE_BOOTSZ1 & FUSE_EESAVE & FUSE_SPIEN ),
  .extended = FUSE_SELFPRGEN,
};
*/
const u8 os_conv_hex[] __attribute__((progmem)) = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

void ApiIntToHex(u16 in, u8 *out)
{
  out[0] =pgm_read_byte(&os_conv_hex[(in >> 12) & 0xf]);
  out[1] =pgm_read_byte(&os_conv_hex[(in >> 8) & 0xf]);
  out[2] =pgm_read_byte(&os_conv_hex[(in >> 4) & 0xf]);
  out[3] =pgm_read_byte(&os_conv_hex[in & 0xf]);
}

u8 ApiCharToFontIndex(u8 c)
{
  #ifdef FONT_ASCII
    return c-0x20;
  #else
  if (c >= 'a' && c <= 'z')return 11 + c - 'a';
  if (c >= 'A' && c <= 'Z')return 11 + c - 'A';
  if (c >= '0' && c <= '9')return 1 + c - '0';
  return 0;
  #endif
}


// convert a C-String to Font Indices, can be used to use sprintf directly in the console buffer
// example:
// sprintf(myconsole,"hello world");
// GfxApiConvertStringToFontIdx(myconsole);
void GfxApiConvertStringToFontIdx(u8 *s)
{
  while(*s)
  {
    *s=ApiCharToFontIndex(*s);
    s++;
  }
}
#define SystemServer_WriteToScreen GfxApiWriteToConsole
// for compatibility until all demos are fixed
extern void GfxApiWriteToConsole(const char *txt, u8 *screen, u8 x, u8 y)
{
  int p = x + 16 * y;
  int l = strlen(txt);
  int i;
  for (i = 0; i < l; i++)screen[(i + p) & 0x7f] = ApiCharToFontIndex(txt[i]);
}
  #ifdef ENABLE_LINEDRAWING
void _reorder_lines()
{
  u8 i;
  for(i=0;i<_gfx_linepos;i+=4)
  {
    if(_gfx_points_of_lines[i]>_gfx_points_of_lines[i+2])
    {
      swapu8(_gfx_points_of_lines[i],_gfx_points_of_lines[i+2]);
      swapu8(_gfx_points_of_lines[i+1],_gfx_points_of_lines[i+3]);
    }
  }
}
  #endif

  #pragma GCC push_options
  #ifndef DISABLE_OFAST
    #pragma GCC optimize ("Ofast")
  #endif


  #pragma GCC pop_options

static void os_gfx_start_display_transfer()
{
  #ifndef ENABLE_WIRE
  os_i2c_start();
  os_i2c_write_byte(SSD1306_ADDRESS);
  os_i2c_write_byte(0x0);
//  os_i2c_write_byte(0xb0);  // ymove   //missing xmove 0,0
  os_i2c_write_byte(0xAF);  // Display On
  os_i2c_write_byte(0x21);  // define x range 
  os_i2c_write_byte(0x0);
  os_i2c_write_byte(0x7f);
  os_i2c_write_byte(0xb0|7);
  os_i2c_write_byte(127&0xf);
  
  os_i2c_write_byte(((127>>4)&0xf)|0x10);

  os_i2c_stop();
  os_i2c_start();
  os_i2c_write_byte(SSD1306_ADDRESS);
  os_i2c_write_byte(0x40);
#else
  Wire.beginTransmission(WIRE_SCREEN_ADDRESS);
  Wire.write(0x0);
  //Wire.write(0xb0);
  Wire.write(0x21);
  Wire.write(0x0);
  Wire.write(0x7f);
  Wire.endTransmission();
  Wire.beginTransmission(WIRE_SCREEN_ADDRESS);
  Wire.write(0x40);
#endif
}
void os_restart_display_transfer()
{
    os_i2c_start();
  os_i2c_write_byte(SSD1306_ADDRESS);
  os_i2c_write_byte(0x40);
}

void os_init_ssd1306 (void)
{
  // brigher screen init:
#ifndef ENABLE_DARKER_SCREEN  
  const u8 init1306[]={
   0,0xe4,0xAE,0xD5, 0x80, 0xA8, 0x3F,0xD3, 0x0,0x40,0x8D, 0x14, 0x20, 0x01, 0xA1, 0xC8, 0xDA, 0x12, 0x81, 0x7F, 0xD9, 0xF1, 0xDB, 0x40,  0xA4,0xA6,0xAF
  };
#else
  // darker screen init:
  const u8 init1306[] = {
    0x0,0x20, 1 /*vertical mode*/,0xB0,0xC8,0x00,0x10,0x40,0x81,0x0,0xA1,0xA6,0xA8,0x3F,0xA4,0xD3,0x00,0xD5,0xF0,0xD9,0x22,0xDA,0x12,0xDB,0x20,0x8D, 0x14,0xAF
  };
#endif  
  os_i2c_write(init1306, sizeof(init1306));
}

void LoadTemporalSettings()
{

  //0xd9, 0x00, 
  
}

// set the brightness of the screen 0 to 15
void GfxApiSetBrightness (u8 brightness)
{
  brightness=brightness|(brightness<<4);
  u8 v1=brightness>>5;
  v1<<=4;
  
  u8 v2=brightness>>4;
  v2+=1;
  if(v2>15)v2=15; // better temporal dither?
  v2|=v2<<4;
  const u8 init1306[]={
    0,
    0xd9, v2,0xdb,v1,0x81,brightness
  };
  os_i2c_write(init1306, sizeof(init1306));
}
void GfxApiConsoleSetAttribute(u8 hb)
{
 _engine_flags_and_frame_counter&=0x7f;
 if(hb)_engine_flags_and_frame_counter|=0x80;
}
// set the brightness of the screen 0 to 15
void GfxApiSetFreq (u8 freq)
{ 
  
  u16 a=(1+freq/2)<<4;
  a|=15-freq;
  a=freq;
  a=0xf0;
  const u8 init1306[]={
    0,
    0xd5,freq//,0xd9,0x00,0xdb,0xff,0xd9,0,0x81,160
  };
  os_i2c_write(init1306, sizeof(init1306));
}




void GfxApiSetFreqTemporal (int q)
{ 
 // GfxApiSetBrightness(2);
  const u8 init1306[]={
0,
    //0xd7,q&4?128:0
    //,0xa7
//    0,0xd0,0,0xd4,50,q,100,
    
  //  0xa6// 0xa6,   /*0xd5,0xF0,*/0xd9,32,/*0xdb,0x00,0xd9,0,*/0x81,255,0xd0,0,0xa7
  };
  os_i2c_write(init1306, sizeof(init1306));
}


void GfxApiSetDisplayEnable (u8 e)
{
  const u8 init1306[]={
    0,
    0x0ae +(e?1:0)
  };
  os_i2c_write(init1306, sizeof(init1306));
}
void GfxApiFlipY(u8 flip_y)
{
  const u8 init1306[]={
    0,
    flip_y? 0xc0:0xc8
  };
  os_i2c_write(init1306, sizeof(init1306));
}
void GfxApiSetInvert (u8 invert)
{
  const u8 init1306[]={
    0,
    0xa6+(invert?1:0)
  };
  os_i2c_write(init1306, sizeof(init1306));
}
void GfxApiSetStartline (u8 startline)
{
  const u8 init1306[]={
    0,
    0x40+(startline&63)
  };
  os_i2c_write(init1306, sizeof(init1306));
}


void GfxApiSetDisplayMux (u8 e)
{
  e=64;
  const u8 init1306[]={
    0,
    0x0a8,  e
  };
  os_i2c_write(init1306, sizeof(init1306));
}


void GfxApiSetDisplayOff ()
{
  const u8 init1306[]={
    0,
    0x0ae
  };
  os_i2c_write(init1306, sizeof(init1306));
}


void TestCmd(u8 a)
{ 
  //return;
  u8 b=a&1;
  const u8 init1306[]={
        0,0xd6,b?1:0,0x9a,b?2:0
  };
  os_i2c_write(init1306, sizeof(init1306));
//  GfxApiSetStartline(16);
}


void TestCmdx(u8 a)
{ 
  //return;
  const u8 init1306[]={
    
    0,0xae + a
  };
  os_i2c_write(init1306, sizeof(init1306));
}



  #ifdef CONSOLE_ENABLE_CURSOR
static u8 cursor_pos=0;
    #ifdef CONSOLE_ENABLE_CURSOR_BLINK
      #ifndef CONSOLE_CURSOR_BLINK_MASK
        #define CONSOLE_CURSOR_BLINK_MASK 1
      #endif
    #endif
  #endif


  #ifdef CONSOLE_ENABLE_PRINTF
static u8 * _printf_console;
    #ifndef CONSOLE_ENABLE_CURSOR
static u8 cursor_pos=0;
    #endif

static void GfxApiAttachConsole(u8*console)
{
  _printf_console=console;
}

int console_putc(char c, FILE *file) {
 // console[1]++;//=10;
  if(c!='\n')
  {
    _printf_console[cursor_pos]=ApiCharToFontIndex(c)|(_engine_flags_and_frame_counter&0x80);
    cursor_pos++;
  }
  else cursor_pos=(cursor_pos&0xf0)+0x10;

  if(cursor_pos>=128)
  {
    u8 i;
    for(i=16;i<128;i++)_printf_console[i-16]=_printf_console[i];
    for(i=128-16;i<128;i++)_printf_console[i]=0;
    cursor_pos-=16;
  }
  return c;
}
  #endif



  #if defined(CONSOLE_ENABLE_CURSOR) || defined(CONSOLE_ENABLE_PRINTF)
void GfxApiGotoXY(u8 x,u8 y)
{
  cursor_pos=(x&15)+(y<<4);
}


  #endif

  #ifndef ENABLE_ARDUINO_SUPPORT
extern void MainTask();
int main()
{
  cli();
/*
  I2C_DDR=0;
//  I2C_PORT=255;  // input + pullup
//only attiny85 PRR=0b1100;   // disable timer, adc
  MCUCR|=0x80; // disable brownout
  ACSR|=0x80;
  */
  os_i2c_init();
  os_init_ssd1306();
    #ifdef ENABLE_ATTINY_POWER_MANAGER
  MCUSR=0;
  wdt_enable(WDTO_120MS);
  WDTCR|=1<<WDIE;             
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
    #endif
  sei();
  
    #ifdef CONSOLE_ENABLE_PRINTF
  fdevopen(&console_putc, 0);
    #endif

  MainTask();
}
    #else
int streamgfx_init_arduino()
{
  os_i2c_init();
  os_init_ssd1306();
    #ifdef CONSOLE_ENABLE_PRINTF
   fdevopen(&console_putc, 0);
    #endif
}
  #endif

  #ifdef ENABLE_ATTINY_POWER_MANAGER
ISR(WDT_vect)
{
  WDTCR |= 1<<WDIE;
}
u16 bat_volt=5200;
u8 low_power_screen_disable=0;
s8 _br=15;
    #define bit_is_set(sfr, bit) (_SFR_BYTE(sfr) & _BV(bit))

int _manage_battery() {
  static u8 phase=0;
  if(phase==1)
  {
    PRR&=~1; // enable adc....
    phase++;
    return bat_volt;
  }
  if(phase==2)
  {
    ADMUX =
    (0 << REFS1) | // Sets ref. voltage to VCC, bit 1
    (0 << REFS0) | // Sets ref. voltage to VCC, bit 0
    (1 << MUX3)  | // use Vbg as input, MUX bit 3
    (1 << MUX2)  | // use Vbg as input, MUX bit 2
    (0 << MUX1)  | // use Vbg as input, MUX bit 1
    (0 << MUX0);   // use Vbg as input, MUX bit 0
    ADCSRA =
    (1 << ADEN)  | // enable ADC
    (1 << ADPS2) | // set prescaler to 64, bit 2
    (1 << ADPS1) | // set prescaler to 64, bit 1
    (0 << ADPS0);  // set prescaler to 64, bit 0
    phase++;
    return bat_volt;
  }
  if(phase==3)
  {
    ADCSRA &= ~(1<<ADEN);
    ACSR |= (1<<ACD);
    ADCSRA |= (1<<ADEN);
    phase++;
    return bat_volt;
  }
  if(phase==4)
  {
    ADCSRA |= (1 << ADSC); // start ADC measurement
    while ( ADCSRA & (1 << ADSC) ); // wait till conversion complete
    uint16_t adc=ADC;
    // clear the ADIF bit by writing 1 to it
    ADCSRA|=(1<<ADIF);
    // disable the ADC
    ADCSRA &= ~(1<<ADEN);
    phase++;
    bat_volt=1000.0f*1024.0f*1.1f/(float)adc;
    return bat_volt;
  }
  PRR|=1;
  if(bat_volt<2000)
  {
    if(!low_power_screen_disable)
    {
      GfxApiSetDisplayEnable(0);

      const u8 disable_charge_pump[]={
        0,
        0x8d,
        0x10
      };
      os_i2c_write(disable_charge_pump, sizeof(disable_charge_pump));
      
      clock_prescale_set(clock_div_256);
    }
    low_power_screen_disable=10;
  }
  else
  {
    if(bat_volt<POWER_MANAGER_LOW_POWER)
    {
      
      if(bat_volt<POWER_MANAGER_LOW_POWER-50)
      _br-=1;
      if(_br<0)_br=0; 
    //  sleep_cpu();
    //  os_i2c_init();  // bring usi back....
    }
    else
    { 
      if(bat_volt>POWER_MANAGER_LOW_POWER+50)
      {
        _br++;
        if(_br>15)_br=15;
      }
    }
    _br=15;
    GfxApiSetBrightness(_br);

    if(low_power_screen_disable)
    {
      sleep_cpu();
      if(bat_volt>3300)
      {
        sleep_cpu();
        low_power_screen_disable--;
        if(!low_power_screen_disable)
        {
          const u8 reset[]={
            0,
            0xe4 // undocumented command, said to help startup if display is "confused"
          };
          clock_prescale_set(clock_div_1);

          _delay_ms(500);
          for (int i=0;i<18;i++)
            os_i2c_stop();
        
          os_init_ssd1306();
        }
      }
    }
  }
  phase++;
  if(phase==10)phase=0;
}
  #endif
#endif  //api functions,fonts, power manager

#ifdef DISPLAYFUNC
/*
 * todo: optimierung aufbau an pixel x=128 y=row 7 beginnen, i2c_dataout statisch machen, clock_i2c abschaffen... kürzerer code....
 * layer in if moven, layersum "statisch für den if" machen, layersum nur berechnen wenn auch nötig
 * 
 * 
 */



#include <math.h>
#include <stdlib.h>
//#define ENABLE_TRIANGLES
/* CPKI AttinyStreamGfxApi & TinyMultiOs, Version 0.9.3b1
see
https://www.youtube.com/watch?v=WNJQXsJqSbM
Copyright (c) 2022
Görg Pflug & CPKI Gmbh, www.cpki.de . All rights reserved.
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
All advertising materials mentioning features or use of this software must display the following acknowledgement: “This product includes software developed by the CPKI Gmbh, Görg Pflug and its contributors.”
Neither the name of the Cpki GmbH nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY Görg Pflug, CPKI Gmbh AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
Version:
0.8.1 faster
0.8   Filled Triangle drawing
0.5   faster, support functions, Consistent API, engine-interleaved I2C, propper line-drawing-api
    much better 3D-Performance
0.4   Prelimary Support for Arduino-Wire, performance worse than on attiny85
0.3   Compatible with C++, compiles in Microchip Studio and Arduino, most samples still untested
    added .INO example for Arduino
0.2   Multiple Consoles
0.2.1 ASCII Font Support
    define: ENABLE_FONT_BASIC
    "Low Quality" Halftoning, saves 64 bytes, faster, for some use cases "low quality" halftoning might even look better
    define: ENABLE_LOW_QUALITY_HALFTONE
    Halftoning gets disabled Automatically when not using layers or pixel callback, faster, saves 64 byte
    
*/

#ifndef ENABLE_WIRE
#define INTERLEAVE
#endif

#pragma GCC push_options
#ifndef DISABLE_OFAST
#pragma GCC optimize ("Ofast")
#endif
#ifndef ENABLE_LAYERS
#ifndef PIXEL_CALLBACK
#define DISABLE_HALFTONE
#endif
#endif

#ifndef GfxApiLayerGetNextByteInlineHack
#define GfxApiLayerGetNextByteInlineHack
static const u8 _bit_set[] PROGMEM={1,2,4,8,16,32,64,128};

static const u8 _ordered_dither_matrix_simulated_annealing[] PROGMEM=
{
        // hacked dither pattern, does not contain 0 and 63, optimized by hillclimbing
55,11,21,52,14,24,61,18,        
48,19,5,45,27,15,43,7,        
13,59,30,9,56,33,22,62,        
26,41,1,36,47,3,51,38,        
28,39,53,17,40,60,1,35,        
57,2,23,32,12,49,25,10,        
16,50,62,6,37,20,54,31,        
4,34,44,29,58,42,8,46,
  /*
13,48,19,56,6,22,62,41,        
30,24,43,0,36,51,10,31,        
2,53,11,58,15,27,44,17,        
40,42,21,38,47,8,34,54,        
14,63,49,4,23,61,20,5,        
45,35,9,59,28,1,50,32,        
18,52,60,16,55,12,25,57,        
29,7,39,26,33,46,37,3,

/*
// this seems to be the best matrix for vector drawing
52,10,59,2,36,9,34,3,
20,38,23,49,19,62,22,48,
61,5,54,11,43,4,40,12,
25,42,17,46,27,57,18,45,
53,1,58,32,15,33,50,8,
21,31,39,7,63,0,30,37,
6,56,13,47,16,44,14,60,
41,24,35,29,55,26,51,28,
*/
};  // ordered dither matrix generated by simulated annealing


// gcc is too stupid to inline this function..... because it is compiled on O3...
// so split.... but we want to define it only once, because the display function can potentially be included multiple times
// one should check this for the sprites too, the compiler pushed loads of variables.
static inline void  __attribute__((always_inline)) GfxApiLayerGetNextByte (GfxApiCompressedLayer *l)
{

  if (l->SkipCounter)
  {
    l->SkipCounter--;
    return;// l->PixelValue;
  }
  u8 r = pgm_read_byte(l->BytePos);
  l->BytePos++;
  u8 color = (r >> 5);
  l->PixelValue = (color<<3)|color;
  u8 rept = r & 31;
  l->SkipCounter = rept;
  if (rept != 31)return;// l->PixelValue;
  for (;;)
  {
    u8 add = pgm_read_byte(l->BytePos);
    l->BytePos++;
    l->SkipCounter += add;
    if (add != 255)return;// color;
  }
}
#ifdef ENABLE_LINEDRAWING

static unsigned char _need_clear=1;
// Bresenham
static inline void _line(u8 *_x0, u8 *_y0, u8 x1, u8 y1,unsigned char *linebuffer) {
  if(x1<_cur_seg) return ; // rechts ist kleiner als das akute segment....raus...die linie
  u8 x0=*_x0;
  const u8 curseg_plus_7=_cur_seg+7;
  if(x0>curseg_plus_7) return ; // links grösser als cur seg... raus...
  
  _need_clear=1;

  // kann das segment nicht schneiden
  const s8 dx =  x1-x0;   // x1 is allways >= x0, no abs needed
  u8 y0=*_y0;
  s8 dy = y0-y1;
  s8 sy;

  if(y0<y1)
  {
    sy=1;
  }
  else 
  {
    dy=-dy;
    sy=-1;
  }
  
  s8 err = dx+dy;
  s8 e2;   // s8 ist hier potentiell ein bug wegen überlauf......
  
  const u8 dasboot=96;
  y0-=dasboot;
  y1-=dasboot;
  
  for(;;)
  {  
    if(((x0&0xf8)==_cur_seg)&&((y0&((u8)~63))==0))   //return;//y-clipping
    {
      const u8 linep=(u8)y0/(u8)8+(x0&(u8)7)*(u8)8;
      linebuffer[linep]|=pgm_read_byte(&_bit_set[y0&7]);//bit;
    }
  
    if ((x0==x1) && (y0==y1))
    {
      return;
    }
-    e2 = 2*err;
    if (e2 <= dx) 
    { 
      err += dx; 
      y0 += sy; 
    } 
    if (e2 >= dy) 
    { 
      err += dy; 
      x0++;
    
      if(x0>curseg_plus_7)
      {
        *_x0=x0;
        *_y0=y0+96;
  
        return;
      }
    }
  } 

}

static void _vline(int y0, int x0, int y1, u8 c, u8 *linebuffer) {
  if ((x0&0xf8)!=_cur_seg)return;
  y0-=96;
  if(y0<0)y0=0;
  if(y0>63)y0=63;

  y1-=96;
  if(y1<0)y1=0;
  if(y1>63)y1=63;

  if(y0>y1)swapu8(y0,y1);
  _need_clear=1;
  x0&=7;
  u8 xx0=x0;
  x0*=8;

  u8 bit=1<<(y0&7);

  u8 y=y0;
  const u8 ye=y1;
  u8 linep=y0/8;
  linep+=x0;

  u16 addr=(u16)&_ordered_dither_matrix_simulated_annealing[xx0*8];

#ifndef ENABLE_VECTOR_ERRORDIFFUSION
  linebuffer+=linep;

  u8 cache=*linebuffer;

  for(;;)
  {
    cache&=~bit;
    if(pgm_read_byte(addr+(y&7))<=c)
    cache|=bit;
    bit<<=1;
    if(!bit)
    {
      *linebuffer=cache;
      bit=1,linebuffer++;
      cache=*linebuffer;
    }
    y++;
    if(y>ye)
    {
      *linebuffer=cache;
      return;
    }
  }
#else

    s8 error;
    s8 *e_left=&linebuffer[64+y];
    linebuffer+=linep;
    u8 cache =*linebuffer;
    
    cache&=~bit;
    if(pgm_read_byte(addr+(y&7))<=c)
    {
      error=c-63;
      cache|=bit;
    } else error=c;
    error>>=1;
    *e_left=error;
    
    y++;
    if(y>ye)
    {
      *linebuffer=cache;
      return;
    }
    bit<<=1;
    if(!bit){
      *linebuffer=cache;
      bit=1,linebuffer++;
      cache=*linebuffer;
    }
    
    e_left++;
  
  for(;;)
  {
    //*linebuffer&=~bit;
    cache&=~bit;
    
    
    error+=c+*e_left;
    if(error>31)
    {
      error-=63;
      //*linebuffer|=bit;
      cache|=bit;
    }
    error>>=1;
    *e_left=error;

    y++;
    if(y>ye)
    {
      *linebuffer=cache;
      return;
    }

    bit<<=1;
    if(!bit){
      *linebuffer=cache;
      bit=1,linebuffer++;
      cache=*linebuffer;
    }
    e_left++;
  } //*/
#endif
}


#ifdef ENABLE_CIRCLES
static inline void drawCircle_y(u8 yc, u8 xc, u8 y, u8 x,u8 *linebuffer,u8 pat)
{
  
  //  _vline(xc-x,yc+y,xc+x,pat,linebuffer);
//    _vline(xc-x,yc-y,xc+x,pat,linebuffer);
    _vline(xc+y,yc+x,xc-y,pat,linebuffer);
    _vline(xc+y,yc-x,xc-y,pat,linebuffer);
  
}
static inline void drawCircle_x(u8 yc, u8 xc, u8 y, u8 x,u8 *linebuffer,u8 pat)
{
  
    _vline(xc-x,yc+y,xc+x,pat,linebuffer);
    _vline(xc-x,yc-y,xc+x,pat,linebuffer);
  //  _vline(xc+y,yc+x,xc-y,pat,linebuffer);
  //  _vline(xc+y,yc-x,xc-y,pat,linebuffer);
  
}

static inline void fillCircle(u8 xc,u8 yc,u8 r,u8 pat,u8*linebuffer)
{
  if(((xc-r)&0xf8)>_cur_seg+7) return ; // links grösser als cur seg... raus...
  if(((xc+r)&0xf8)<_cur_seg) return ; // rechts ist kleiner als das akute segment....raus...die linie
// kann das segment nicht schneiden
  _need_clear=1;
  s8 x = 0, y = r;
  s8  d = 3 - 2 * r;
  drawCircle_x(xc, yc, x, y,linebuffer,pat);
  drawCircle_y(xc, yc, x, y,linebuffer,pat);

  while (y >= x)
  {
    x++;
    drawCircle_x(xc, yc, x, y,linebuffer,pat);

    if (d > 0)
    {
      y--;
// hier die y abhängigen vlines zeichnen, nicht mit draw circle...

      d = d + 4 * (x - y) + 10;
    }
    else
    d = d + 4 * x + 6;
  drawCircle_y(xc, yc, x, y,linebuffer,pat);
 // drawCircle2(xc, yc, x, y,linebuffer,pat);
  }
}
#endif

#ifdef ENABLE_TRIANGLES
// triangle code was once based on https://www.avrfreaks.net/sites/default/files/triangles.c , https://www.avrfreaks.net/forum/algorithm-draw-filled-triangle

static void fillTriangle(u8 x1,u8 y1,u8 x2,u8 y2,u8 x3,u8 y3, u8 c,u8*linebuffer) {
  u8 t1x,t2x,y,minx,maxx,t1xp,t2xp;
  u8 change=0;
  const u8 terminate_line=_cur_seg+7;

//note: x and y are swapped to get better organization of the line


  minx=y1;
  if(y2<minx)minx=y2;
  if(y3<minx)minx=y3;
  maxx=y1;
  if(y2>maxx)maxx=y2;
  if(y3>maxx)maxx=y3;
  if((minx&0xf8)>_cur_seg+7) return ; // links grösser als cur seg... raus...
  if((maxx&0xf8)<_cur_seg) return ; // rechts ist kleiner als das akute segment....raus...die linie


  s8 signx1,signx2,dx1,dy1,dx2,dy2;
  u8 e1,e2;
  // Sort vertices
  if (y1>y2) 
  { 
    swapu8(y1,y2); 
    swapu8(x1,x2); 
  }
  if (y1>y3) 
  { 
    swapu8(y1,y3); 
    swapu8(x1,x3); 
  }
  if (y2>y3) 
  { 
    swapu8(y2,y3); 
    swapu8(x2,x3); 
  }

  t1x=x1;
  t2x=x1; 
  
  y=y1;   // Starting points

  dx1 = (s8)(x2 - x1); 
  if(dx1<0) 
  { 
    dx1=-dx1; 
    signx1=-1; 
  } 
  else 
    signx1=1;
  dy1 = (s8)(y2 - y1);
  
  dx2 = (s8)(x3 - x1); 
  if(dx2<0) 
  { 
    dx2=-dx2; 
    signx2=-1; 
  } 
  else signx2=1;
  
  dy2 = (s8)(y3 - y1);
  
  if (dy1 > dx1) 
  {   // swap values
    swapu8(dx1,dy1);
    change|=1;// = true;
  }
  if (dy2 > dx2) 
  {   // swap values
    swapu8(dy2,dx2);
    change|=2;// = true;
  }
  
  e2 = (u8)(dx2>>1);
  // Flat top, just process the second half
  if(y1==y2) goto next;
  
  e1 = (u8)(dx1>>1);
  
  for (u8 i = 0; i < dx1;) 
  {
    t1xp=0; t2xp=0;
    if(t1x<t2x) 
    { 
      minx=t1x; 
      maxx=t2x; 
    }
    else    
    { 
      minx=t2x; 
      maxx=t1x; 
    }
    // process first line until y value is about to change
    while(i<dx1) 
    {
      i++;
      e1 += dy1;
      while (e1 >= dx1) 
      {
        e1 -= dx1;
        if (change&1) 
          t1xp=signx1;
        else          
          goto next1;
      }
      if (change&1) break;
      
      t1x += signx1;
    }
    // Move line
    next1:
    // process second line until y value is about to change
    while (1) 
    {
      e2 += dy2;
      while (e2 >= dx2) {
        e2 -= dx2;
        if (change&2) 
          t2xp=signx2;
        else          
          goto next2;
      }
      if (change&2)
        break;
      t2x += signx2;
    }
    next2:

    if(minx>t1x) minx=t1x; 
    if(minx>t2x) minx=t2x;
    if(maxx<t1x) maxx=t1x; 
    if(maxx<t2x) maxx=t2x;
    //*/
    _vline(minx,y, maxx, c,linebuffer);    // Draw line from min to max points found on the y
    if(y==terminate_line)return;
    // Now increase y
    if(!(change&1)) t1x += signx1;
    t1x+=t1xp;
    if(!(change&2)) t2x += signx2;
    t2x+=t2xp;
    y += 1;
    if(y==y2) break;
    
  }
  next:
  // Second half
  dx1 = (s8)(x3 - x2); 
  if(dx1<0) 
  { 
    dx1=-dx1; 
    signx1=-1; 
  } 
  else 
    signx1=1;
  dy1 = (s8)(y3 - y2);
  t1x=x2;
  
  if (dy1 > dx1) 
  {   // swap values
    swapu8(dy1,dx1);
    change|=1;// = true;
  } 
  else 
    change&=2;//false;
  
  e1 = (u8)(dx1>>1);
  
  for (u8 i = 0; i<=dx1; i++) 
  {
    t1xp=0; t2xp=0;
    if(t1x<t2x) 
    { 
      minx=t1x; 
      maxx=t2x; 
    }
    else    
    { 
      minx=t2x; 
      maxx=t1x; 
    }
    // process first line until y value is about to change
    while(i<dx1) {
      e1 += dy1;
      while (e1 >= dx1) 
      {
        e1 -= dx1;
        if (change&1) 
        { 
          t1xp=signx1; 
          break; 
        }
        else          
          goto next3;
      }
      if (change&1) 
        break;
      
      t1x += signx1;
      if(i<dx1) i++;
    }
    next3:
    // process second line until y value is about to change
    while (t2x!=x3) 
    {
      e2 += dy2;
      while (e2 >= dx2) 
      {
        e2 -= dx2;
        if(change&2) 
          t2xp=signx2;
        else          
          goto next4;
      }
      if (change&2)     
        break;
      
      t2x += signx2;
    }
    next4:

    if(minx>t1x) minx=t1x; 
    if(minx>t2x) minx=t2x;
    if(maxx<t1x) maxx=t1x; 
    if(maxx<t2x) maxx=t2x;
//  */
    _vline(minx,y, maxx, c,linebuffer);    // Draw line from min to max points found on the y
    if(y==terminate_line)return;
    // Now increase y
    if(!(change&1)) t1x += signx1;
    t1x+=t1xp;
    if(!(change&2)) t2x += signx2;
    t2x+=t2xp;
    y += 1;
    if(y>y3) return;
  }
}
#endif
#endif
#endif


static void DISPLAYFUNC (
  GfxApiCompressedLayer * layers, 
  u8 fade, 
  GfxApiSprite * Sprites, 
  u8 * screen
#ifdef ENABLE_CONSOLE0_FONT_SWITCHING
,int font_adress_console_0_in
#endif
#ifdef ENABLE_SECOND_CONSOLE
  ,u8 * screen2, 
  s16 zoomX,
  s16 zoomY,
  s16 scrollX,
  s16 scrollY
#endif
#ifdef ENABLE_SECOND_CONSOLE_AFFINE
  ,s16 zoomX2,
  s16 zoomY2
#endif
#ifdef ENABLE_BARGRAPH
  ,GfxApiBargraph *bargraph
#endif
#ifdef ENABLE_VSYNC
  ,u8 DisplaySingleFrame
#endif

  )
{
#define TEMPORAL_TEST
#ifdef TEMPORAL_TEST
  static u8 temp_cnt;
  temp_cnt+=TEMP_E;
  if(temp_cnt==TEMP_E*3)temp_cnt=0;
#endif




#ifdef ENABLE_VFD_GRID

  u16 stream_addr=&iv17_pattern[0];
#endif
  
#ifdef ENABLE_BARGRAPH
  s8 bar_strength[8]={4*31,4*31,4*31,4*31,4*31,4*31,4*31,4*31};
#endif
  u8 pattern=0;
  #ifdef CONSOLE_ENABLE_CHESSBOARD_DITHER
  +0x55
  #endif
  ;

  u8 pattern_55=0x55;
  if (_engine_flags_and_frame_counter&1)pattern=~pattern,pattern_55=~pattern_55;

#ifdef ENABLE_ATTINY_POWER_MANAGER
  if(low_power_screen_disable)
  {
    _manage_battery();
    return;
  }
#endif
#ifdef INTERLEAVE
static  u8 _i2c_data_out;
        u8 i2c_data_out=_i2c_data_out;
        //u8 must_float_pin=i2c_data_out&0x80;
#endif
#ifdef ENABLE_WIRE
  u8 wire_cnt=0;
#else
  u8 clock_i2c=0;    
#endif
#ifdef ENABLE_LINEDRAWING
  u8 linebuffer[64
#ifdef ENABLE_VECTOR_ERRORDIFFUSION
  +64
#endif  
  ];
  u8 linebuffer_pos=0;
  memset(linebuffer,0,sizeof(linebuffer));
  reorder_lines();
#endif
#ifdef ENABLE_LAYERS
  u16 layers_constant_counter=0;
#endif
#ifdef ENABLE_SPRITES
  u16 sprites_constant_counter=0;
#endif
  u8 x_pos_screen; 
  os_gfx_start_display_transfer();

#ifndef DISABLE_HALFTONE
#ifndef ENABLE_LOW_QUALITY_HALFTONE
#ifndef ENABLE_ORDERED_DITHERING
  s8 error_right[64];
  memset(error_right,0,sizeof(error_right));
#endif
#endif
  s8 layersum=0;
#endif

  u8 jitter=_engine_flags_and_frame_counter&0x3;

#ifdef ENABLE_SECOND_CONSOLE
  s16 second_console_x=scrollX;
#endif
#ifdef ENABLE_SECOND_CONSOLE_AFFINE
  s16 second_console_y0=scrollY;
#endif

#ifdef ENABLE_BARGRAPH
    s8 falloff[8];
    static u8 b_cnt;
    b_cnt++;
    for (char i=0;i<8;i++)falloff[i]=bargraph->bargraph_falloff[i];
#endif

#ifdef TEMPORAL_TEST
   u8 temporal_x=0;
#endif

  for (x_pos_screen = 0; x_pos_screen < 128; x_pos_screen++
#ifdef ENABLE_SECOND_CONSOLE
  ,second_console_x+=zoomX
#endif
#ifdef ENABLE_SECOND_CONSOLE_AFFINE
  ,second_console_y0+=zoomY2
#endif
  )
  {
    
#ifdef TEMPORAL_TEST
  temporal_x++;
  if(temporal_x==TEMP_E)temporal_x=0;
  u8 pattern_t3=pgm_read_byte(&dither_temp3[temporal_x+temp_cnt]);
  
  //if(temp_cnt==0)pattern_t3=255;
#endif


#ifdef ENABLE_BARGRAPH
    u8 bar_elem=pgm_read_byte(x_pos_screen+&DISPLAYFUNC)&(7)?0:4;
    for (char i=0;i<8;i++) 
    {
      if(x_pos_screen>bargraph->bargraph_value[i])
      {
        u8 a=bar_strength[i]/32;
        bar_strength[i]-=falloff[i]+1;
        if(bar_strength[i]<0)bar_strength[i]=0;
        u8 b=bar_strength[i]/32;
        if(a!=b)
        {
          falloff[i]+=bargraph->bargraph_falloff_change[i];
          if(falloff[i]<0)falloff[i]=0;
        }
      }
    }
//    bar_strength=2*(64-abs(64-x_pos_screen))-(127-bar_end);
  //  if(bar_strength<0)bar_strength=0;
#endif
// Mux hack
#ifdef ENABLE_VSYNC
    if(x_pos_screen==110)
    {
      os_i2c_stop();
      GfxApiSetDisplayMux(64);
      os_restart_display_transfer(); 
    }
#endif

#ifdef ENABLE_LINEDRAWING
    

     if((x_pos_screen&7)==0)
     {  // clear linebuffer for 8x64 block and fill with lines
       linebuffer_pos=0;
//       os_i2c_stop();
//u8 rd;
  //  eeprom_i2c_read(0x50 << 1, 0, &rd, 1);
    //   os_i2c_restart_display_transfer();
       
       if(_need_clear)
       {
         _need_clear=0;
         memset(linebuffer, 0,64);
      //   _clear_start=64,_clear_end=0;
       }
       _cur_seg=(x_pos_screen&0xf8)+64;
      
       for(u8 i=0;i<_gfx_linepos;i+=4)
           _line(&_gfx_points_of_lines[i], &_gfx_points_of_lines[i+1], _gfx_points_of_lines[i+2], _gfx_points_of_lines[i+3],linebuffer);
#ifdef ENABLE_TRIANGLES
       for(u8 i=_gfx_linepos;i<_gfx_linepos+_gfx_tripos;i+=8)
       {
         fillTriangle(_gfx_points_of_lines[i], _gfx_points_of_lines[i+1], _gfx_points_of_lines[i+2], _gfx_points_of_lines[i+3],_gfx_points_of_lines[i+4], _gfx_points_of_lines[i+5],_gfx_points_of_lines[i+6],linebuffer);
       }
#endif         
#ifdef ENABLE_CIRCLES

#ifndef ENABLE_TRIANGLES
#define _gfx_tripos 0
#endif
       for(u8 i=_gfx_linepos+_gfx_tripos;i<_gfx_linepos+_gfx_circlepos+_gfx_tripos;i+=4)
       {
          fillCircle(_gfx_points_of_lines[i], _gfx_points_of_lines[i+1], _gfx_points_of_lines[i+2], _gfx_points_of_lines[i+3],linebuffer);
       }
#ifndef ENABLE_TRIANGLES
#undef _gfx_tripos
#endif
#endif
     }
#endif
#if defined(ENABLE_CONSOLE) || defined (CONSOLE_ENABLE_CURSOR) || defined (ENABLE_SEGMENTED)
    u8 charpos = x_pos_screen >> 3;
#endif




#ifdef ENABLE_SECOND_CONSOLE
#ifndef ENABLE_SECOND_CONSOLE_AFFINE
    u8 charpos_second_con = (second_console_x>>11);
  //  if(charpos_second_con>SECOND_CONSOLE_LINE_LENGTH)charpos_second_con=SECOND_CONSOLE_LINE_LENGTH-1;
    s16 second_console_y=scrollY;
#else
    s16 second_console_y=second_console_y0;
    s16 second_console_x_affine=second_console_x;
#endif
#endif
    u8 or_bit=1;
#ifndef ENABLE_ORDERED_DITHERING    
    s8 propagte_error=0;
#endif
    u8 block_8_px=0;
    u8 y_pos_screen;
#ifdef ENABLE_ORDERED_DITHERING
    u8 ordered_dither_y=0;
    u8 ordered_dither_x=(x_pos_screen&7)<<3;
#endif

    pattern=~pattern;

    
/*
    u8 ormask=pattern&1?0x80:0;
    pattern=pattern>>1;
    pattern|=ormask;
    //*/
    const u8 subpos_x=x_pos_screen&7;






#ifdef ENABLE_MATRIX_VFD
  u8 vfd_bits=matrix_vfd[x_pos_screen/8];
  u8 vfd_low0=0;
  u8 vfd_low1=0;

  u8 vfd_high0=0;
  u8 vfd_high1=0;


  u8 vfd_higher0=0;
  u8 vfd_higher1=0;
  u8 vfd_last=0;
#if 0
  
  if ((subpos_x==0)||(subpos_x==6))
  {
    vfd_higher1=0b011111;
  }else

  if (subpos_x==3)
  {
    vfd_low1=255;
    vfd_high1=0x55;
    vfd_high0=0x55<<1;
    
    vfd_higher0=255;
  }
  else if(subpos_x<6)
  {
    vfd_high0=0b0111110;
    vfd_higher1=0b1000001;
    vfd_higher0=0b0111110;

    
    vfd_low1=0b0111110;    
  }

  u8 vfd_low_h0=vfd_low0;
  u8 vfd_low_h1=vfd_low1;

  if((subpos_x==1)||(subpos_x==5))vfd_low1=0;
#endif
  u8 vfd_low_h0=0;
  u8 vfd_low_h1=255;
  vfd_high0=127;
  vfd_higher0=255;
  


  if (subpos_x!=7)
#endif


#ifdef ENABLE_CONSOLE0_FONT_SWITCHING
    const int font_adress_console_0=subpos_x+font_adress_console_0_in+(int)&os_font[0];
#endif


#if defined (ENABLE_SEGMENTED) || defined (ENABLE_BARGRAPH)
      u8 row=0;
#endif


    for (y_pos_screen = 0; y_pos_screen < 64; y_pos_screen++)
    {
#ifdef INTERLEAVE     
      asm volatile("nop");
    asm volatile("nop");
  
    if(i2c_data_out&0x80)
    {
        os_i2c_sda_high_ppull();
//        or_bit<<=1;
        if(or_bit==128)
        {
          os_i2c_sda_high();
          os_i2c_sda_low_ppull();
          
        }
    }
    else
    {
        os_i2c_sda_low_ppull();
  //      or_bit<<=1;
    }
    i2c_data_out<<=1;
    
    asm volatile("nop");
    asm volatile("nop");
    os_i2c_scl_high_ppull();
    asm volatile("nop");
    asm volatile("nop");
    

#endif
#ifndef DISABLE_HALFTONE
#ifndef PIXEL_CALLBACK
      s8 background_pixel=0;
#else

      s8 background_pixel=0;
#ifdef ENABLE_NEWMODE
      if(!(y_pos_screen&1))
#endif
        background_pixel=PIXEL_CALLBACK (x_pos_screen,y_pos_screen);
#endif
      //layersum=background_pixel;
#endif
#ifdef ENABLE_LAYERS
      if(!layers_constant_counter)
      {
        u8 i;
        for(i=0;i<NR_LAYERS;i++)
        {
          GfxApiLayerGetNextByte(&layers[i]);
        //  Serial.println(tmp);
      //  layers[i].PixelValue = tmp;
        }
//                if(!layers_constant_counter)
        {
          u16 min=layers[0].SkipCounter;
          u8 i;
          for(i=1;i<NR_LAYERS;i++)
          {
            if(layers[i].SkipCounter<min)min=layers[i].SkipCounter;
          }
          if(min)
          {
            layers_constant_counter=min;
            for(i=0;i<NR_LAYERS;i++)
              layers[i].SkipCounter-=layers_constant_counter;
          }
        }
      layersum=0;             
#ifndef LAYERS_COLORKEY
#ifndef SUBTRACT_LAYER0
      layersum+=layers[0].PixelValue>>fade;
#else
      layersum-=layers[0].PixelValue>>fade;
#endif
#endif
      
#ifndef LAYERS_COLORKEY
      for(u8 i=1;i<NR_LAYERS;i++)
        layersum+=layers[i].PixelValue;
#else
      for(u8 i=0;i<NR_LAYERS;i++)if(layers[i].PixelValue)
        layersum=layers[i].PixelValue;
#endif


      }
      else layers_constant_counter--;
      // change to implement different combination logic for the layers
#endif
      
#ifndef DISABLE_HALFTONE      
#ifdef ENABLE_LAYERS

#ifndef LAYERS_COLORKEY
      background_pixel+=layersum;
#else
      if(layersum)background_pixel=layersum;
#endif
#endif
      if(background_pixel>63)background_pixel=63;
#ifdef SUBTRACT_LAYER0
      if(background_pixel<0)background_pixel=0;
#endif

#ifdef ENABLE_NEWMODE
      if(!(y_pos_screen&1))
      {
#endif


  //    if(layersum<0)layersum=0;
  //    if(layersum>63)layersum=63;
#ifndef ENABLE_ORDERED_DITHERING
#ifndef ENABLE_LOW_QUALITY_HALFTONE     
      propagte_error+=background_pixel+error_right[y_pos_screen];
#else
      propagte_error+=background_pixel;
#endif
/*
      if (propagte_error > 31)
      block_8_px |= or_bit, propagte_error-=63;
  */    
        if (propagte_error > 21*2)
      block_8_px |= or_bit |(or_bit<<1), propagte_error-=63;
      else
        if (propagte_error > 21)
      block_8_px |= or_bit, propagte_error-=31;
      
#ifndef ENABLE_LOW_QUALITY_HALFTONE
      propagte_error/=2;
      error_right[y_pos_screen]=propagte_error;
#endif
#endif      
#endif

#ifdef ENABLE_NEWMODE
      }
#endif


#ifdef ENABLE_ORDERED_DITHERING
      if (pgm_read_byte(&_ordered_dither_matrix_simulated_annealing[ordered_dither_y+ordered_dither_x])>layersum)block_8_px |= or_bit;
      ordered_dither_y++;
#endif
      /*
#ifdef ENABLE_SPRITES
      if(!sprites_constant_counter)
      {
        u8 i;
        for(i=0;i<NR_SPRITES;i++)
        if(GfxApiReadSprite(&Sprites[i]))block_8_px|=or_bit;
      }else sprites_constant_counter--;
#endif
      */
#ifdef ENABLE_SECOND_CONSOLE
#ifndef ENABLE_SECOND_CONSOLE_AFFINE
  if((y_pos_screen>=SECOND_CONSOLE_LINE_START)&&((y_pos_screen)<SECOND_CONSOLE_LINE_END))
      {
        u8 the_char = (screen2[charpos_second_con +SECOND_CONSOLE_LINE_LENGTH* (((second_console_y>>8))>>3)]);


//        u8 the_char = (screen2[charpos_second_con +SECOND_CONSOLE_LINE_LENGTH* (((second_console_y>>8)-SECOND_CONSOLE_LINE_START)>>3)]);
        const u8 font_block8=pgm_read_byte(&os_font[((int)the_char << 3) + ( (second_console_x>>8) & 7)]);
        if(font_block8 & (1<<((second_console_y>>8)&7)))//todo pgm read
          block_8_px|=or_bit;
//        if(y_pos_screen&1)  
  //        block_8_px|=or_bit;
          
        second_console_y+=zoomY;
      }
#else
      if((y_pos_screen>=SECOND_CONSOLE_LINE_START)&&((y_pos_screen)<SECOND_CONSOLE_LINE_END))
      {
        u8 charpos_second_con = (second_console_x_affine>>11);
        if(charpos_second_con>SECOND_CONSOLE_LINE_LENGTH)charpos_second_con=SECOND_CONSOLE_LINE_LENGTH-1;
        u8 the_char = (screen2[charpos_second_con +(SECOND_CONSOLE_LINE_LENGTH)* (((second_console_y>>8))>>3)]);
        const u8 font_block8=pgm_read_byte(&os_font[((int)the_char << 3) + ( (second_console_x_affine>>8) & 7)]);
        if(font_block8 & (1<<((second_console_y>>8)&7)))
          block_8_px|=or_bit;
        second_console_y+=zoomY;
        second_console_x_affine+=zoomX2;
      }
#endif
#endif
/*
#ifdef ENABLE_LINEDRAWING
      if(linebuffer[y_pos_screen]&line_cur_bit)block_8_px|=or_bit;;
#endif
*/
      or_bit<<=1;
      if(or_bit==0)
      {
#ifdef ENABLE_LINEDRAWING
      block_8_px|=linebuffer[linebuffer_pos];
      linebuffer_pos++;
#endif

#ifdef INTERLEAVE
   /*     if(must_float_pin)
        {
          os_i2c_sda_high();
          os_i2c_sda_low_ppull();
        }
*/
        os_i2c_scl_low_ppull();
        
        os_i2c_sda_high();
        //os_i2c_sda_low_ppull();
        asm volatile ("nop");   // :-(
#endif
      
#ifdef ENABLE_ORDERED_DITHERING
        ordered_dither_y=0;
#endif
        or_bit=1;
        os_i2c_scl_high_ppull();


#ifdef TEMPORAL_TESTxxxx
  block_8_px|=pgm_read_byte(&dither_temp3[temporal_x+temp_cnt]);
#endif



#ifdef qENABLE_SEGMENTED

    const u8 charpos_mul2=charpos*2;
    
    
    s8 br=segment[pgm_read_byte(&segmented_map[charpos_mul2])];
    
    u8 tile=pgm_read_byte(&segmented_map[charpos_mul2+1]);
    

#if 1

#else
    u8 br=segment[(x_pos_screen>>5)|((row>>1)<<3)];
#endif

    u8 t=pgm_read_byte(&sbox128[charpos^(x_pos_screen&4)]);
    if((t>error_level_vfd))
     {
        br-=t&3;
        t>>=2;
        if(br<0)br=0;
        if(t&1)br>>=1;
     }
    u8 subpos_x2=subpos_x;
#ifdef _SEGMENTED_FLIP_BIT
    if(tile&_SEGMENTED_FLIP_BIT)subpos_x2=7-subpos_x2,tile&=~_SEGMENTED_FLIP_BIT;
#endif
    u8 combined=tile|br;  
    u16 elem=combined*8+subpos_x2;
    
    
#ifdef _TUBE3BIT
    elem*=3;
    const u8 plane0=pgm_read_byte(&segmented_map[256+elem]);
    const u8 plane1=pgm_read_byte(&segmented_map[256+elem+1]);
    const u8 plane2=pgm_read_byte(&segmented_map[256+elem+2]);

    const u8 mask10 = plane0&(~plane1); // nur plane0 gesetzt
    const u8 mask01 = plane1&(~plane0); // nur plane1 gesetzt
    const u8 mask11 = plane1&plane0;

    block_8_px|=mask10      |
      (mask01&pattern)      |
      (mask11&(~pattern_t3))|
      (plane2&(pattern_t3));
#endif
#ifdef _TUBE2BIT
    elem*=2;
    const u8 plane0=pgm_read_byte(&segmented_map[256+elem]);
    const u8 plane1=pgm_read_byte(&segmented_map[256+elem+1]);

    const u8 mask10 = plane0&(~plane1); // nur plane0 gesetzt
    const u8 mask01 = plane1&(~plane0); // nur plane1 gesetzt
    const u8 mask11 = plane1&plane0;

    block_8_px|=mask10       |
      (mask01&pattern)      |
      (mask11&(~pattern_t3));
#endif
#ifdef _TUBE1BIT
    elem*=1;
    const u8 plane0=pgm_read_byte(&segmented_map[256+elem]);
    //block_8_px=0;0b1000001;
#endif


if(0)    
if(row==4)
{
    char cl;
    if((x_pos_screen^pattern_55)&1)
    {
      cl=(((127-x_pos_screen)>>2))>>1;
      cl+=(pgm_read_byte(sbox128[x_pos_screen+jitter])&3)+(pgm_read_byte(sbox128[x_pos_screen])&3);
    }
    else
    {      
      cl=(((x_pos_screen)>>2))>>1;
      cl+=(pgm_read_byte(sbox128[x_pos_screen^jitter])&7)+(pgm_read_byte(sbox128[x_pos_screen^63])&1);
    }
   //(pgm_read_byte(sbox128[x_pos_screen+jitter])&63)
    cl+=cathode_level[row];
    if(cl<0)cl=0;
    else
    if(cl>11)cl=11;
    const u8 sdiv=2;
    char tmp=64/sdiv-abs(64-x_pos_screen)/sdiv;
    //tmp+=
    if(tmp<cl)cl=tmp;
    
    
    block_8_px|=
    (pgm_read_byte(&cathode_image[cl*4])&pattern_55)|
    ((u8)(pattern_t3)&pgm_read_byte(&cathode_image[cl*4+1]));
}
#endif




#ifdef ENABLE_MATRIX_VFDqqqq

  if(vfd_bits&1)
  {
    
  if((vfd_bits&2)&&vfd_last)
  
    block_8_px|=vfd_higher0|(pattern&vfd_higher1);
    else
    block_8_px|=vfd_high0|(pattern&vfd_high1);
    vfd_last=1;
  }
  else          
  {

    
    if((vfd_bits&2)||vfd_last)
 
    block_8_px|=vfd_low_h0|
    (pattern&vfd_low_h1);
    else
    block_8_px|=vfd_low0|
    (pattern&vfd_low1);
    vfd_last=0;
  }
  vfd_bits>>=1;        
  
#endif



#ifdef ttttttENABLE_BARGRAPH
        u8 bargraph_bits=0;
        
        u16 sub_element=bargraph->bargraph_type[row]+bar_elem;
        if(bargraph->bargraph_type[row]<16)
        {

        
          s16 br=4-bar_strength[row]/32;
          if(br<0)br=0;
          //br+=10;
          sub_element+=br*16;  // nach unten.....
          sub_element*=8;



          int addr=&in19_pattern[subpos_x];
          
          int addr2=&in19_pattern[subpos_x+128*8];
          addr+=sub_element;
          addr2+=sub_element;
          u8 bits_0=pgm_read_byte(addr);
          u8 bits_1=pgm_read_byte(addr2);
          block_8_px|=bits_0|(pattern&bits_1);
        }
        
#endif


#if defined (ENABLE_SEGMENTED) || defined (ENABLE_BARGRAPH)
        row++;
#endif


///////xxxxxx

#ifdef ENABLE_CONSOLE
#if ( CONSOLE_LINE_START )>0
        if(y_pos_screen>=CONSOLE_LINE_START)
#endif
#if ( CONSOLE_LINE_END )<64
        if(y_pos_screen<CONSOLE_LINE_END)
#endif
        {
          u8 the_char = screen[charpos];
#ifdef ENABLE_CONSOLE0_FONT_SWITCHING
        int addr=font_adress_console_0;//+pgm_address(os_font);
        addr+=((int)the_char * 8) ;// opt+ (x_pos_screen & 7);
#endif

#ifdef ENABLE_CONSOLE_BIT7_INVERT
          u8 xor_mask=(the_char&0x80)?255:0;
#endif                



#ifdef ENABLE_CONSOLE_BIT7_BRIGHTNESS
          u8 bright=(the_char&0x80);//?1:0;
          the_char&=0x7f;




#ifndef ENABLE_CONSOLE0_FONT_SWITCHING
        
           u8 bits_0=pgm_read_byte(&os_font[((int)the_char << 3) + subpos_x]);
           u8 bits_1=0;//pgm_read_byte(128*8+&os_font[((int)the_char << 3) + subpos_x]);
#else
           u8 bits_0=pgm_read_byte(addr);
           u8 bits_1=0;//pgm_read_byte(addr+128*8);
#endif
      if(bright)bits_1=bits_0, bits_0=0;
      //swapu8(bits_0,bits_1);


#else   // no brightness attribute
#ifndef ENABLE_CONSOLE0_FONT_SWITCHING

#ifdef ENABLE_CONSOLE_BIT7_INVERT
          the_char&=0x7f;
#endif            
           u8 bits_0=pgm_read_byte(&os_font[((int)the_char << 3) + subpos_x]);
#ifdef ENABLE_CONSOLE_EXTENDED_COLOR         
          const u8 bits_1=pgm_read_byte(128*8+&os_font[((int)the_char << 3) + subpos_x]);
#endif
#else
          u8 bits_0=pgm_read_byte(addr);
#ifdef ENABLE_CONSOLE_EXTENDED_COLOR
          const u8 bits_1=pgm_read_byte(addr+128*8);
#endif
#endif

#endif  //brightness, else zweig




#ifdef ENABLE_CONSOLE_EXTENDED_COLOR
          u8 bits=pattern&bits_1;
          bits |=bits_0;
#else
          u8 bits=bits_0;
#endif

#ifdef CONSOLE_ENABLE_CURSOR

          if(charpos==cursor_pos)
#ifdef CONSOLE_ENABLE_CURSOR_BLINK
        //if(_engine_flags_and_frame_counter&1)//CONSOLE_CURSOR_BLINK_MASK)
          if(_engine_flags_and_frame_counter&CONSOLE_CURSOR_BLINK_MASK)
#endif
          bits=~bits;
#endif
          block_8_px |= bits


#ifdef ENABLE_CONSOLE_BIT7_INVERT
          ^xor_mask
#endif                
          ;
        }
#endif

#if defined(ENABLE_CONSOLE) || defined (CONSOLE_ENABLE_CURSOR) || defined (ENABLE_SEGMENTED)
          charpos+=16;
#endif
#ifdef ENABLE_SPRITES        
        static char vor;
        block_8_px |= vor;
        vor=0;
         if(!sprites_constant_counter)
        {
                }

      if(sprites_constant_counter<8)
      {
        u8 i;
        for(i=0;i<NR_SPRITES;i++)
        {
          if(Sprites[i].SkipCounter<8)
          {
            u16 tmp=pgm_read_byte(Sprites[i].readpos_byte++);
            tmp<<=Sprites[i].SkipCounter;
            u8 pat=Sprites[i].Color?pattern:255;
            block_8_px|=tmp&pat;
            vor|=(tmp>>8)&pat;
            Sprites[i].SkipCounter+=64-8;
            Sprites[i].sprite_height--;
            if(!Sprites[i].sprite_height)
              Sprites[i].SkipCounter=0x1fff;          
          } else Sprites[i].SkipCounter-=8;
        }
          u16 min=Sprites[0].SkipCounter;
         for(i=1;i<NR_SPRITES;i++)
          {
            if(Sprites[i].SkipCounter<min)min=Sprites[i].SkipCounter;
          }
          if(min)
          {
            sprites_constant_counter=min-(min&7);
            for(i=0;i<NR_SPRITES;i++)
              Sprites[i].SkipCounter-=sprites_constant_counter;
          }
 //*/
      
      }
      else sprites_constant_counter-=8;

       




#endif
#ifdef INTERLEAVE

        i2c_data_out=block_8_px;
       // must_float_pin=i2c_data_out&0x80;
#else
        os_i2c_write_byte(block_8_px);xxxxx
#endif
        block_8_px = 0;
#ifdef ENABLE_LAYERS
#endif
#ifdef ENABLE_SPRITES    // todo: reimplement sprites as int shift left n, oder wenigstens verschieben
#endif
#ifndef ENABLE_WIRE
      os_i2c_scl_low_ppull();
      os_i2c_sda_low();
#endif
      }  // ende 8 pixel block
      #ifdef INTERLEAVE      
        os_i2c_scl_low_ppull();
#endif       

    }   // ende y schleife
#ifdef ENABLE_WIRE
    wire_cnt++;
    if (wire_cnt==3)
    {
      wire_cnt=0;   
      Wire.endTransmission();
      Wire.beginTransmission(WIRE_SCREEN_ADDRESS);
      Wire.write(0x40);
    }
#endif
  }   // ende x schleife
#ifdef INTERLEAVE
  os_i2c_write_byte(i2c_data_out);
  _i2c_data_out=i2c_data_out;
#endif  
  os_i2c_stop();
#ifdef ENABLE_ATTINY_POWER_MANAGER
  _manage_battery();    
#endif
    u8 tmp=_engine_flags_and_frame_counter+1;
    tmp&=0x7f;
    _engine_flags_and_frame_counter&=0x80;
    _engine_flags_and_frame_counter|=tmp;


#ifdef ENABLE_VSYNC
    if (DisplaySingleFrame)
    {
      GfxApiSetDisplayMux(1);
      GfxApiSetDisplayOff();
    }
#endif
}
#undef INTERLEAVE
#ifdef DISABLE_HALFTONE
#undef DISABLE_HALFTONE
#endif
#undef STRINGNIZE
#undef STRINGNIZE_NX

#pragma GCC pop_options
#endif
