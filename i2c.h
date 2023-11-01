#include <stdint.h>

void i2c_start(void);			// start I2C session
void i2c_rep_start(void);		// repeated start
void i2c_stop(void);			// stop I2C session
void i2c_write(uint8_t);		// send byte to slave
uint8_t i2c_read(void);			// read byte from slave
uint8_t i2c_read_last(void);	// read last byte from slave
void I2CMasterWriteBuf(uint8_t address, uint8_t buf[], uint8_t len);
void I2CMasterReadBuf(uint8_t address, uint8_t reg, uint8_t buf[], uint8_t len);
void I2CMasterReadBuf2(uint8_t address, uint8_t buf[], uint8_t len);
