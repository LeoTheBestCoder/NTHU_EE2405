#include"mbed.h"

Thread thread1;
Thread thread2;

Serial pc(USBTX,USBRX); //tx,rx
Serial uart(D12,D11); //tx,rx
DigitalIn button(SW2);
EventQueue queue0(32 * EVENTS_EVENT_SIZE);

EventQueue queue(32 * EVENTS_EVENT_SIZE);

void receive_thread(){
   while(1) {
      if(uart.readable()){
            char recv = uart.getc();
            pc.putc(recv);
            // pc.printf("\r\n");
      }
   }
}

void send_thread(){
   while(1){
      if( button == 0){
         char s[21];
         sprintf(s,"find_qrcode");
         uart.puts(s);
         pc.printf("send\r\n");
         wait(0.5);
      }
   }
}

int main(){
   uart.baud(9600);
   thread1.start(callback(&queue0, &EventQueue::dispatch_forever));
   thread2.start(callback(&queue, &EventQueue::dispatch_forever));
   queue0.call(send_thread);
   queue.call(receive_thread);
}