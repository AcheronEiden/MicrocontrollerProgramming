/*Information

 Created: 2023-01-23
 Author : Alhassan Jawad & Andreas Hertzberg (Group ANIMATION)
 About:
        The code shows 5 differnt animations and it is an application written in C that
        uses an Atmega328P microcontroller to implement 4 different subtasks:

          - The first subtask is the usage of GPIO and inputs/outputs to
              1) display animations on an LCD display
              2) make use of push buttons.
          - The second subtask that this code implements is the usage of Timers, specifically the
            usage of timer0 which calculates how long the code have been running.
          - The thrid & fourth tasks is the usage of Interrupts where two external
            push buttons are used for reseting the internal timer0 & changing how
            the LCD will show the specific animations (either sequentially or in a random order).


        The code uses libraries for functions such as input/output, delays, interrupts, and working
        with time and date. The code also uses a number of libraries for the LCD display,
        including functions for setting the orientation and filling rectangles, as well as libraries
        for the display's graphics and fonts.
 */

//--------------------------Include(s)----------------------------------------------------------------
#include <stdio.h>         // Allows us to perform input and output operations
#include <avr/io.h>        // Includes the apropriate IO definition usage
#include <util/delay.h>    // Includes the apropriate delay functions
#include <math.h>          // Functions to compute common mathematical operations and transformations
#include <avr/interrupt.h> // Includes the interrupt library & its functions
#include <stdlib.h>        // Collection of functions for performing general purpose tasks
#include <time.h>          // Collection of functions and macros for working with time and date
#include "spi.h"           // Library located in src folder
#include "st7735.h"        // Library located in src folder
#include "st7735_gfx.h"    // Library located in src folder
#include "st7735_font.h"   // Library located in src folder
#include "logo_bw.h"       // Library located in src folder
#include "free_sans2.h"    // Library located in src folder
//----------------------------------------------------------------------------------------------------

// Global variables, characters & specific pin initialization
#define LED_PIN PD7                // Defines pin D7 as a Led_PIN to help with the AnimationType task
char buffer[40];                   // Computer memory block that acts as a temporary placeholder
volatile uint16_t counter_time = 0; // Volatile so microcontroller knows it can/will be changed
volatile int rand_on = 1;          // Volatile so microcontrolelr knows it can/will be changed
volatile int sequence = 0;                  // The specific animation that will be shown

// Using External Interrupt Request 0 that will activate when the first pushbutton
// is pressed and will reset the internal timer0 (Animation5)
ISR(INT0_vect)
{
  counter_time = 0;
}

// Using External Interrupt 1 that will activate when the second pushbutton
// is pressed and will make the shown animation sequence sequential or random (AnimationType)
ISR(INT1_vect)
{
  if (rand_on == 1)
  {
    if (sequence == 0)
    {
      sequence = 1;
    }
    else
    {
      sequence = 0;
    }

    rand_on = 0;
    PORTD &= ~(1 << LED_PIN); // Turns of the red diode
  }
  else
  {
    rand_on = 1;
    PORTD |= (1 << LED_PIN); // Turns on the red diode
  }
}

// Using Timer0 to caluclate the code's running time
ISR(TIMER0_OVF_vect)
{
  counter_time++;
}

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

// Main Code
int main(void)
{
  DDRD = 0b11111111; // All D pins becomes outputs
  PORTD = 0b11111100;
  DDRC = 0b00111111; // All C pins becomes outputs

  EIMSK = (1 << INT1) | (1 << INT0); // External Interrupts INT0 & INT1 enabled
  EICRA = EICRA = (1 << ISC11) | (0 << ISC10) |
                  (1 << ISC01) | (0 << ISC00); // When a button is pressed it becomes an input
  sei();                                       // Enables global interrupts

  init();        // Runs the init code (line 85-99)
  spi_init();    // Runs the init code in the external library spi.c
  st7735_init(); // Runs the init code in the external library st7735.c

  // Decides if the Animation sequence will be random or not
  int animation = 0;

  st7735_set_orientation(ST7735_PORTRAIT);
  // Covers the whole screen in black as bg color
  st7735_fill_rect(0, 0, 128, 128, ST7735_COLOR_BLACK);

  ADMUX = (0 << REFS1) | (1 << REFS0) | (0 << MUX3) | (0 << MUX2) | (0 << MUX1) | (0 << MUX0);
  ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0);

  while (ADCSRA & (1 << ADSC))
    ;

  while (1)
  {
    // Covers the whole screen in black as bg color
    st7735_fill_rect(0, 0, 128, 128, ST7735_COLOR_BLACK);
    if (rand_on == 1)
    {
      animation = rand() % 5; // Generates a random number between 0 and 4
    }
    else
    {
      animation = sequence;    // Choose which animation will be shown
      sequence++;              // Increment sequence number
      sequence = sequence % 5; // Limits the chosen case between 0-4
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
      PORTC |= (1 << PC5);
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
      PORTC &= ~(1 << PC5);
      break;

    case 1: // ANIMATION 2: Rectangle(s)
      PORTC |= (1 << PC4);
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
      PORTC &= ~(1 << PC4);
      break;

    case 2: // Animation 3: A big star that grows
      PORTC |= (1 << PC3);
      for (c_animation5 = 0; c_animation5 <= 14; c_animation5++)
      {
        const uint16_t color_animation5[] = {ST7735_COLOR_RED, ST7735_COLOR_GREEN, ST7735_COLOR_BLUE,
                                             ST7735_COLOR_YELLOW, ST7735_COLOR_BLUE, ST7735_COLOR_CYAN,
                                             ST7735_COLOR_MAGENTA, ST7735_COLOR_WHITE, ST7735_COLOR_RED,
                                             ST7735_COLOR_GREEN, ST7735_COLOR_YELLOW, ST7735_COLOR_CYAN,
                                             ST7735_COLOR_MAGENTA, ST7735_COLOR_WHITE,
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
      PORTC &= ~(1 << PC3);
      break;

    case 3: // Animation 4: A lot of stars at random positions
      PORTC |= (1 << PC2);
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
      PORTC &= ~(1 << PC2);
      break;

    case 4: // Animation 5: Digita clock that shows running time
      PORTC |= (1 << PC1);
      uint16_t time_seconds = counter_time/31; // 0.03264 * counter_time
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
      PORTC &= ~(1 << PC1);
      break;
    }
  }
}