# ssd1306-undocumented-grayscale-mode

this is a test for the undocumented register 0x9a=2 of the ssd1306, it enables a 128x32 grayscale mode, this is stretched using the zoom
command to fill the screen ( 0xd6=1)

if this works for you please open a ticket, if it does not please also open a ticket!

see function TestCmd for the used commands (0x9a,2,0xd6,1), Set Mux to 64

https://www.youtube.com/shorts/1Vx-WqN9S30



the pixel format is simple 2 pixels on y become one, summed up
pixel at (0,0) = 1 , pixel at (0,1) = 0   -> both pixels get 0.5 brightness
if both are 1 its 1 brightness for both (and for 0 the same)

this mode only seems to work on few ssd1306, but i found a second one, which works by setting zoom=1 then driving the vsync via mux, the display overwrites the screen two times in this case.

see:
https://www.youtube.com/shorts/LxZx6vhKvHE


----- Scan for undocumented registers, my second ssd1306 has more registers these registers are only those present in both displays
all registers scanned and "analyzed": 
  undocumented register A9
  test:display 1: 
  0: black screen
  255: normal
  254: off
  253: off
  0xf:on
  7: darkened, elapsed
  0b1011: normal
  0b1001: black
  0b1010: black
  0b1110: black
  0b0011: darkened, elapsed identical 7, bit 2 probably no function on screen 1 but on screen 2
  display 2:
  0b0011: off
  0b0111: normal
  0b1011: normal darker than 0b1111,
  0b1111: on, identical 255

  -> works on both: 0b1111: screen on
  0b0000: off
  the upper 4 bits seem to have no function
  probably this register controls the power of the oled, switching from 15 to 0 makes the screen go blacck in about 2 frames.


  undocumented register 9F
  test:Display 1: write causes black screen, apparently irreversible

  undocumented register AD, documented for ssd1303, DC Control
  test:Display 1: 1p, present, influence on brightness, bit 0 seems to have no effect

  undocumented register D8, documented for ssd1303 area color, low power display mode
  test:display 1: present, affects brightness when written to, probably low power mode support

  undocumented register D4, differs between displays, maybe parameter 2 & 3 are dummies 
  test:Display 1: 1p, timing? row timing?
  display 2: 3p
  parameter 1: timing effect
  parameter 2: nothing visible directly
  parameter 3: nothing visible directly

  undocumented register DD
  test:Display 1: also something there, corrupts the vram at one bit
  Display 2: one bit has a brightness effect
 
  undocumented Register F2
  test:Display 1: Lock vram (1<<4?), 32 crash?

  undocumented register FD, documented for ssd1309, lock register
  test:Display 1: command lock works with 0x16 for lock, 0x12 for unlock
  works as expected

  undocumented register D1
  test:Display 1
  Something with row Timing?
  display 2
  picture black depending on bit
addition undocumented stuff is in vcomh



+ mux transistion trick for VSYNC
    mux can be used for vsync
    
    to display a single Frame (oneshot style):
      set mux to 63 
      wait 150 us 
      set mux to 1 (one of my displays has problems with mux 0)
      
+ the mux=1 , move scanline to draw grayscale images trick
      mux=1 results in a 2 pixels high image
      move this image with the "Set Display Offset Register" from the top of the screen to the bottom
      for each line set the sourceline
        delay a bit, 9khz rate of the calls seems fine.
        now you can draw the image two times, and increment the source line every other line. and change the brightness between the two phases
        and you got a 4 color grayscale mode

+ the scroll area are in fact line compare registers, which work fine without the scroll commands

+ the content scroll commands of the ssd1309 work to allow scrolling on X and Y without the display doing continous scroll
    the second dummy in content scroll, which "should" be set to 1 is a flag for "clear new pixels/rotate image"

+ the normal scroll commands accept a window on x, the both dummy parametes which should be set to 255 are start and and, this is documented in the ssd1309 and ssd1306B Datasheet

+ larger range in the charge pump, documented for ssd1306b Datasheet

+ grayscale by the following procedure:
      Display a single Frame, 3 Colors, aproximatly
      Disable Remap(0xc0,0xa0) otherwise there will be artifacts
      Set Zoom=1 (register 0xd6)
      
      Display a single Frame
      
      set mux to 63 
      wait 150 us 
      set mux to 1 (one of my displays has problems with mux 0)


