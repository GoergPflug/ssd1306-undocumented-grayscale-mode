## Grayscale HW Grayscale Mode (The New and Compatible One)

There are two modes for achieving grayscale on the SSD1306 display. The first mode seems to work only on a few SSD1306 displays. However, I found a second mode that works by setting `zoom=1` and driving the VSYNC by chaning the mux register periodically. In this case, the display overwrites the screen two times.

commands:

0xd6,1,      //zoom:1

,0xa8,63,   //mux 64

0xe3,0xe3,0xe3,0xe3,0xe3,0xe3,0xe3,0xe3,0xe3,0xe3,   // this must be enough nops to cause 150us(?) delay (by transfering them to the display)

0xe3,0xe3,0xe3,0xe3,0xe3,0xe3,0xe3,0xe3,0xe3,0xe3,

0xa8,1   // mux:1



To see a demonstration of the new grayscale mode, please watch the following YouTube video:

https://www.youtube.com/shorts/bLATW7clPwE

Please note that the original undocumented mode that works only on a few (newer?) displays is described at the bottom of this document.

## Copper Effects on the SSD1306

this can be easily achieved by the Mux trick (and changing contrast)

https://www.youtube.com/watch?v=LOilqUmZNBU&feature=youtu.be

# Scan for Undocumented Registers - SSD1306

This is the result of testing the registers of the SSD1306 for undocumented features. The following registers were found in both displays.

## Undocumented Register A9

**Test: Display 1**

- 0: Black screen
- 255: Normal
- 254: Off
- 253: Off
- 0xf: On
- 7: Darkened, elapsed
- 0b1011: Normal
- 0b1001: Black
- 0b1010: Black
- 0b1110: Black
- 0b0011: Darkened, elapsed (identical to 7) - Bit 2 probably has no function on screen 1 but does on screen 2

**Display 2**

- 0b0011: Off
- 0b0111: Normal
- 0b1011: Normal, darker than 0b1111
- 0b1111: On (identical to 255)

The following values work on both displays:

- 0b1111: Screen on
- 0b0000: Off

The upper 4 bits seem to have no function. This register probably controls the power of the OLED. Switching from 15 to 0 makes the screen go black in about 2 frames.


## Undocumented Register A9

- bit 0 and 1 must be set or screen will go black. display1: brighness effect for the rest of the bits


## Undocumented Register 9F

**Test: Display 1**

Writing to this register causes a black screen, and it appears to be irreversible.

## Undocumented Register AD

This register is documented for SSD1303 as DC Control.

**Test: Display 1**

- Bit 0 seems to have no effect.
- Influences brightness.

## Undocumented Register D8

This register is documented for SSD1303 as Area Color and low power display mode. Probably low power mode is present.

**Test: Display 1**

- Affects brightness when written to.
- Probably supports low power mode.

## Undocumented Register 0x24, Hardware accelerated rectangle.
This command draws a rectangle

0x24

,0,

2,  // row

fuzz_d2,  // pattern to fill 

4,  // row

32  // xstart

,64 // xend



## Undocumented Register D0

## Undocumented Register D2

bit 7 stripes
bit ? black screen


## Undocumented Register D4

Differs between displays, and parameters 2 and 3 may be dummies.

**Test: Display 1**

- 1p (timing? row timing?)
  
**Display 2**

- 3p
- Parameter 1: Timing effect
- Parameter 2: Nothing visible directly
- Parameter 3: Nothing visible directly

## Undocumented Register DD

**Test: Display 1**
  
  not present

**Display 2**

- One bit has a brightness effect

## Undocumented Register F2

**Test: Display 1**

- Lock VRAM (1<<4?)
- 32 crash?

## Undocumented Register FD

This register is documented for SSD1309 as a lock register.

**Test: Display 1**

- Command lock works with 0x16 for lock, 0x12 for unlock
- Works as expected

## Undocumented Register D1

**Test: Display 1**

- Something with row timing?, when in 128x32x3 bit 0 flashes the screen, must be 0...other bits slight brightness effect

**Display 2**

- Picture black depending on bit

Additional undocumented stuff is in VCOMH.

## Mux Transition Trick for VSYNC

Mux can be used for VSYNC. The trick is based on the idea to let the SSD1306 display a single (or 2) lines, and then set the image height (mux) to 64, wait at least 150us and set mux back to 1 (or 0, 0 would be better, but my newer display needs mux:1, which also works on my older). To display a single frame (oneshot style) you do the following:
- Set mux to 63.
- Wait 150 us. (just insert the nop commmand a few times in your command stream)
- Set mux to 1 (one of my displays has problems with mux 0). 
- repeat at your intended framerate
- make the top 2 lines black or they will glow pretty bright.

## The Mux=1 Trick - Move Scanline to Draw Grayscale Images

- Mux=1 results in a 2 pixels high image.
- Move this image with the "Set Display Offset Register" from the top of the screen to the bottom.
- For each line, set the sourceline.
- Delay a bit, 9kHz rate of the calls seems fine.
- Now you can draw the image two times, increment the source line every other line, and change the brightness between the two phases.
- You'll get a 4-color grayscale mode.




## Scroll Area as Line Compare Registers

The scroll area functions as line compare registers and works fine without the scroll commands, just change display offset, and startline...this allows for a scrolling area in the center of the screen

## the Content Scroll Commands for SSD1309 work on the SSD1306

The content scroll commands of the SSD1309 allow scrolling on the X and Y axes without the display continuously scrolling.
- The second dummy in content scroll, which "should" be set to 1, acts as a flag for "clear new pixels/rotate image".
- content scroll at mux=1 is very fast: https://www.youtube.com/watch?v=DmiUed8LGRo

## Normal Scroll Commands

The normal scroll commands accept a window on the X axis. Both dummy parameters, which should be set to 255, represent the start and end points. This information is documented in the SSD1309 and SSD1306B datasheets.

## Larger Range in Charge Pump

The SSD1306B datasheet documents a larger range in the charge pump.

## Hardware 3 Color Grayscale Mode

To achieve grayscale mode,the palette will close to 0,195,255, follow this procedure:

1. Disable Remap (0xc0,0xa0) to avoid artifacts.
2. Set Zoom=1 (register 0xd6).
Display a single frame: the ssd1306 will "overrun" and draw the zoomed image 2 times, so the pixels from row 32 in vram get added to row 0 and 1, from row 33 to 2 and 3  
3. Set the mux to 63.
4. Wait for 150 microseconds.
5. Set the mux to 1. (Note: One of my displays has problems with mux 0.)
6. wait, goto 3 at 60hz rate.

# ssd1306-undocumented-grayscale-mode on SOME new ssd1306

this is a test for the undocumented register 0x9a=2 of the ssd1306, it enables a 128x32 grayscale mode, this is stretched using the zoom
command to fill the screen ( 0xd6=1)

if this works for you please open a ticket, if it does not please also open a ticket!

see function TestCmd for the used commands (0x9a,2,0xd6,1), Set Mux to 64

https://www.youtube.com/shorts/1Vx-WqN9S30



the pixel format is simple 2 pixels on y become one, summed up
pixel at (0,0) = 1 , pixel at (0,1) = 0   -> both pixels get 0.5 brightness
if both are 1 its 1 brightness for both (and for 0 the same)
