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


