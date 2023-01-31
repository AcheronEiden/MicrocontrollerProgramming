/*
 Created: 2023-01-23
 Author : Alhassan Jawad & Andreas Hertzberg (Group ANIMATION)
 */

//--------------------------Include(s)------------------------------------------------------------------
#include <stdio.h>         // Allows us to perform input and output operations
#include <avr/io.h>        // Includes the apropriate IO definition usage
#include <util/delay.h>    // Includes the apropriate delay functions
#include <math.h>          // Set of functions to compute common mathematical operations and transformations
#include <avr/interrupt.h> // Includes the interrupt library & its functions
#include <stdlib.h>        // Collection of functions for performing general purpose tasks
#include <time.h>          // Collection of functions and macros for working with time and date
#include "spi.h"           // Library located in src folder
#include "st7735.h"        // Library located in src folder
#include "st7735_gfx.h"    // Library located in src folder
#include "st7735_font.h"   // Library located in src folder
#include "logo_bw.h"       // Library located in src folder
#include "free_sans2.h"    // Library located in src folder
//------------------------------------------------------------------------------------------------------

// Global variables & characters
#define LED_PIN PD7
char buffer[40];                   // Denotes a computer memory block that acts as a temporary placeholder
volatile int16_t counter_time = 0; // Decleared as volatile so microcontroller knows it can/will be changed
volatile int rand_on = 1;          // Declared as volatile so microcontrolelr knows it can/will be changed
int sequence = 0;                  // The specific animation that will be shown

// Initialize the timer
void init(void)
{
  // Selects 1) Normal Operation Mode (COM0A1|COM0A0)
  //	       2) OC0A disconnected (COM0A1|COM0A0)
  //	       3) Waveform Generation Mode 0 (WGM01|WGM00)
  TCCR0A = (0 << COM0A1) | (0 << COM0A0) | (0 << COM0B1) | (0 << COM0B0) | (0 << WGM01) | (0 << WGM00);

  // Selects 1) Waveform Generation Mode 0 (WGM01|WGM00)
  //	       2) Clock select (prescaler) (CS02|CS01|CS00)
  TCCR0B = (0 << FOC0A) | (0 << FOC0B) | (0 << WGM02) | (1 << CS02) | (0 << CS01) | (1 << CS00);

  // 1) Disables OCIE0B/OCIE0A Compare Match Interrupt
  // 2) Enable TOIE0 Overflow Interrupt
  TIMSK0 = (0 << OCIE0B) | (0 << OCIE0A) | (1 << TOIE0);
}

// Using External Interrupt Request 0 that will activate when first the pushbutton
// is pressed and will reset the internal timer0
ISR(INT0_vect)
{
  counter_time = 0;
}

// Using External Interrupt 1 that will activate when the second pushbutton
// is pressed and will make the shown animation sequence sequential or random
ISR(INT1_vect)
{
  if (rand_on == 1)
  {
    sequence = 0;
    rand_on = 0;
    PORTD |= (0 << LED_PIN); // DDRD |= (0 << LED_PIN);
  }
  else
  {
    rand_on = 1;
    PORTD |= (1 << LED_PIN); // DDRD |= (1 << LED_PIN);
  }
}

// Using Timer0 to caluclate the code's running time
ISR(TIMER0_OVF_vect)
{
  counter_time++;
}

// Main Code
int main(void)
{
  DDRD = 0xF0; // Same as 0b11110000
  PORTD |= 0xFC; // Same as 0b11111100
  EICRA = 0x0a;                      // Configures so that when the button is pressed it becomes an input
  sei();                             // Enables global interrupts
  EIMSK = (1 << INT1) | (1 << INT0); // External Interrupt 0 (INT0) enabled

  init();        // Runs the init code (line 27-41)
  spi_init();    // Runs the init code in the external library spi.c
  st7735_init(); // Runs the init code in the external library st7735.c

  // Decides if the Animation sequence will be random or not
  int animation = 0;

  st7735_set_orientation(ST7735_PORTRAIT);
  st7735_fill_rect(0, 0, 128, 128, ST7735_COLOR_BLACK); // Covers the whole screen in black as bg color

  ADMUX = (0 << REFS1) | (1 << REFS0) | (0 << MUX3) | (0 << MUX2) | (0 << MUX1) | (0 << MUX0);
  ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0);

  while (ADCSRA & (1 << ADSC))
    ;

  while (1)
  {
    st7735_fill_rect(0, 0, 128, 128, ST7735_COLOR_BLACK); // Covers the whole screen in black as bg color
    if (rand_on == 1)
    {
      animation = rand() % 6; // Generates a random number between 0 and 4
    }
    else
    {
      animation = sequence;    // Choose which animation will be shown
      sequence++;              // Increment sequence number
      sequence = sequence % 6; // Limits the chosen case between 0-5
    }
    switch (animation)
    {
      int r;            // Circle (Animation 1) radius modification
      int i;            // Rectanlge (Animation 2) position modification
      int16_t a, b;     // Smiley (Animation 3) parameters
      int i2;           // For star (Animation 4) position modificaton
      int i3;           // For drawing star (Animation 5)
      int c_animation5; // For star (Animation 5) color modification

    case 0: // Animation 1: Circle(s)
      for (r = 0; r < 11; r++)
      {
        int radius = r * 5 + 10;                         // Increment radius by 5 pixels each iteration
        int color_animation1 = ST7735_COLOR_RED + 1 * r; // Increment color by 1 each iteration
        // Draws cirlce with specified color & radius at position (64,64)
        st7735_draw_circle(64, 64, radius, color_animation1);
        _delay_ms(500);
      }
      // Covers the whole screen in black as bg color
      st7735_fill_rect(0, 0, 128, 128, ST7735_COLOR_BLACK);
      break;

    case 1: // ANIMATION 2: Rectangle(s)
      for (i = 0; i < 12; i++)
      {
        int x = i * 5;                                      // Increment x by 5 pixels each iteration
        int y = i * 5;                                      // Increment y by 5 pixels each iteration
        int color_animation2 = ST7735_COLOR_GREEN + 10 * i; // Increment color by 10 each iteration
        // Draws a 80x80 pixels rectanlge with specified color at position (x,y)
        st7735_draw_rect(x, y, 80, 80, color_animation2);
        _delay_ms(500);
      }
      // Covers the whole screen in black as bg color
      st7735_fill_rect(0, 0, 128, 128, ST7735_COLOR_BLACK);
      break;

    case 2: // Animation 3: Smiley Face
      // Draws the head
      for (b = 20; b < 60; b++)
      {
        uint16_t color_animation3 = ST7735_COLOR_YELLOW;
        for (a = 20; a < 60; a++)
        {
          st7735_draw_pixel(a, b, color_animation3);
        }
      }

      // Draws the eyes
      st7735_draw_pixel(30, 30, ST7735_COLOR_BLACK);
      st7735_draw_pixel(50, 30, ST7735_COLOR_BLACK);

      // Draws the mouth
      for (a = 30; a < 50; a++)
      {
        st7735_draw_pixel(a, 50, ST7735_COLOR_BLACK);
      }
      _delay_ms(500);
      // Covers the whole screen in black as bg color
      st7735_fill_rect(0, 0, 128, 128, ST7735_COLOR_BLACK);
      break;

    case 3: // Animation 4: A lot of stars at random positions
      for (i2 = 0; i2 < 50; i2++)
      {
        int16_t x = rand() % 128;
        int16_t y = rand() % 128;
        int c_animation4, color_animation4;
        for (c_animation4 = 0; c_animation4 < 5; c_animation4++)
        {
          color_animation4 = ST7735_COLOR_WHITE;
          st7735_draw_pixel(x - c_animation4, y - c_animation4, color_animation4);
          st7735_draw_pixel(x + c_animation4, y - c_animation4, color_animation4);
          st7735_draw_pixel(x - c_animation4, y + c_animation4, color_animation4);
          st7735_draw_pixel(x + c_animation4, y + c_animation4, color_animation4);
        }
        for (c_animation4 = 0; c_animation4 < 4; c_animation4++)
        {
          color_animation4 = ST7735_COLOR_WHITE;
          st7735_draw_pixel(x - c_animation4, y, color_animation4);
          st7735_draw_pixel(x + c_animation4, y, color_animation4);
          st7735_draw_pixel(x, y - c_animation4, color_animation4);
          st7735_draw_pixel(x, y + c_animation4, color_animation4);
        }
        st7735_draw_pixel(x, y, ST7735_COLOR_WHITE);
      }
      _delay_ms(1000);
      // Covers the whole screen in black as bg color
      st7735_fill_rect(0, 0, 128, 128, ST7735_COLOR_BLACK);
      break;

    case 4: // Animation 5: A big star that grows
      for (c_animation5 = 0; c_animation5 <= 14; c_animation5++)
      {
        const uint16_t color_animation5[] = {ST7735_COLOR_RED, ST7735_COLOR_GREEN, ST7735_COLOR_BLUE,
                                             ST7735_COLOR_YELLOW, ST7735_COLOR_BLUE, ST7735_COLOR_CYAN, ST7735_COLOR_MAGENTA,
                                             ST7735_COLOR_WHITE, ST7735_COLOR_RED, ST7735_COLOR_GREEN, ST7735_COLOR_YELLOW,
                                             ST7735_COLOR_CYAN, ST7735_COLOR_MAGENTA, ST7735_COLOR_WHITE,
                                             ST7735_COLOR_BLACK};
        int color = color_animation5[c_animation5];
        for (i3 = 0; i3 < 50; i3++)
        {
          int l3;
          int16_t q = 64;
          int16_t p = 64;
          for (l3 = 0; l3 < 5; l3++)
          {
            st7735_draw_pixel(q - l3 - i3, p - l3 - i3, color);
            st7735_draw_pixel(q + l3 + i3, p - l3 - i3, color);
            st7735_draw_pixel(q - l3 - i3, p + l3 + i3, color);
            st7735_draw_pixel(q + l3 + i3, p + l3 + i3, color);
          }
          for (l3 = 0; l3 < 4; l3++)
          {
            st7735_draw_pixel(q - l3 - i3, p, color);
            st7735_draw_pixel(q + l3 + i3, p, color);
            st7735_draw_pixel(q, p - l3 - i3, color);
            st7735_draw_pixel(q, p + l3 + i3, color);
          }
          st7735_draw_pixel(q, p, color);
        }
        _delay_ms(100);
      }
      // Covers the whole screen in black as bg color
      // st7735_fill_rect(0, 0, 128, 128, ST7735_COLOR_BLACK);
      break;

    case 5: // Animation 6: Digital clock with running time since start of code
      while (1)
      {
        int16_t time_seconds = 0.03264 * counter_time;
        int hours = time_seconds / 3600;
        int minutes = (time_seconds % 3600) / 60;
        int seconds = (time_seconds % 3600) % 60;

        struct tm time_info = {0};
        time_info.tm_hour = hours;
        time_info.tm_min = minutes;
        time_info.tm_sec = seconds;
        strftime(buffer, 40, "%H:%M:%S", &time_info);
        st7735_draw_text(20, 64, buffer, &Open_Sans_Regular_20, 1, ST7735_COLOR_BLUE);
        _delay_ms(5000);
        // Covers the whole screen in black as bg color
        st7735_fill_rect(0, 0, 128, 128, ST7735_COLOR_BLACK);
        break;
      }
      break;
    }
  }
}