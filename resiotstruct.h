#include "mbed.h"

#ifndef RESIOTSTRUCT_H
#define RESIOTSTRUCT_H
//Struct to send all data in 30 bytes to resiot platform
struct __attribute__((__packed__)) resiotbytes_s{
		uint8_t light;
		uint8_t temp;
		uint8_t hum;
		uint8_t x;
		uint8_t y;
		uint8_t z;
		uint8_t clear;
		uint8_t red;
		uint8_t green;
		uint8_t blue;
		uint8_t sm;
		uint8_t lat [3];
		uint8_t lon [3];
		uint8_t time [3];
		uint8_t nsats;
		uint8_t hdop [3];
		uint8_t alt [3];
		uint8_t geoid [3];
};

#endif