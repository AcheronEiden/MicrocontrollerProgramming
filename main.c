/*
 * 20210907_m328_st7735_lcd.c
 *
 * Created: 2021-09-07 20:25:40
 * Author : uwezi
 */ 

#include <avr/io.h> // includes the apropriate IO definition usage
#include <util/delay.h> // includes the apropriate delay functions
#include <math.h>
#include "spi.h"
#include "st7735.h"
#include "st7735_gfx.h"
#include "st7735_font.h"

#include "logo_bw.h"
#include "free_sans2.h"

#include <stdio.h> // allows us to perform input and output operations


char buffer[40];

int main(void) 
{
  int counter = 0;
  
  spi_init();
  st7735_init();

  st7735_set_orientation(ST7735_PORTRAIT);  
  st7735_fill_rect(0, 0, 128, 128, ST7735_COLOR_BLACK);
  st7735_draw_fast_hline(10, 30, 100, ST7735_COLOR_RED);

  ADMUX = (0 << REFS1) | (1 << REFS0) | (0 << MUX3) | (0 << MUX2) | (0 << MUX1) | (0 << MUX0);
  ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0);

  while (ADCSRA & (1 << ADSC));

  while(1)
  { 
    st7735_fill_rect(0, 0, 128, 128, ST7735_COLOR_BLACK);
    if(counter % 10 == 0)
    {
      // Display the red circle animation
      st7735_draw_circle(64, 64, 30, ST7735_COLOR_RED);
    }else
    {
      // Display the green rectangle animation
      st7735_draw_rect(20, 20, 80, 80, ST7735_COLOR_GREEN);
    }
    counter += 5;
    _delay_ms(5000);
  }
}