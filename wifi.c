#include "hardware.h"
#include "wifi.h"
#include "uart.h" 

extern volatile char buffer[];
extern volatile int8_t temp;
extern volatile uint8_t humi;

const char req1[] = "GET /update?api_key=5CB4E53RRDCJ17Q5";
const char req2[] = " HTTP/1.1\r\nHost: api.thingspeak.com\r\nConnection: close\r\n\r\n";

void WiFi_setup(void)
{
	USART1_WriteString("Preparing... \r\n");
	do
	{
		CMD_AMW037("get wl n s");						// wait for connection with AP
		Delay_ms(1000);
	} while (buffer[0] != '2');
	USART1_WriteString("Client is ready\r\n");	
}

/*
void HTTP_request(char s)
{
	char write_request[] = "write X NNN";				// write request
	char tdata[] = "&field1=TT";						// temperature data
	char hdata[] = "&field2=HH";						// humidity data
	uint16_t data_length;								// request data length
	
	data_length = sizeof(req1)-1 + sizeof(tdata)-1 + sizeof(hdata)-1 + sizeof(req2)-1;
	write_request[6] = s;	
	write_request[8] = (data_length / 100) + '0';
	write_request[9] = (data_length % 100) / 10 + '0';
	write_request[10] = (data_length % 10) + '0';

	tdata[8] = (temp/10) + '0';							// compose data for server
	tdata[9] = (temp%10) + '0';
	hdata[8] = (humi/10) + '0';
	hdata[9] = (humi%10) + '0';
	
	WriteCR_AMW037(write_request);						// send request command
	Write_AMW037(req1);									// send request data
	Write_AMW037(tdata);
	Write_AMW037(hdata);
	Write_AMW037(req2);	
}

void Close_stream(char s)
{
	char close[] = "close X";
	close[6] = s;
	CMD_AMW037(close);
}
*/
