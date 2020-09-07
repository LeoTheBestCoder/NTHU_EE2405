#include "mbed.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include "math.h"
#include "algorithm"

#define UINT14_MAX        16383
// FXOS8700CQ I2C address
#define FXOS8700CQ_SLAVE_ADDR0 (0x1E<<1) // with pins SA0=0, SA1=0
#define FXOS8700CQ_SLAVE_ADDR1 (0x1D<<1) // with pins SA0=1, SA1=0
#define FXOS8700CQ_SLAVE_ADDR2 (0x1C<<1) // with pins SA0=0, SA1=1
#define FXOS8700CQ_SLAVE_ADDR3 (0x1F<<1) // with pins SA0=1, SA1=1
// FXOS8700CQ internal register addresses
#define FXOS8700Q_STATUS 0x00
#define FXOS8700Q_OUT_X_MSB 0x01
#define FXOS8700Q_OUT_Y_MSB 0x03
#define FXOS8700Q_OUT_Z_MSB 0x05
#define FXOS8700Q_M_OUT_X_MSB 0x33
#define FXOS8700Q_M_OUT_Y_MSB 0x35
#define FXOS8700Q_M_OUT_Z_MSB 0x37
#define FXOS8700Q_WHOAMI 0x0D
#define FXOS8700Q_XYZ_DATA_CFG 0x0E
#define FXOS8700Q_CTRL_REG1 0x2A
#define FXOS8700Q_M_CTRL_REG1 0x5B
#define FXOS8700Q_M_CTRL_REG2 0x5C
#define FXOS8700Q_WHOAMI_VAL 0xC7

#define PI 3.1416

I2C i2c( PTD9,PTD8);
Serial pc(USBTX, USBRX);
InterruptIn sw2(SW2);
DigitalOut led3(LED3);
// uLCD_4DGL uLCD(D1, D0, D2); // serial tx, serial rx, reset pin;
int m_addr = FXOS8700CQ_SLAVE_ADDR1;

void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len);
void FXOS8700CQ_writeRegs(uint8_t * data, int len);

Thread t;
Thread led_thread;
Thread log_thread;



float x_data[100];
float y_data[100];
float z_data[100];
float acc_data[100];
// int tilt_data[100];
int over_five[100];
int idx = 0;
float displacement = 0.0;   // in the unit of cm

// fill(x_data, x_data + 100, 0.0);
// fill(y_data, y_data + 100, 0.0);
// fill(z_data, z_data + 100, 0.0);
// fill(tilt_data, tilt_data + 100, 0);


void logger(int unuse){
   float theta, length;
   uint8_t who_am_i, data[2], res[6];
   int16_t acc16;
   float t[3];
   // Enable the FXOS8700Q

   FXOS8700CQ_readRegs( FXOS8700Q_CTRL_REG1, &data[1], 1);
   data[1] |= 0x01;
   data[0] = FXOS8700Q_CTRL_REG1;
   FXOS8700CQ_writeRegs(data, 2);

   // Get the slave address
   FXOS8700CQ_readRegs(FXOS8700Q_WHOAMI, &who_am_i, 1);

   // pc.printf("Here is %x\r\n", who_am_i);
   // while (true) {
      FXOS8700CQ_readRegs(FXOS8700Q_OUT_X_MSB, res, 6);

      acc16 = (res[0] << 6) | (res[1] >> 2);
      if (acc16 > UINT14_MAX/2)
         acc16 -= UINT14_MAX;
      t[0] = ((float)acc16) / 4096.0f;

      acc16 = (res[2] << 6) | (res[3] >> 2);
      if (acc16 > UINT14_MAX/2)
         acc16 -= UINT14_MAX;
      t[1] = ((float)acc16) / 4096.0f;

      acc16 = (res[4] << 6) | (res[5] >> 2);
      if (acc16 > UINT14_MAX/2)
         acc16 -= UINT14_MAX;
      t[2] = ((float)acc16) / 4096.0f;
      /*
      printf("FXOS8700Q ACC: X=%1.4f(%x%x) Y=%1.4f(%x%x) Z=%1.4f(%x%x)\r\n",\
            t[0], res[0], res[1],\
            t[1], res[2], res[3],\
            t[2], res[4], res[5]\
      );
      */
      float x = t[0], y = t[1], z = t[2];
      // wait(1.0);
      // pc.printf("x[%d] = %1.4f, y[%d] = %1.4f, z[%d] = %1.4f\r\n", idx, x, idx, y, idx, z);

    length = sqrt(x * x + y * y + z * z);
    //   theta = acos(z / length) * 180 / PI;
      x_data[idx] = x;
      y_data[idx] = y;
      z_data[idx] = z;
    //   if(theta > 45) tilt_data[idx] = 1;
    //   else tilt_data[idx] = 0;
      float actual_vector_scalar = sqrt(x * x + y * y);
      float actual_vector;
      if(x * y > 0){
         actual_vector = actual_vector_scalar;
      }
      else{
         actual_vector = -actual_vector_scalar;
      }
      float actual_acc = 9.8 * actual_vector;
      float actual_displacement = actual_vector * (0.1 * 0.1 / 2);
      acc_data[idx] = actual_displacement;
      displacement += actual_displacement;
      if(displacement > 0.05){
         over_five[idx] = 1;
      }
      else{
         over_five[idx] = 0;
      }

      idx++;
      
   // }
}


void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len) {
   char t = addr;
   i2c.write(m_addr, &t, 1, true);
   i2c.read(m_addr, (char *)data, len);
}

void FXOS8700CQ_writeRegs(uint8_t * data, int len) {
   i2c.write(m_addr, (char *)data, len);
}

void Trig_led3(int unuse){
   led3 = !led3;
}

EventQueue queue(32 * EVENTS_EVENT_SIZE);
EventQueue queue2(32 * EVENTS_EVENT_SIZE);
EventQueue queue3(32 * EVENTS_EVENT_SIZE);

void blink_log(){
   led_thread.start(callback(&queue2, &EventQueue::dispatch_forever));
   log_thread.start(callback(&queue3, &EventQueue::dispatch_forever));
   queue2.call_every(1000, Trig_led3, 0);
   queue3.call_every(100, logger, 0);
   wait(10);
   led_thread.terminate();
   log_thread.terminate();
   for(int i=0; i<100; i++){
      // pc.printf("%1.4f %1.4f %1.4f %d\r\n", x_data[i], y_data[i], z_data[i], over_five[i]);
      pc.printf("%1.4f %d\r\n", acc_data[i], over_five[i]);
   }
}

int main() {   
   pc.baud(115200);
   t.start(callback(&queue, &EventQueue::dispatch_forever));
   sw2.rise(queue.event(blink_log));
}