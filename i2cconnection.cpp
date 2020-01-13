//#include "mbed.h"
//#include "rtos.h"
////#include "MMA8451Q.h"
//#include "common.h"

//#define MMA8451_I2C_ADDRESS (0x1d<<1)
//#define SENSOR_ADDR (0x29<<1)

//float hum, temp;

//int clear_value, red_value, green_value, blue_value;
//char dominant_colour;

//bool readColour =  false; //Variable for ISR

//float x, y, z;

//Thread i2c_thread(osPriorityNormal, 1024); // 1K stack size

////ISR code
//void read_colour (void) {
//	readColour =  true;
//}

////Get max value (r,g,b) function
//char getMax(int r, int g, int b) {
//  char result;
//  if (r>g && r>b){ 
//    result = 'r';  
//  } 
//	else if (g>b){
//    result = 'g';
//  }
//  else{
//    result = 'b';
//  }
//  return result;
//}

//void i2c_callback(); 
//DigitalOut ledR(PH_0); //RGB led - red light
//DigitalOut ledG(PH_1);  //RGB led - green light 
//DigitalOut ledB(PB_13);  //RGB led - blue light

//void i2c_callback() {
//  I2C i2c(PB_9,PB_8);
//	i2c.frequency(100000);
//	//Inicializar PWM
//	
//	//MMA8451Q acc(PB_9, PB_8, MMA8451_I2C_ADDRESS);
//	
//	//TEMP/HUM DECLARATIONS
//	char tx_buff[2];
//	char rx_buff[2];
//	
//	//RGB DECLARATIONS
//	DigitalOut whiteLed(PB_7); // TCS34725 led
//	
//	//Variable for ISR
//	bool read_Colour;
//	
//	DigitalOut green(LED1); //LED of B-L072Z-LRWAN1 board
//	Ticker t;
//	
//	t.attach(read_colour, 1.0); // Every second the ticker triggers an interruption
//  green = 1; // LED of B-L072Z-LRWAN1 board on
//    
//	char id_regval[1] = {0x92}; //?1001 0010? (bin)
//	char data[1] = {0}; //?0000 0000?
//	
//	//We obtain device ID from ID register (0x12)
//  i2c.write(SENSOR_ADDR,id_regval,1, true);
//  i2c.read(SENSOR_ADDR,data,1,false); 
//    
//	// Timing register address 0x01 (0000 0001). We set 1st bit to 1 -> 1000 0001
//	char timing_register[2] = {0x81,0x50}; //0x50 ~ 400ms
//	i2c.write(SENSOR_ADDR,timing_register,2,false); 
//    
//	// Control register address 0x0F (0000 1111). We set 1st bit to 1 -> 1000 1111
//	char control_register[2] = {0x8F,0}; //{0x8F, 0x00}, {1000 1111, 0000 0000} -> 1x gain
//	i2c.write(SENSOR_ADDR,control_register,2,false);
//    
//	// Enable register address 0x00 (0000 0000). We set 1st bit to 1 -> 1000 0000
//	char enable_register[2] = {0x80,0x03}; //{0x80, 0x03}, {1000 0000, 0000 0011} -> AEN = PON = 1
//	i2c.write(SENSOR_ADDR,enable_register,2,false);
//    
//  // Read data from color sensor (Clear/Red/Green/Blue)
//	char clear_reg[1] = {0x94}; // {?1001 0100?} -> 0x14 and we set 1st bit to 1
//	char clear_data[2] = {0,0};
//	char red_reg[1] = {0x96}; // {?1001 0110?} -> 0x16 and we set 1st bit to 1
//	char red_data[2] = {0,0};
//	char green_reg[1] = {0x98}; // {?1001 1000?} -> 0x18 and we set 1st bit to 1
//	char green_data[2] = {0,0};
//	char blue_reg[1] = {0x9A}; // {?1001 1010?} -> 0x1A and we set 1st bit to 1
//	char blue_data[2] = {0,0};
//	
//   
//  while (true) {

//    // READ TEMPERATURE MEASURES
//			
//		tx_buff[0] = 0xF3;  // CMD: Measure Temperature, No Hold Master Mode
//				
//    i2c.write(0x80, (char *) tx_buff, 1);	// Device ADDR: 0x80 = SI7021 7-bits address shifted one bit left.
//    Thread::wait(100);
//    i2c.read(0x80, (char*) rx_buff, 2);		// Receive MSB = rx_buff[0], then LSB = rx_buff[1]
//  
//    temp = (((rx_buff[0] * 256 + rx_buff[1]) * 175.72) / 65536.0) - 46.85;	// Conversion based on Datasheet

//    tx_buff[0] = 0xF5;  // CMD: Measure Relative Humidity, No Hold Master Mode
//    
//    i2c.write(0x80, (char *) tx_buff, 1);
//    Thread::wait(100);
//    i2c.read(0x80, (char*) rx_buff, 2);		// Receive MSB first, then LSB
//  
//    hum = (((rx_buff[0] * 256 + rx_buff[1]) * 125.0) / 65536.0) - 6;				// Conversion based on Datasheet
//		
//		//READ RGB MEASURES
//			
//		if (read_Colour){
//			whiteLed = 1;
//			
//			readColour = 0; //readColour = false
//			//Reads clear value
//			i2c.write(SENSOR_ADDR,clear_reg,1, true);
//			Thread::wait(100);
//			i2c.read(SENSOR_ADDR,clear_data,2, false);
//				
//			//We store in clear_value the concatenation of clear_data[1] and clear_data[0]
//			clear_value = ((int)clear_data[1] << 8) | clear_data[0];
//				
//			//Reads red value
//			i2c.write(SENSOR_ADDR,red_reg,1, true);
//			i2c.read(SENSOR_ADDR,red_data,2, false);
//				
//			//We store in red_value the concatenation of red_data[1] and red_data[0]
//			red_value = ((int)red_data[1] << 8) | red_data[0];
//				
//			//Reads green value
//			i2c.write(SENSOR_ADDR,green_reg,1, true);
//			i2c.read(SENSOR_ADDR,green_data,2, false);
//				
//			//We store in green_value the concatenation of green_data[1] and green_data[0]
//			green_value = ((int)green_data[1] << 8) | green_data[0];
//				
//			//Reads blue value
//			i2c.write(SENSOR_ADDR,blue_reg,1, true);
//			i2c.read(SENSOR_ADDR,blue_data,2, false);
//				
//			//We store in blue_value the concatenation of blue_data[1] and blue_data[0]
//			blue_value = ((int)blue_data[1] << 8) | blue_data[0];
//				
//			//Obtains which one is the greatest - red, green or blue
//			dominant_colour = getMax(red_value, green_value, blue_value);
//		
//			//Switchs the color of the greatest value. First, we switch off all of them
//			
//			/*if(current_mode == TEST_MODE){
//				ledR.write(1);
//				ledG.write(1);
//				ledB.write(1);
//				switch (dominant_colour){
//					case 'r':
//						ledR.write(0);
//						break;
//					case 'g':
//						ledG.write(0);
//						break;
//					case 'b':
//						ledB.write(0);
//						break;
//				} 
//			}*/
//			whiteLed = 0;
//		}
//		
//		//READ ACCELEROMETES MEASURES
////		x = acc.getAccX();
////		y = acc.getAccY();
////		z = acc.getAccZ();
//		Thread::wait(200);
//	}
//}  
