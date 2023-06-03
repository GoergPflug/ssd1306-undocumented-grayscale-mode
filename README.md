
the first mode only seems to work on few ssd1306, but i found a second one, which works by setting zoom=1 then driving the vsync via mux, the display overwrites the screen two times in this case. the orginial undocumented mode is at the bottom of this document

Grayscale HW Grayscale mode (the new and compatible one):

see:
https://www.youtube.com/shorts/LxZx6vhKvHE
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

- Corrupts the VRAM at one bit

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

- Something with row timing?

**Display 2**

- Picture black depending on bit

Additional undocumented stuff is in VCOMH.




----old:
# Scan for Undocumented Registers in SSD1306

All registers scanned and analyzed:

## Undocumented Register A9

### Test: Display 1
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
- 0b0011: Darkened, elapsed (identical to 7, bit 2 probably has no function on screen 1 but on screen 2)

### Test: Display 2
- 0b0011: Off
- 0b0111: Normal
- 0b1011: Normal (darker than 0b1111)
- 0b1111: On (identical to 255)

- Works on both: 0b1111 - Screen on
- 0b0000: Off
- The upper 4 bits seem to have no function
- Probably this register controls the power of the OLED. Switching from 15 to 0 makes the screen go black in about 2 frames.

## Undocumented Register 9F

### Test: Display 1
- Writing causes a black screen, apparently irreversible.

## Undocumented Register AD (DC Control)

### Test: Display 1
- 1p present, influences brightness. Bit 0 seems to have no effect.

## Undocumented Register D8 (Area Color, Low Power Display Mode)

### Test: Display 1
- Present, affects brightness when written to. Probably supports low power mode.

## Undocumented Register D4

### Test: Display 1
- 1p timing? Row timing?

### Test: Display 2
- 3p
- Parameter 1: Timing effect
- Parameter 2: Nothing visible directly
- Parameter 3: Nothing visible directly

## Undocumented Register DD

### Test: Display 1
- Also something there, corrupts the VRAM at one bit.

### Test: Display 2
- One bit has a brightness effect.

## Undocumented Register F2

### Test: Display 1
- Lock VRAM (1<<4?). 32 crash?

## Undocumented Register FD (Lock Register)

### Test: Display 1
- Command lock works with 0x16 for lock, 0x12 for unlock. Works as expected.

## Undocumented Register D1

### Test: Display 1
- Something with row timing?

### Test: Display 2
- Picture black depending on bit.

Additional undocumented stuff is in VCOMH.

## Mux Transition Trick for VSYNC

Mux can be used for VSYNC. To display a single frame (oneshot style):
- Set mux to 63.
- Wait 150 us.
- Set mux to 1 (one of my displays has problems with mux 0).

## The Mux=1 Trick - Move Scanline to Draw Grayscale Images

- Mux=1 results in a 2 pixels high image.
- Move this image with the "Set Display Offset Register" from the top of the screen to the bottom.
- For each line, set the sourceline.
- Delay a bit, 9kHz rate of the calls seems fine.
- Now you can draw the image two times, increment the source line every other line, and change the brightness between the two phases.
- You'll get a 4-color grayscale mode.





## Mux Transition Trick for VSYNC

The mux can be used for VSYNC.

To display a single frame (oneshot style):
1. Set the mux to 63.
2. Wait for 150 microseconds.
3. Set the mux to 1. (Note: One of my displays has problems with mux 0.)

## Mux=1, Move Scanline to Draw 4 Color Grayscale Images Trick

Setting the mux to 1 results in a 2-pixel high image.
Follow these steps to draw grayscale images:
1. Move this image using the "Set Display Offset Register" from the top of the screen to the bottom.
2. For each line, set the sourceline.
3. Delay for a bit. A 9kHz rate of the calls seems fine.
4. Now, you can draw the image two times and increment the source line every other line. Change the brightness between the two phases, and you'll achieve a 4-color grayscale mode.

## Scroll Area as Line Compare Registers

The scroll area functions as line compare registers and works fine without the scroll commands.

## Content Scroll Commands for SSD1309

The content scroll commands of the SSD1309 allow scrolling on the X and Y axes without the display continuously scrolling.
- The second dummy in content scroll, which "should" be set to 1, acts as a flag for "clear new pixels/rotate image".

## Normal Scroll Commands

The normal scroll commands accept a window on the X axis. Both dummy parameters, which should be set to 255, represent the start and end points. This information is documented in the SSD1309 and SSD1306B datasheets.

## Larger Range in Charge Pump

The SSD1306B datasheet documents a larger range in the charge pump.

## Hardware 3 Color Grayscale Mode

To achieve grayscale mode, follow this procedure:
1. Display a single frame with approximately 3 colors.
2. Disable Remap (0xc0,0xa0) to avoid artifacts.
3. Set Zoom=1 (register 0xd6).
4. Display the single frame.
5. Set the mux to 63.
6. Wait for 150 microseconds.
7. Set the mux to 1. (Note: One of my displays has problems with mux 0.)


# ssd1306-undocumented-grayscale-mode on SOME new ssd1306

this is a test for the undocumented register 0x9a=2 of the ssd1306, it enables a 128x32 grayscale mode, this is stretched using the zoom
command to fill the screen ( 0xd6=1)

if this works for you please open a ticket, if it does not please also open a ticket!

see function TestCmd for the used commands (0x9a,2,0xd6,1), Set Mux to 64

https://www.youtube.com/shorts/1Vx-WqN9S30



the pixel format is simple 2 pixels on y become one, summed up
pixel at (0,0) = 1 , pixel at (0,1) = 0   -> both pixels get 0.5 brightness
if both are 1 its 1 brightness for both (and for 0 the same)
