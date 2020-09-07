#include "mbed.h"
#include "bbcar.h"

#define LED_ON 0
#define LED_OFF 1


DigitalOut led1(LED1);  // red led
DigitalOut led2(LED2);  // green led
PwmOut pin9(D9), pin8(D8);
RawSerial xbee(D12, D11);
DigitalInOut pin10(D10);
Ticker servo_ticker;

Serial uart(D1, D0);
Serial pc(USBTX, USBRX);
BBCar car(pin8, pin9, servo_ticker);

float shape[50];


int main() {
    pc.baud(9600);
    parallax_ping  ping1(pin10);

    // for (int i = 0; i < 200; i++) {
    //     shape[i] = 0.0;
    // }

    pc.printf("the adventure starts...\r\n");
    xbee.printf("the adventure starts...\r\n");
    car.goStraight(100);
    led1 = LED_OFF;
    led2 = LED_ON;
    wait(8.7);
    // while(1){
    //     pc.printf("%1.4f\r\n", (float)ping1);

    //     if((float)ping1>20) {
    //         led1 = LED_OFF;
    //         led2 = LED_ON;
    //     }
    //     else{
    //         led1 = LED_ON;
    //         led2 = LED_OFF;
    //         car.stop();
    //         break;
    //     }
    //     wait(.01);
    // }
    car.stop();
    led1 = LED_ON;
    led2 = LED_OFF;
    wait(1.0);

    pc.printf("start to turn left...\r\n");
    xbee.printf("start to turn left...\r\n");
    car.turn(-60, 0.5);
    led1 = LED_OFF;
    led2 = LED_ON;
    wait_us(1000000);

    car.stop();
    led1 = LED_ON;
    led2 = LED_OFF;
    wait(1.0);

    pc.printf("continue to go straight...\r\n");
    xbee.printf("continue to go straight...\r\n");
    car.goStraight(100);
    led1 = LED_OFF;
    led2 = LED_ON;
    wait_us(5500000);

    car.stop();
    led1 = LED_ON;
    led2 = LED_OFF;
    wait(1.0);

    // while(1){
    //     pc.printf("%1.4f\r\n", (float)ping1);

    //     if((float)ping1>30) {
    //         led1 = LED_OFF;
    //         led2 = LED_ON;
    //     }
    //     else{
    //         led1 = LED_ON;
    //         led2 = LED_OFF;
    //         car.stop();
    //         break;
    //     }
    //     wait(.01);
    // }

    // while(1) {
    //     pc.printf("%1.4f\r\n", (float)ping1);
    //     led1 = LED_OFF;
    //     led2 = LED_ON;
        
    //     car.turn(60, 0.5);
    //     wait(1.1);
    //     car.stop();
    //     break;
    // }
    pc.printf("start to turn right a lot...\r\n");
    xbee.printf("start to turn right a lot...\r\n");
    car.turn(60, 0.5);
    led1 = LED_OFF;
    led2 = LED_ON;
    wait_us(1100000);

    car.stop();
    led1 = LED_ON;
    led2 = LED_OFF;
    wait(1.0);

    car.goBack(100);
    led1 = LED_OFF;
    led2 = LED_ON;
    wait_us(400000);

    car.stop();
    led1 = LED_ON;
    led2 = LED_OFF;
    wait(1.0);

    // take a picture here
    pc.printf("start to take a picture...\r\n");
    xbee.printf("start to take a picture...\r\n");
    char s[21];
    sprintf(s, "image_classification");
    uart.puts(s);
    wait(2.0);

    pc.printf("start to send the number on the picture...\r\n");
    xbee.printf("start to send the number on the picture...\r\n");



//     waiting for repaire
    while(1){
        if(uart.readable()){
            char recv = uart.getc();
            pc.putc(recv);
            xbee.printf("the result of the number: ");
            xbee.putc(recv);
            xbee.printf("\r\n");
            break;
        }
   }


    car.stop();
    led1 = LED_ON;
    led2 = LED_OFF;
    wait(1.0);

    pc.printf("start to turn left a little bit\r\n");
    xbee.printf("start to turn left a little bit\r\n");
    car.turn(-45, 0.5);
    led1 = LED_OFF;
    led2 = LED_ON;
    wait_us(450000);

    car.stop();
    led1 = LED_ON;
    led2 = LED_OFF;
    wait(1.0);

    pc.printf("start to move backward...\r\n");
    xbee.printf("start to move backward...\r\n");
    car.goBack(100);
    wait_us(1400000);

    car.stop();
    led1 = LED_ON;
    led2 = LED_OFF;
    wait(1.0);

    pc.printf("start to go out of the garage...\r\n");
    xbee.printf("start to go out of the garage...\r\n");
    car.goStraight(100);
    led1 = LED_OFF;
    led2 = LED_ON;
    wait_us(2000000);

    car.stop();
    led1 = LED_ON;
    led2 = LED_OFF;
    wait(1.0);


    pc.printf("start to turn right...\r\n");
    xbee.printf("start to turn right...\r\n");
    car.turn(60, 0.5);
    led1 = LED_OFF;
    led2 = LED_ON;
    wait_us(1000000);

    car.stop();
    led1 = LED_ON;
    led2 = LED_OFF;
    wait(1.0);

    pc.printf("start to go straight...\r\n");
    xbee.printf("start to go straight...\r\n");
    car.goStraight(100);
    led1 = LED_OFF;
    led2 = LED_ON;
    wait_us(4200000);

    car.stop();
    led1 = LED_ON;
    led2 = LED_OFF;
    wait(1.0);

    pc.printf("start to turn right\r\n");
    xbee.printf("start to turn right\r\n");
    car.turn(60, 0.5);
    led1 = LED_OFF;
    led2 = LED_ON;
    wait_us(1000000);

    car.stop();
    led1 = LED_ON;
    led2 = LED_OFF;
    wait(1.0);

    pc.printf("start to go straight...\r\n");
    xbee.printf("start to go straight...\r\n");
    car.goStraight(100);
    led1 = LED_OFF;
    led2 = LED_ON;
    wait_us(8200000);

    car.stop();
    led1 = LED_ON;
    led2 = LED_OFF;
    wait(1.0);

    pc.printf("start to turn right...\r\n");
    xbee.printf("start to turn right...\r\n");
    car.turn(70, 0.5);
    led1 = LED_OFF;
    led2 = LED_ON;
    wait_us(800000);

    car.stop();
    led1 = LED_ON;
    led2 = LED_OFF;
    wait(1.0);

    // pc.printf("start to go straight...\r\n");
    // xbee.printf("start to go straight...\r\n");
    // car.goStraight(100);
    // led1 = LED_OFF;
    // led2 = LED_ON;
    // wait_us(3500000);

    // car.stop();
    // led1 = LED_ON;
    // led2 = LED_OFF;
    // wait(1.0);



    xbee.printf("go straight a little bit...\r\n");
    car.goStraight(100);
    led1 = LED_OFF;
    led2 = LED_ON;
    wait_us(2000000);

    car.stop();
    led1 = LED_ON;
    led2 = LED_OFF;
    wait(1.0);

    pc.printf("start to turn right...\r\n");
    xbee.printf("start to turn right...\r\n");
    car.turn(60, 0.5);
    led1 = LED_OFF;
    led2 = LED_ON;
    wait_us(600000);

    car.stop();
    led1 = LED_ON;
    led2 = LED_OFF;
    wait(1.0);

    xbee.printf("start to get close to the shape...\r\n");
    car.goStraight(100);
    led1 = LED_OFF;
    led2 - LED_ON;
    wait_us(3000000);

    car.stop();
    led1 = LED_ON;
    led2 = LED_OFF;
    wait(1.0);

    // car.turn(100, 0.5);
    // led1 = LED_OFF;
    // led2 = LED_ON;
    // wait_us(500000);

    pc.printf("start to scan the shape...\r\n");
    xbee.printf("start to scan the shape...\r\n");
    car.turn(-40, 0.5);
    led1 = LED_OFF;
    led2 = LED_ON;
    for (int i = 0; i < 30; i++) {
        shape[i] = (float) ping1;
        wait_us(20000);
    }


    
    
    car.stop();
    led1 = LED_ON;
    led2 = LED_OFF;
    wait(1.0);

    xbee.printf("the shape is\r\n");
    if (shape[25] > shape[2] && shape[25] > shape[48]) {
        xbee.printf("***********\r\n");
        xbee.printf("***** *****\r\n");
        xbee.printf("****   ****\r\n");
        xbee.printf("***     ***\r\n");
        xbee.printf("**       **\r\n");
        xbee.printf("*         *\r\n");
    }
    else if (shape[2] < shape[25] && shape[2] < shape[48]){
        xbee.printf("***********\r\n");
        xbee.printf("  *********\r\n");
        xbee.printf("    *******\r\n");
        xbee.printf("      *****\r\n");
        xbee.printf("        ***\r\n");
        xbee.printf("          *\r\n");
    }
    else if (abs(shape[2] - shape[25]) <= 2.0 && abs(shape[25] - shape[48] <= 2.0)) {
        xbee.printf("***********\r\n");
        xbee.printf("***********\r\n");
        xbee.printf("***********\r\n");
        xbee.printf("***********\r\n");
        xbee.printf("***********\r\n");
        xbee.printf("***********\r\n");
    }
    else {
        xbee.printf("***********\r\n");
        xbee.printf("********* \r\n");
        xbee.printf(" *******  \r\n");
        xbee.printf("  *****   \r\n");
        xbee.printf("   ***    \r\n");
        xbee.printf("    *     \r\n");
}

    xbee.printf("turn left a little...\r\n");
    car.turn(-60, 0.5);
    led1 = LED_OFF;
    led2 = LED_ON;
    wait_us(700000);

    car.stop();
    led1 = LED_ON;
    led2 = LED_OFF;
    wait(1.0);
    

    pc.printf("start to move back to the road...\r\n");
    xbee.printf("start to move back to the road...\r\n");
    car.goStraight(100);
    led1 = LED_OFF;
    led2 = LED_ON;
    wait_us(2000000);

    car.stop();
    led1 = LED_ON;
    led2 = LED_OFF;
    wait(1.0);


    pc.printf("start to turn right...\r\n");
    xbee.printf("start to turn right...\r\n");
    car.turn(60, 0.5);
    led1 = LED_OFF;
    led2 = LED_ON;
    wait_us(1000000);

    car.stop();
    led1 = LED_ON;
    led2 = LED_OFF;
    wait(1.0);

    pc.printf("start to go straight...\r\n");
    xbee.printf("start to go straight...\r\n");
    car.goStraight(100);
    led1 = LED_OFF;
    led2 = LED_ON;
    wait_us(2000000);

    car.stop();
    led1 = LED_ON;
    led2 = LED_OFF;
    wait(1.0);

    pc.printf("start to turn right...\r\n");
    xbee.printf("start to turn right...\r\n");
    car.turn(80, 0.5);
    led1 = LED_OFF;
    led2 = LED_ON;
    wait_us(900000);

    car.stop();
    led1 = LED_ON;
    led2 = LED_OFF;
    wait(1.0);

    pc.printf("start to go out...\r\n");
    xbee.printf("start to go out...\r\n");
    car.goStraight(100);
    led1 = LED_OFF;
    led2 = LED_ON;
    wait(20);


}