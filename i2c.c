#include <em_device.h>
#include "i2c.h"

void i2c_start(void)							// start I2C session
{
	I2C0->CMD = I2C_CMD_START + I2C_CMD_ABORT;	// generate START
	while (!(I2C0->IF & I2C_IF_START)){}		// wait for the start flag to set
	I2C0->IFC = I2C_IFC_START;					// clear start flag
}

void i2c_rep_start(void)						// repeated start
{
	I2C0->CMD = I2C_CMD_START;
	while (!(I2C0->IF & I2C_IF_START)){}		// wait for the rstart flag to set
	I2C0->IFC = I2C_IFC_START;					// clear restart flag
}

void i2c_stop(void)								// stop I2C session
{
	while (!(I2C0->IF & I2C_IF_TXC)){}			// wait for shift register empty
	I2C0->IFC = I2C_IFC_NACK;	
	I2C0->CMD = I2C_CMD_STOP;
	while (!(I2C0->IF & I2C_IF_MSTOP)){}		// wait for the stop flag to set
	I2C0->IFC = I2C_IFC_MSTOP;					// clear stop flag
}

void i2c_write(uint8_t b)						// send byte to slave
{
	I2C0->TXDATA = b;
	while (!(I2C0->IF & (I2C_IF_NACK+I2C_IF_TXBL+I2C_IF_BUSHOLD))){}	// wait for the TXBL flag to set
	if (I2C0->IF & I2C_IF_NACK)
	{
		I2C0->IFC = I2C_IF_NACK;
		I2C0->CMD = I2C_CMD_CONT;
	}		
}

uint8_t i2c_read(void)							// read byte from slave
{
	uint8_t b;
	while (!(I2C0->IF & I2C_IF_RXDATAV)){}		// wait for byte receive
	b = I2C0->RXDATA;
	I2C0->CMD = I2C_CMD_ACK;					// send ACK	
	return b;									// return received byte		
}

uint8_t i2c_read_last(void)						// read last byte from slave
{
	uint8_t b;
	while (!(I2C0->IF & I2C_IF_RXDATAV)){}		// wait for byte receive
	b = I2C0->RXDATA;
	I2C0->CMD = I2C_CMD_NACK;					// send NACK
	return b;									// return received byte
}

void I2CMasterWriteBuf(uint8_t address, uint8_t buf[], uint8_t len)
{
	uint32_t i;
	i2c_start();
	i2c_write(address<<1);						// write mode
	for (i=0; i<len; i++)
		i2c_write(buf[i]);
	while (!(I2C0->IF & I2C_IF_TXC)){}			// wait for shift register empty
	I2C0->IFC = I2C_IFC_TXC;
	i2c_stop();		
}

void I2CMasterReadBuf(uint8_t address, uint8_t reg, uint8_t buf[], uint8_t len)
{
	uint32_t i;	
	i2c_start();
	i2c_write(address<<1);						// send reg address to read from
	i2c_write(reg);
	while (!(I2C0->IF & I2C_IF_TXC)){}			// wait for transfer end
	I2C0->IFC = I2C_IFC_TXC;

	i2c_rep_start();		
	i2c_write((address<<1) + 1);				// read mode
	for (i=0; i<len-1; i++)
		buf[i] = i2c_read();
	buf[i] = i2c_read_last();
	i2c_stop();
}

void I2CMasterReadBuf2(uint8_t address, uint8_t buf[], uint8_t len)
{
	uint32_t i;	
	i2c_start();
	i2c_write((address<<1) + 1);				// send reg address to read from
	while (!(I2C0->IF & I2C_IF_TXC)){}			// wait for transfer end
	I2C0->IFC = I2C_IFC_TXC;
	
	for (i=0; i<len-1; i++)
		buf[i] = i2c_read();
	buf[i] = i2c_read_last();
	i2c_stop();
}
