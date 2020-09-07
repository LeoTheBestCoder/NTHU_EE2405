#include "mbed.h"
#include "mbed_rpc.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#include <math.h>

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
RawSerial pc(USBTX, USBRX);
RawSerial xbee(D12, D11);

EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread t;
Thread t_for_acc;
EventQueue queue_for_acc(32 * EVENTS_EVENT_SIZE);

int m_addr = FXOS8700CQ_SLAVE_ADDR1;
void xbee_rx_interrupt(void);
void xbee_rx(void);
void reply_messange(char *xbee_reply, char *messange);
void check_addr(char *xbee_reply, char *messenger);
// accelermeter
void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len);
void FXOS8700CQ_writeRegs(uint8_t * data, int len);
void GetData(int unuse);
void send_vel(Arguments *in, Reply *out);
RPCFunction rpcFXOS8700CQ(&send_vel, "send_vel");

int cnt;
float timer = 0.0;
int rpc_cnt = 0;
float x, y, z, theta;
float acc_h, vel_h;
float vel_array[100];
// fill(vel_array, vel_array + 100, 0.0);
int tilt;
float length;
int idx = 0;

int main(){
    for (int i=0; i<100; i++)
        vel_array[i] = 0.0;
    t_for_acc.start(callback(&queue_for_acc, &EventQueue::dispatch_forever));
    queue_for_acc.call_every(100, GetData, 0);
    pc.baud(9600);

    char xbee_reply[4];

    // XBee setting
    xbee.baud(9600);
    xbee.printf("+++");
    xbee_reply[0] = xbee.getc();
    xbee_reply[1] = xbee.getc();
    if(xbee_reply[0] == 'O' && xbee_reply[1] == 'K') {
      	pc.printf("enter AT mode.\r\n");
      	xbee_reply[0] = '\0';
      	xbee_reply[1] = '\0';
    }
    xbee.printf("ATMY 0x136\r\n");
    reply_messange(xbee_reply, "setting MY : 0x136");

    xbee.printf("ATDL 0x236\r\n");
    reply_messange(xbee_reply, "setting DL : 0x236");

    xbee.printf("ATID 0x1\r\n");
    reply_messange(xbee_reply, "setting PAN ID : 0x1");

    xbee.printf("ATWR\r\n");
    reply_messange(xbee_reply, "write config");

    xbee.printf("ATMY\r\n");
    check_addr(xbee_reply, "MY");

    xbee.printf("ATDL\r\n");
    check_addr(xbee_reply, "DL");

    xbee.printf("ATCN\r\n");
    reply_messange(xbee_reply, "exit AT mode");
    xbee.getc();

    // start
    pc.printf("start\r\n");
    t.start(callback(&queue, &EventQueue::dispatch_forever));

    // Setup a serial interrupt function of receiving data from xbee
    xbee.attach(xbee_rx_interrupt, Serial::RxIrq);
}

void send_vel(Arguments *in, Reply *out) {
    pc.printf("RPC Call starts here...\r\n");
    for (int i = 0; i < 100; i++){
        pc.printf("i = \r\n", i);
    }
}

void xbee_rx_interrupt(void)
{
    xbee.attach(NULL, Serial::RxIrq); // detach interrupt
    queue.call(&xbee_rx);
}

void xbee_rx(void)
{
    char buf[100] = {0};
    char outbuf[100] = {0};
    while(xbee.readable()){
      	for (int i=0; ; i++) {
        	char recv = xbee.getc();
      		if (recv == '\r') {
        		break;
      		}
     	    buf[i] = pc.putc(recv);
    	}
    	RPC::call(buf, outbuf);
        // while (1) {
            // cnt++;
            // xbee.printf("%1.4f %1.4f %1.4f %1.4f %d %d\r\n", x, y, z, theta, cnt, rpc_cnt);

            // wait(0.1);
            // timer += 0.1;
            // if (timer >= 1){
            //     timer = 0.0;
            //     break;
            // }
        // }
        cnt = 0;
        rpc_cnt++;
    	// pc.printf("%s\r\n", outbuf);
    	// wait(0.1);
  	}
  	xbee.attach(xbee_rx_interrupt, Serial::RxIrq); // reattach interrupt
}

void reply_messange(char *xbee_reply, char *messange){
  	xbee_reply[0] = xbee.getc();
  	xbee_reply[1] = xbee.getc();
  	xbee_reply[2] = xbee.getc();
  	if(xbee_reply[1] == 'O' && xbee_reply[2] == 'K'){
    	pc.printf("%s\r\n", messange);
    	xbee_reply[0] = '\0';
    	xbee_reply[1] = '\0';
    	xbee_reply[2] = '\0';
  	}
}

void check_addr(char *xbee_reply, char *messenger){
  	xbee_reply[0] = xbee.getc();
  	xbee_reply[1] = xbee.getc();
  	xbee_reply[2] = xbee.getc();
  	xbee_reply[3] = xbee.getc();
  	pc.printf("%s = %c%c%c\r\n", messenger, xbee_reply[1], xbee_reply[2], xbee_reply[3]);
  	xbee_reply[0] = '\0';
  	xbee_reply[1] = '\0';
  	xbee_reply[2] = '\0';
  	xbee_reply[3] = '\0';
}
void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len) {
    char t = addr;
    i2c.write(m_addr, &t, 1, true);
    i2c.read(m_addr, (char *)data, len);
}

void FXOS8700CQ_writeRegs(uint8_t * data, int len) {
    i2c.write(m_addr, (char *)data, len);
}
void GetData(int unuse = 0) {
    if(idx >= 99){
        idx = 0;
    }
    else{
        idx++;
    }
    pc.baud(9600);

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

    // for (int i = 0; i < 2; i++){
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

    x = t[0];
    y = t[1];
    z = t[2];


    length = sqrt(x * x + y * y + z * z);
    theta = acos(z / length) * 180 / PI;
    tilt = 1;
    cnt++;
    acc_h = sqrt(x * x + y * y);
    vel_h = acc_h * 0.1;
    vel_array[idx] = vel_h;
    pc.printf("vel_h[%d] = %1.3f\r\n", idx, vel_h);
}

