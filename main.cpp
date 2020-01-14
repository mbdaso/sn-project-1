/**
 * Copyright (c) 2017, Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
//#include <stdio.h>

#include "lorawan/LoRaWANInterface.h"
#include "lorawan/system/lorawan_data_structures.h"
#include "events/EventQueue.h"
//#include "SerialGPS.h"

// Application helpers
#include "trace_helper.h"
#include "lora_radio_helper.h"
#include "mbed.h"

using namespace events;


// Max payload size can be LORAMAC_PHY_MAXPAYLOAD.
// This example only communicates with much shorter messages (<30 bytes).
// If longer messages are used, these buffers must be changed accordingly.
uint8_t tx_buffer[30];
uint8_t rx_buffer[30];

/*
 * Sets up an application dependent transmission timer in ms. Used only when Duty Cycling is off for testing
 */
#define TX_TIMER                        10000

/**
 * Maximum number of events for the event queue.
 * 10 is the safe number for the stack events, however, if application
 * also uses the queue for whatever purposes, this number should be increased.
 */
#define MAX_NUMBER_OF_EVENTS            10

/**
 * Maximum number of retries for CONFIRMED messages before giving up
 */
#define CONFIRMED_MSG_RETRY_COUNTER     3

/**
 * Dummy pin for dummy sensor
 */
#define PC_9                            0

/**
 * Dummy sensor class object
 */

/**
* This event queue is the global event queue for both the
* application and stack. To conserve memory, the stack is designed to run
* in the same thread as the application and the application is responsible for
* providing an event queue to the stack that will be used for ISR deferment as
* well as application information event queuing.
*/
static EventQueue ev_queue(MAX_NUMBER_OF_EVENTS * EVENTS_EVENT_SIZE);

/**
 * Event handler.
 *
 * This will be passed to the LoRaWAN stack to queue events for the
 * application which in turn drive the application.
 */
static void lora_event_handler(lorawan_event_t event);

/**
 * Constructing Mbed LoRaWANInterface and passing it down the radio object.
 */
static LoRaWANInterface lorawan(radio);

/**
 * Application specific callbacks
 */
static lorawan_app_callbacks_t callbacks;




/**
 * Entry point for application
 */
uint16_t update_tx_buffer();
//#include "common.h"
#include "monitoring.h"
int main (void){
		printf("\n\rmain Thread id: %#x\n\r", Thread::gettid());
		setupsensors();
    // setup tracing
    setup_trace();

    // stores the status of a call to LoRaWAN protocol
    lorawan_status_t retcode;

    // Initialize LoRaWAN stack
    if (lorawan.initialize(&ev_queue) != LORAWAN_STATUS_OK) {
        printf("\r\n LoRa initialization failed! \r\n");
        return -1;
    }

    printf("\r\n Mbed LoRaWANStack initialized \r\n");

    // prepare application callbacks
    callbacks.events = mbed::callback(lora_event_handler);
    lorawan.add_app_callbacks(&callbacks);

    // Set number of retries in case of CONFIRMED messages
    if (lorawan.set_confirmed_msg_retries(CONFIRMED_MSG_RETRY_COUNTER)
                                          != LORAWAN_STATUS_OK) {
        printf("\r\n set_confirmed_msg_retries failed! \r\n\r\n");
        return -1;
    }

    printf("\r\n CONFIRMED message retries : %d \r\n",
           CONFIRMED_MSG_RETRY_COUNTER);

    // Enable adaptive data rate
    if (lorawan.enable_adaptive_datarate() != LORAWAN_STATUS_OK) {
        printf("\r\n enable_adaptive_datarate failed! \r\n");
        return -1;
    }

    printf("\r\n Adaptive data  rate (ADR) - Enabled \r\n");
		
    retcode = lorawan.connect();

    if (retcode == LORAWAN_STATUS_OK ||
        retcode == LORAWAN_STATUS_CONNECT_IN_PROGRESS) {
    } else {
        printf("\r\n Connection error, code = %d \r\n", retcode);
        return -1;
    }

    printf("\r\n Connection - In Progress ...\r\n");
    // make your event queue dispatching events forever
    ev_queue.dispatch_forever();
		
    return 0;
}

/**
 * Sends a message to the Network Server
 */

BusOut ledRGB(PH_0, PH_1, PB_13);
uint16_t update_tx_buffer(){
    uint16_t packet_len;

		readsensors();
		
		float lat1 = gpsline.latitude/100;
		float lon1 = gpsline.longitude/100;
		
		//Translate degrees, minutes, seconds to decimal degrees
		//info: https://www.rapidtables.com/convert/number/degrees-minutes-seconds-to-degrees.html
		int latintpart = lat1;
		int lonintpart = lon1;	
		float latdecimalpart = lat1 - latintpart;
		float londecimalpart = lon1 - lonintpart;	
		lat1 = (float)latintpart + latdecimalpart*100/60;
		lon1 = (float)lonintpart + londecimalpart*100/60;	
    float sm1 = sensors.sm;
    float light1 = sensors.light;
    float temp1 = sensors.temp;
    char colour1 = sensors.colour;

    packet_len = sprintf((char*) tx_buffer, "%+08.4f%+09.4f%04.1f%04.1f%04.1f%c", lat1, lon1, temp1, light1, sm1, colour1);
		//packet_len = sprintf((char*) tx_buffer, "%+08.4f|%+09.4f|%04.1f|%04.1f|%04.1f|%c", lat1, lon1, temp1, light1, sm1, colour1);
		tx_buffer[packet_len] = 0;
		
		return packet_len;
}

static void send_message()
{
    uint16_t packet_len = update_tx_buffer();
    int16_t retcode;
	
		
	
		if(packet_len <= 30){
			retcode = lorawan.send(MBED_CONF_LORA_APP_PORT, tx_buffer, packet_len,
														 MSG_CONFIRMED_FLAG);
		}
		else{
			printf("%s too long - %d bytes", tx_buffer, packet_len);
		}

    if (retcode < 0) {
        retcode == LORAWAN_STATUS_WOULD_BLOCK ? printf("send - WOULD BLOCK\r\n")
                : printf("\r\n send() - Error code %d \r\n", retcode);
        return;
    }

    printf("\r\n %d bytes scheduled for transmission: %s \r\n", retcode, (const char*)tx_buffer);
		memset(tx_buffer, 0, sizeof(tx_buffer));

}

/**
 * Receive a message from the Network Server
 */

#include "mbedstring.h"

bool equal_strings(const char * s1, const char * s2){
	char a = '0', b = '0';
	int i = 0;
	while(a != '\0' && b != '\0'){
		a = s1[i];
		b = s2[i];
		if(a != b)
			return false;
		i++;
	}
	return (a==b);
}
static void receive_message()
{
    int16_t retcode;
    retcode = lorawan.receive(MBED_CONF_LORA_APP_PORT, rx_buffer,
                              sizeof(rx_buffer),
                              MSG_CONFIRMED_FLAG|MSG_UNCONFIRMED_FLAG);

    if (retcode < 0) {
        printf("\r\n receive() - Error code %d \r\n", retcode);
        return;
    }

    printf(" Data: \n");

    for (uint8_t i = 0; i < retcode; i++) {
        printf(" %c %x\n", rx_buffer[i], rx_buffer[i]);
    }
		
		//The available commands must be: "OFF", "Green", and
		//"Red"
		if(equal_strings((const char*)rx_buffer, "OFF")){
			ledRGB = 7; //111
		}
		if(equal_strings((const char*)rx_buffer, "Green")){
			ledRGB = 5; //101
		}
		if(equal_strings((const char*)rx_buffer, "Red")){
			ledRGB = 6; //110
		}

    printf("\r\n Data Length: %d\r\n", retcode);

    memset(rx_buffer, 0, sizeof(rx_buffer));
}

/**
 * Event handler
 */
static void lora_event_handler(lorawan_event_t event)
{
    switch (event) {
        case CONNECTED:
            printf("\r\n Connection - Successful \r\n");
            if (MBED_CONF_LORA_DUTY_CYCLE_ON) {
                send_message();
            } else {
                ev_queue.call_every(TX_TIMER, send_message);
            }

            break;
        case DISCONNECTED:
            ev_queue.break_dispatch();
            printf("\r\n Disconnected Successfully \r\n");
            break;
        case TX_DONE:
            printf("\r\n Message Sent to Network Server \r\n");
            if (MBED_CONF_LORA_DUTY_CYCLE_ON) {
                send_message();
            }
            break;
        case TX_TIMEOUT:
						printf("\n\r TX_TIMEOUT");
        case TX_ERROR:
        case TX_CRYPTO_ERROR:
        case TX_SCHEDULING_ERROR:
            printf("\r\n Transmission Error - EventCode = %d \r\n", event);
            // try again
            if (MBED_CONF_LORA_DUTY_CYCLE_ON) {
                send_message();
            }
            break;
        case RX_DONE:
            printf("\r\n Received message from Network Server \r\n");
            receive_message();
            break;
        case RX_TIMEOUT:
        case RX_ERROR:
            printf("\r\n Error in reception - Code = %d \r\n", event);
            break;
        case JOIN_FAILURE:
            printf("\r\n OTAA Failed - Check Keys \r\n");
            break;
        default:
            MBED_ASSERT("Unknown Event");
    }
}

// EOF
