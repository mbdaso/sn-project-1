#ifndef MONITORING_H 
#define MONITORING_H

#include "SerialGPS.h"
#include "rtos.h"
/**/

//float valueLight;
//float valueSM;
//float read_temperature();
//char dominant_colour;
//float temp;

struct sensors_s{
	float light;
	float sm;
	char colour;
	float temp;
	float lat;
	float lon;
};

extern sensors_s sensors;
extern SerialGPS gpsline;

void setupsensors(void);
void readsensors(void);

void printinfo();
#endif
