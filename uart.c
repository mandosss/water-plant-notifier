#include <em_device.h>
#include "uart.h"
#define BUFFER_SIZE 1000

volatile char buffer[BUFFER_SIZE];
volatile int16_t respLength, respIndex;
volatile int8_t lineCnt, respCode;

void LEUART0_WriteChar(uint8_t c) 				// outputs character c
{
	while (!(LEUART0->IF & LEUART_IF_TXBL)){}	// wait for TDRE bit to set	
	LEUART0->TXDATA = c;						// transmit byte c
}	

void USART1_WriteChar(uint8_t c) 				// outputs character c
{
	while (!(USART1->IF & USART_IF_TXBL)){}		// wait for TDRE bit to set	
	USART1->TXDATA = c;							// transmit byte c
}	

void USART1_WriteString(char *s)
{
	int8_t i;
	for (i=0; s[i]; i++)
		USART1_WriteChar(s[i]);
}

void USART1_NewLine() 							// start new line
{
	USART1_WriteChar(13);						// CR signal
	USART1_WriteChar(10);						// LF signal
}

void USART1_WriteDec(uint32_t d)				// outputs 32-bit unsigned number
{
	uint8_t rem[8];								// storage for digits
	int32_t i = 0;
	if (d == 0)
		USART1_WriteChar('0');					// handle special case d = 0
	else
	{
		do										// loop over all digits
		{
			rem[i++] = d % 10;					// compute digit
			d = d / 10;
		} while (d != 0);						// supress leading 0s
		for (i=i-1; i>=0; i--)					// print digits
			USART1_WriteChar(rem[i] + '0');
	}
}

void Write_AMW037(const char* s)
{
	uint8_t i;
	for (i=0; s[i]; i++)						// echo command to the terminal
		USART1_WriteChar(s[i]);	
	for (i=0; s[i]; i++)						// send command to AMW037
		LEUART0_WriteChar(s[i]);
}	

void WriteCR_AMW037(const char* s)
{
	uint8_t i;
	for (i=0; s[i]; i++)						// echo command to the terminal
		USART1_WriteChar(s[i]);
	USART1_NewLine();							// send CR + LF	
	for (i=0; s[i]; i++)						// send every string char
		LEUART0_WriteChar(s[i]);	
	LEUART0_WriteChar(13);						// send CR + LF
	LEUART0_WriteChar(10);
}

void CMD_AMW037(const char* s)					// send command to AMW037 and get response
{
	WriteCR_AMW037(s);	
	//Get_CMDresponse();
}

/*
void Get_CMDresponse(void)
{
	respLength = 0;
	respIndex = 0;								// reset response buffer index
	lineCnt = 0;								// read that many response lines	
	SCB->SCR = SCB_SCR_SLEEPONEXIT_Msk;			// prepare to read module response	
	__WFI();									// wait for module response	
}

*/
		
void LEUART0_IRQHandler(void)
{	
	char c = LEUART0->RXDATA;					// get received char from AMW037
	while (!(USART1->IF & USART_IF_TXBL)){}		// wait for TDRE bit to set	
	USART1->TXDATA = c;							// echo it to the terminal

	if (respIndex < BUFFER_SIZE)				// buffer the WiFi module response
		buffer[respIndex] = c;
	respIndex++;
	
	if (c == 10)								// end of line 
	{
		lineCnt++;								// update line counter
		if (lineCnt == 1)						// get response length
		{
			respIndex = 0;						// 
			respCode = buffer[1];				// AMW037 response code
			respLength = buffer[2]*10000 + buffer[3]*1000 + buffer[4]*100 +
						 buffer[5]*10 + buffer[6] - ('0'*11111);	// compute response length
		}
	}		
	
	if (respIndex == respLength)
		SCB->SCR = 0;							// cancel sleep-on-exit
}

void USART1_RX_IRQHandler(void)
{
	LEUART0->TXDATA = USART1->RXDATA;			// get char from terminal and forward it to AMW037
}
