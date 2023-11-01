/* 
	Author:		Armando Ramirez
	Project: 	Humidity Notifier 
	Description:	This project aims to take humidity readings from a plant. 
		If the moisutre is lower than a certain value, it sends an http post request
		to a server (Node.js) which sends a custom email to a recipient to notify.
	Bugs:					The external humidity sensor was not configured to work with this project.
		Future changes will be made to work properly.
*/

#include <em_device.h>									// header for all micro registers
#include "hardware.h"									// custom headers
#include "uart.h"
#include "si7021.h"
#include "wifi.h"
#include "defines.h"

extern volatile uint8_t humi;
extern char buffer[];									// AMW037 response buffer
char stream;
uint8_t connect;
uint8_t isSend = 0;
uint8_t closeHTTP = 0;
uint8_t isHTTPClosed = 0;
char tempHumi[10] = "";


int main()
{
	CMU_setup();										// configure clock
	GPIO_setup();										// configure GPIO ports	
	I2C0_setup();
	USART_setup();
	LEUART_setup();
	Timer0_setup();
	WiFi_setup();
	RTC_setup();
	
	while(1)											// main application loop
	{
		__WFI();										// wait for connect time	
		if (!connect) continue;
		SI7021_requestConversion();					// request new conversion		
		Delay_ms(500);									// wait for conversion end
		SI7021_getTempHumi();							// read temperature from sensor		

		//debugging purposes
		tempHumi[0] = humi/10 + '0';
		tempHumi[1] = humi%10 + '0';
		tempHumi[2] = '\r'; 
		tempHumi[3] = '\n';
		
		USART1_WriteString(tempHumi);
		//this if statement will send the request to the server
		// and also it will turn on the LED0 to notify the user
		if (!isSend && connect && humi < 35)							// connect if returned value is 0
		{
			Delay_ms(50);
			CMD_AMW037("hpo 3.86.33.250/apinode/mail/send/SECRETKEY");	// get request stream number
			Delay_ms(500);
			(GPIO->P[2]).DOUTSET = LED0;			//turn on LED0 light
			isSend = 1;
			closeHTTP = 1;
		}		
		
		//the message is sent and the plant has been watered, 
		// then we need to reset the microcontroller by pressing BUTTON0.
		// this will will turn off the LED0 and also close the http stream,
		// reset the message for next time
		if(isSend && closeHTTP && !((GPIO->P[2]).DIN & BUTTON0))
		{
			(GPIO->P[2]).DOUTCLR = LED0;			//turn on LED0 light
			Delay_ms(500);									// wait for conversion end
			CMD_AMW037("close http/https");						// close stream
			Delay_ms(500);
			closeHTTP = 0;
			isSend = 0;
		}
		connect = 0;									// reset software flag
	}
}

void RTC_IRQHandler(void)
{
	RTC->IFC = RTC_IFC_COMP0;							// clear RTC IF
	connect = 1;										// set software flag
}
