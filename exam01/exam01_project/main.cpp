#include "mbed.h"
#include "uLCD_4DGL.h"

uLCD_4DGL uLCD(D1, D0, D2); // serial tx, serial rx, reset pin;
PwmOut PWM1(D6);
//Serial pc( USBTX, USBRX );


//int sample = 128;
//int x;

//float ADCdata[128];

int main()
{
    int i, j;

    //uLCD display
    uLCD.printf("\n107061218\n");
    uLCD.line(30, 30, 30, 60, RED);
    uLCD.line(30, 60, 60, 60, RED);
    uLCD.line(60, 60, 60, 30, RED);
    uLCD.line(60, 30, 30, 30, RED);

    //PWM output
    
    PWM1.period(0.001);
    //PWM1 = 0.8;
    
    while(true){
        for(i=0; i<=1; i+=0.1){
            PWM1 = i;
            wait(0.1);
        }
        for(j=1; j>=0; j-=0.1){
            PWM1 = j;
            wait(0.1);
        }
    }
    /*
    for (i = 0; i < sample; i++){
        ADCdata[i] = PWM1;
        wait(1./sample);
    }
    for (i = 0; i < sample; i++){
        pc.printf("%1.3f\r\n", ADCdata[i]);
        wait(0.1);
    }
    */
}
