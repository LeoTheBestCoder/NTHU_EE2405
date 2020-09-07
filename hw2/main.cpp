#include <stdio.h>
#include "mbed.h"

void print_sine(int);

Serial pc( USBTX, USBRX );
AnalogOut Aout(DAC0_OUT);
AnalogIn Ain(A0);

DigitalIn Switch(SW3);
DigitalOut redLED(LED1);
DigitalOut greenLED(LED2);
BusOut display(D6, D7, D9, D10, D11, D5, D4, D8);

int sample = 1500;
int i;
float ADCdata[1500];

char table[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
char end_table[10] = {0b10111111, 0b10000110, 0b11011011, 0b11001111, 0b11100110, 0b11101101, 0b11111101, 0b10000111, 0b11111111, 0b11101111};


int main(){
    int j, k;
    int count = 2;
    //bool start_count = false;
    int frequency;
    int units_digit, tens_digit, hundreds_digit;
    

    for (i = 0; i < sample; i++){
        Aout = Ain;
        ADCdata[i] = Ain;
        wait(1./sample);
    }
    
    for (i = 0; i < sample; i++){
        pc.printf("%1.3f\r\n", ADCdata[i]);
        //wait(0.1);
    }
    
    for(j = 300; j<sample; j++){
        if(ADCdata[j] == 0){
            /*
            if(count < 10){
                start_count = true;    //the first time it reads 0
            }
            else{
                start_count = false;    //the second time it reads 0
                break;
            }
            */
            for(k=1; k<sample; k++){
                if(ADCdata[j+k] == 0 && ADCdata[j+k] - ADCdata[j+k-1] < 0){
                    goto end;
                }
                else{
                    count++;
                }
            }
        }
        /*
        if(start_count == true){
            count++;
        }
        */
    }

    end:

    frequency = (int)sample/count;
    /*
    pc.printf("A period has %3d data.\r\n", count);
    pc.printf("The frequency is %3d.\r\n", frequency);
    */

    hundreds_digit = (int) frequency / 100;
    tens_digit = (int) (frequency - 100 * hundreds_digit) / 10;
    units_digit = frequency % 10;

    //pc.printf("freq = %d %d %d\r\n", hundreds_digit, tens_digit, units_digit);
    

    // for displaying purpose
    
    
    while(1){
        if(Switch == 0){
            greenLED = 1;
            redLED = 0;
            
            display = table[hundreds_digit];
            wait(1);
            display = table[tens_digit];
            wait(1);
            display = end_table[units_digit];
            wait(1);
            
            print_sine(frequency);
        }
        else{
            redLED = 1;
            greenLED = 0;
            display = 0b00000000;
        }
        
        
    }
    
}

void print_sine(int freq){
    float out;
    for(out=0; out<3; out+=0.0001){
        Aout = 0.5 + 0.5 * sin(2 * 3.14159 * freq * out);
        wait(0.0001);
    }
}