#include <em_device.h>
#include "hardware.h"
#include "i2c.h"
#include "defines.h"

void CMU_setup(void)									// configure clock
{
	CMU->CTRL |= CMU_CTRL_LFXOTIMEOUT_32KCYCLES;		// set 1 sec LFXO startup time
	CMU->OSCENCMD = CMU_OSCENCMD_HFXOEN +
					CMU_OSCENCMD_LFRCODIS + CMU_OSCENCMD_LFXOEN; // enable HFXO, LFXO
	CMU->CMD = CMU_CMD_HFCLKSEL_HFXO;
	CMU->OSCENCMD |= CMU_OSCENCMD_HFRCODIS;				// disable HFRCO
	CMU->LFCLKSEL = CMU_LFCLKSEL_LFA_LFXO;				// set LFXO as LFA
	CMU->HFCORECLKEN0 = CMU_HFCORECLKEN0_LE + CMU_HFCORECLKEN0_DMA;	// enable LE clock
}

void GPIO_setup(void)									// configure GPIO ports
{
	CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;			// enable HF clock to GPIO
	(GPIO->P[0]).MODEH |= GPIO_P_MODEH_MODE9_PUSHPULL;	// BC_EN at PA.9
	(GPIO->P[0]).DOUTSET = 1<<9;						// enable it
	(GPIO->P[2]).MODEH |= GPIO_P_MODEH_MODE15_INPUT;	// DATA_RECEIVED at PC.15
	(GPIO->P[2]).MODEL |= GPIO_P_MODEL_MODE2_INPUT;		// GPIO3 at PC.2
	GPIO->EXTIPSELL = GPIO_EXTIPSELL_EXTIPSEL2_PORTC;	// set rising edge interrupt
	GPIO->EXTIRISE = 1<<2;								//   at PC.2
	GPIO->IEN = 1<<2;
	NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);				// clear pending GPIO interrupt
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);						// enable GPIO interrupt in NVIC		 
	
	//ADDED NEW BUTTON AND S
	(GPIO->P[2]).MODEH |= GPIO_P_MODEH_MODE10_PUSHPULLDRIVE + GPIO_P_MODEH_MODE8_INPUTPULL;
	(GPIO->P[2]).DOUT = BUTTON0;
}

void RTC_setup(void)									// configure RTC
{
	CMU->LFACLKEN0 |= CMU_LFACLKEN0_RTC;				// set driver clock to RTC
	//RTC->COMP0 = 32767*20;								// 20 sec period setup
	RTC->COMP0 = 32767*5;								// 20 sec period setup
	RTC->CTRL = RTC_CTRL_COMP0TOP + RTC_CTRL_EN;		// start RTC
	RTC->IEN = RTC_IEN_COMP0;							// enable RTC Channel 0 match interrupt
	NVIC_ClearPendingIRQ(RTC_IRQn);						// clear pending LPTRM0 interrupt
	NVIC_EnableIRQ(RTC_IRQn);							// enable LPTMR0 interrupt in NVIC
}

void USART_setup(void)
{
	CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_USART1; 		// enable clock to UART1

	USART1->CLKDIV = 0x30 << 6;							// set 115200 bd @ 24 MHz clock
	USART1->ROUTE = USART_ROUTE_LOCATION_LOC2 +
		USART_ROUTE_TXPEN + USART_ROUTE_RXPEN;			// enable TX/RX pins at PD.7/6
	(GPIO->P[3]).MODEL |= GPIO_P_MODEL_MODE7_PUSHPULL; 	// configure PD.7 for output
	(GPIO->P[3]).MODEL |= GPIO_P_MODEL_MODE6_INPUT; 	// configure PD.6 for input
	USART1->CMD = USART_CMD_TXEN + USART_CMD_RXEN;		// enable TX, RX

	USART1->IEN = USART_IEN_RXDATAV;					// enable RX data valid interrupt
	NVIC_ClearPendingIRQ(USART1_RX_IRQn);				// clear pending USART1 RX interrupt
	NVIC_EnableIRQ(USART1_RX_IRQn);						// enable USART1 RX interrupt in NVIC
}

void LEUART_setup(void)
{
	CMU->LFCLKSEL |= CMU_LFCLKSEL_LFB_LFXO;				// set LFRCO as LFB
	CMU->LFBCLKEN0 |= CMU_LFBCLKEN0_LEUART0;			// enable LFB clock to LEUART
	LEUART0->CLKDIV = 0x4D << 3;						// set 9600bd @ 32768 clock
	LEUART0->ROUTE = LEUART_ROUTE_LOCATION_LOC4 +
		LEUART_ROUTE_TXPEN + LEUART_ROUTE_RXPEN;		// enable TX/RX pins at PF.2/PA.0
	(GPIO->P[5]).MODEL |= GPIO_P_MODEL_MODE2_PUSHPULL; 	// configure PF.2 for output
	(GPIO->P[0]).MODEL |= GPIO_P_MODEL_MODE0_INPUT; 	// configure PA.0 for input
	LEUART0->CMD = LEUART_CMD_TXEN + LEUART_CMD_RXEN;	// enable TX/RX pins

	LEUART0->IEN = LEUART_IEN_RXDATAV;					// enable RX data valid interrupt
	NVIC_ClearPendingIRQ(LEUART0_IRQn);					// clear pending LEART0 interrupt
	NVIC_EnableIRQ(LEUART0_IRQn);						// enable LEUART0 interrupt in NVIC
}

void I2C0_setup(void)
{
	CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_I2C0;			// enable clock to I2C0
	(GPIO->P[4]).MODEH |= GPIO_P_MODEH_MODE12_WIREDAND+	// PE12/13 = SDA/SCL
						  GPIO_P_MODEH_MODE13_WIREDAND;
	I2C0->ROUTE = I2C_ROUTE_LOCATION_LOC6 +				// enable I2C pins at location 6
				  I2C_ROUTE_SDAPEN + I2C_ROUTE_SCLPEN; 	// SDA/SCL = PE12/PE13
	I2C0->CLKDIV = 7;									// set 400 KHz I2C clock (@ PERCLK=32 MHz)
	I2C0->CTRL = I2C_CTRL_EN;							// clear module reset
}

void Timer0_setup(void)
{
	CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_TIMER0; 		// enable clock to timer	
	TIMER0->CTRL = TIMER_CTRL_PRESC_DIV256;				// run timer at 24MHz / 256 = 93,750 KHz
}

void Delay_ms(uint32_t d)
{
	TIMER0->TOP = 94*d;	
	TIMER0->IFC = TIMER_IFC_OF;
	TIMER0->CMD = TIMER_CMD_START;						// start timer0
	while (! ((TIMER0->IF) & TIMER_IF_OF)){}
	TIMER0->CMD = TIMER_CMD_STOP;						// stop timer0
}
