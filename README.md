# MicrocontrollerProgramming
This is for saving my project for the course Microcontroller Programming 10hp (Uppsala University course code: 1TE663).
The Program used is VSC together with the PlatformIO extension & the external AVRDUDESS program.

---------------------------------------------------------------

The chosen project is ANIMATION of LCD display (precisely the 1.4 inch SPI 128x128 GREEN TAB LCD display) with different animations based on a button press and INTERRUPTS & TIMERS.

The project isn't done!

---------------------------------------------------------------

The main file (where all the code is) is the main.c file that should be placed in the src file of the PlatformIO created project.

The files:
  - free_sans.h
  - free_sans2.h
  - logo.h
  - logo_bw.h
  - spi.h
  - spi.c
  - st7735.h
  - st7735.c
  - st7735_font.h
  - st7735_font.c
  - st7735_gfx.h
  - st7735_gfx.c
  - st7735initcmds.h
  - tom_thumb.h

are external libraries that should be saved in the lib or src folder of the PlatfromIO created project.

 - platformio.ini is the initiation file where it is stated that the used microcontroller used is an atmega328P, the clock frequency is 1000000UL and where the AVRDUDE is located
