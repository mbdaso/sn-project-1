#include "mbed.h"
#include "rtos.h"
#include "SerialGPS.h"
SerialGPS gpsline(PA_9, PA_10, 9600);
DigitalOut led2(LED2);
//Thread threadGPS;
void GPS_thread(); 
void GPS_thread() {
	printf("\ngps Thread id: %#x\n\r", Thread::gettid());

	while (true) {
			led2 = !led2;
			gpsline.sample();
			Thread::wait(1000);
	}
}
