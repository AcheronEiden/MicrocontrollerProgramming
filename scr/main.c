/*
 Created: 2023-01-23
 Author : Alhassan Jawad & Andreas Hertzberg (Group ANIMATION)
 */ 

//--------------------------Include(s)------------------------------------------------------------------
#include <stdio.h> // Allows us to perform input and output operations
#include <avr/io.h> // Includes the apropriate IO definition usage
#include <util/delay.h> // Includes the apropriate delay functions
#include <math.h> // Set of functions to compute common mathematical operations and transformations
#include <avr/interrupt.h> // Include the interrupt library
#include "spi.h"
#include "st7735.h"
#include "st7735_gfx.h"
#include "st7735_font.h"
#include "logo_bw.h"
#include "free_sans2.h"
//------------------------------------------------------------------------------------------------------

char buffer[40]; // Buffer denotes a computer memory block that acts as a temporary placeholder
volatile int counter = 0; // Decleared as volatile so microcontroller knows it can be changed

// Using External Interrupt Request 0 that will activate when the pushbutton is pressed
ISR(INT0_vect)
{
  // RESET ANIMATION
  st7735_fill_rect(0, 0, 128, 128, ST7735_COLOR_WHITE);
  counter = 0; // Reset the animation sequence
}

int main(void)
{
  DDRD = 0xF0; // All D pins becomes outputs
  PORTD |= 0xFC;
  EICRA = 0x0a; // Configures so that when the button is pressed it becomes an input
  sei(); // Enable global interrupts
  EIMSK |= 0x03 ; // External Interrupt 0 (INT0) enabled
  
  spi_init();
  st7735_init();

  st7735_set_orientation(ST7735_PORTRAIT);  
  st7735_fill_rect(0, 0, 128, 128, ST7735_COLOR_BLACK); // Covers the whole screen in black as bg color

  ADMUX = (0 << REFS1) | (1 << REFS0) | (0 << MUX3) | (0 << MUX2) | (0 << MUX1) | (0 << MUX0);
  ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0);

  while (ADCSRA & (1 << ADSC));
  
  while(1)
  {
    st7735_fill_rect(0, 0, 128, 128, ST7735_COLOR_BLACK); // Covers the whole screen in black as bg color
    if(counter % 10 == 0)
    {
      // ANIMATION 1
      // Display the red circle animation with radius 30 pixels centered at positions (64,64)
      st7735_draw_circle(64, 64, 30, ST7735_COLOR_RED);
    } else
    {
      // ANIMATION 2
      // Display the green rectangle animation with height/width of 80 pixels centered at (20, 20)
      st7735_draw_rect(20, 20, 80, 80, ST7735_COLOR_GREEN);
    }
    counter += 5;
    _delay_ms(5000);
  }
}