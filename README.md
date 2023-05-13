# ssd1306-undocumented-grayscale-mode

this is a test for the undocumented register 0x9a=2 of the ssd1306, it enables a 128x32 grayscale mode, this is stretched using the zoom
command to fill the screen ( 0xd6=1)

if this works for you please open a ticket, if it does not please also open a ticket!

see function TestCmd for the used commands (0x9a,2,0xd6,1)

https://www.youtube.com/shorts/1Vx-WqN9S30



the pixel format is simple 2 pixels on y become one, summed up
pixel at (0,0) = 1 , pixel at (0,1) = 0   -> both pixels get 0.5 brightness
if both are 1 its 1 brightness for both (and for 0 the same)
