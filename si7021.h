#include <stdint.h>
#define SI7021_SLAVE_ADDRESS       	0x40    // sensor slave address

void SI7021_getTempHumi(void);         		// function prototypes
void SI7021_requestConversion(void);
void SI7021_writeUserReg(uint8_t);
uint8_t SI7021_readUserReg(void);

