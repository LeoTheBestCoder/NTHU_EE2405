 #include "mbed.h"
 #include "bbcar.h"

 DigitalOut led1(LED1);
 PwmOut pin9(D9), pin8(D8);
 DigitalInOut pin10(D10);
 Ticker servo_ticker;
 Serial pc(USBTX, USBRX);

 BBCar car(pin8, pin9, servo_ticker);

Thread t;
double d;

void print(){
    while (1)
    {
        pc.printf("%3.3f\r\n", d);
        wait(.01);
    }
    
}
 int main() {
     parallax_ping  ping1(pin10);
    //  pc.baud(9600);
    t.start(print);
     car.goStraight(100);
     while(1){
         d = (float) ping1;
         if((float)ping1>10) led1 = 1;
         else{
             led1 = 0;
             car.stop();
             break;
         }
         wait(.01);
     }
 }