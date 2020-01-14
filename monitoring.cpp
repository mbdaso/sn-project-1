#include "mbed.h"
#include "rtos.h"
#include <stdio.h>
#include "monitoring.h"

sensors_s sensors;
void setupi2c();
Thread threadGPS(osPriorityNormal, 1024);
extern void GPS_thread();
void setupsensors(void){
	//GPS
	threadGPS.start(GPS_thread);
	setupi2c();
}

float read_temperature();
char read_colour();

AnalogIn lightsensor(PA_4); 
AnalogIn soilmois(PA_0); 
extern SerialGPS gpsline;

void readsensors(void){	
	sensors.light = lightsensor*100;
	sensors.sm = soilmois*100;
	sensors.temp = read_temperature();
	sensors.colour = read_colour();
}

void printinfo(){
	printf("\n\rlat,lon: %f, %f \
			\n\rsoil moisture: %f \
			\n\rlight: %f, \
			\n\rsensors.temperature: %f \
			\n\rdominant colour: %c \
			\n\r",
			gpsline.latitude, gpsline.longitude,
			sensors.sm,
			sensors.light,
			sensors.temp,
			sensors.colour);
}

void monitoring(void){
	printf("\n\rmonitoring Thread id: %#x\n\r", Thread::gettid());
	readsensors();
	printinfo();
}
#define SENSOR_ADDR (0x29<<1)
I2C i2c(PB_9,PB_8);
void setupi2c(){
	i2c.frequency(100000);

	char id_regval[1] = {0x92}; //?1001 0010? (bin)
	char data[1] = {0}; //?0000 0000?
	//We obtain device ID from ID register (0x12)
  i2c.write(SENSOR_ADDR,id_regval,1, true);
  i2c.read(SENSOR_ADDR,data,1,false); 
    
	// Timing register address 0x01 (0000 0001). We set 1st bit to 1 -> 1000 0001
	char timing_register[2] = {0x81,0x50}; //0x50 ~ 400ms
	i2c.write(SENSOR_ADDR,timing_register,2,false); 
    
	// Control register address 0x0F (0000 1111). We set 1st bit to 1 -> 1000 1111
	char control_register[2] = {0x8F,0}; //{0x8F, 0x00}, {1000 1111, 0000 0000} -> 1x gain
	i2c.write(SENSOR_ADDR,control_register,2,false);
	
	// Enable register address 0x00 (0000 0000). We set 1st bit to 1 -> 1000 0000
	char enable_register[2] = {0x80,0x03}; //{0x80, 0x03}, {1000 0000, 0000 0011} -> AEN = PON = 1
	i2c.write(SENSOR_ADDR,enable_register,2,false);
}

float read_temperature(void){ 
	char tx_buff[2];
	char rx_buff[2];	
	float temp;
	tx_buff[0] = 0xF3;  // CMD: Measure sensors.temperature, No Hold Master Mode
	i2c.write(0x80, (char*) tx_buff, 1);	// Device ADDR: 0x80 = SI7021 7-bits address shifted one bit left.
	Thread::wait(100);
	i2c.read(0x80, (char*) rx_buff, 2);		// Receive MSB = rx_buff[0], then LSB = rx_buff[1]
	temp = (((rx_buff[0] * 256 + rx_buff[1]) * 175.72) / 65536.0) - 46.85;
	return temp;
}

int max(int a, int b){
	return (a > b) ? a : b;
}

char read_colour(){

	char red_reg[1] = {0x96}; // {?1001 0110?} -> 0x16 and we set 1st bit to 1
	char red_data[2] = {0,0};
	char green_reg[1] = {0x98}; // {?1001 1000?} -> 0x18 and we set 1st bit to 1
	char green_data[2] = {0,0};
	char blue_reg[1] = {0x9A}; // {?1001 1010?} -> 0x1A and we set 1st bit to 1
	char blue_data[2] = {0,0};
	//Reads red value
	i2c.write(SENSOR_ADDR,red_reg,1, true);
	i2c.read(SENSOR_ADDR,red_data,2, false);
		
	int red_value, green_value, blue_value;
	//We store in red_value the concatenation of red_data[1] and red_data[0]
	red_value = ((int)red_data[1] << 8) | red_data[0];
		
	//Reads green value
	i2c.write(SENSOR_ADDR,green_reg,1, true);
	i2c.read(SENSOR_ADDR,green_data,2, false);
		
	//We store in green_value the concatenation of green_data[1] and green_data[0]
	green_value = ((int)green_data[1] << 8) | green_data[0];
		
	//Reads blue value
	i2c.write(SENSOR_ADDR,blue_reg,1, true);
	i2c.read(SENSOR_ADDR,blue_data,2, false);
		
	//We store in blue_value the concatenation of blue_data[1] and blue_data[0]
	blue_value = ((int)blue_data[1] << 8) | blue_data[0];
		
	char colour;
	int maxval;
	//Obtains which one is the greatest - red, green or blue
	maxval = max(max(red_value, green_value), blue_value);
	if(maxval == red_value) colour = 'r';
	if(maxval == green_value) colour = 'g';
	if(maxval == blue_value) colour = 'b';
	
	return colour;
}
