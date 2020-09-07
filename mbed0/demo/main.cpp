#include "mbed.h"

DigitalOut redLED(LED_RED);
DigitalOut greenLED(LED_GREEN);

void rLed(DigitalOut& ledpin);
void gLed(DigitalOut& ledpin);

int main()
{
   redLED = 1;
   greenLED = 1;
   while (true) {
       rLed(redLED);
       gLed(greenLED);
   }
}