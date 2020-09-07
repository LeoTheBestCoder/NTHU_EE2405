#include "mbed.h"

void rLed(DigitalOut& ledpin)
{
   for(int i=0; i<6; ++i) { //blink for 10 times
       ledpin = !ledpin; // toggle led
       wait(0.2f);
   }
}


void gLed(DigitalOut& ledpin)
{
   for(int i=0; i<4; ++i) { //blink for 10 times
       ledpin = !ledpin; // toggle led
       wait(0.2f);
   }
}