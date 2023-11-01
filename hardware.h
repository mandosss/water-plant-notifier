#include <stdint.h>

void CMU_setup(void);						// configure clock
void GPIO_setup(void);						// configure GPIO ports	
void I2C0_setup(void);						// configure I2C
void USART_setup(void);
void LEUART_setup(void);
void Delay_ms(uint32_t);
void RTC_setup(void);
void Timer0_setup(void);
