#include "si7021.h"
#include "i2c.h"
#include "hardware.h"
volatile int8_t temp;
volatile uint8_t humi;
	
void SI7021_requestConversion()                	// request new temp/humi conversion
{
    uint8_t reqC[] = {0xF5};					// buffer to initiate single temp/humi conversion           
    I2CMasterWriteBuf(SI7021_SLAVE_ADDRESS, reqC, 1);
}

void SI7021_getTempHumi()                      	// read temp/humi data from sensor
{
    uint8_t reqR[2];							// receive buffer for temp/humi 
    I2CMasterReadBuf2(SI7021_SLAVE_ADDRESS, reqR, 2);
    humi = ((((reqR[0] << 8) + reqR[1])*125 + 0x8000) >> 16) + 6;	// compute humi
    if (humi > 99) humi = 99;
		reqR[0] = 0xE0;
		I2CMasterWriteBuf(SI7021_SLAVE_ADDRESS, reqR, 1);
		I2CMasterReadBuf2(SI7021_SLAVE_ADDRESS, reqR, 2);
//    temp = ((((reqR[0] << 8) + reqR[1])*176 + 0x8000) >> 16) - 47;	// compute temp in Celcius
//    temp = 9*temp/5 + 32;                                          // convert temp to Fahrenheit
}    

void SI7021_writeUserReg(uint8_t val)			// write sensor control register 
{
	uint8_t regVal[] = {0xE6, val};
	I2CMasterWriteBuf(SI7021_SLAVE_ADDRESS, regVal, 2);
}

uint8_t SI7021_readUserReg()					// read sensor control register
{
	uint8_t regVal = 0xE7;
	I2CMasterReadBuf2(SI7021_SLAVE_ADDRESS, &regVal, 1);
	return regVal;
}
