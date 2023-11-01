#include <stdint.h>

void LEUART0_NewLine(void);
void LEUART0_WriteChar(uint8_t);
void USART1_WriteChar(uint8_t);
void USART1_WriteString(char*);
void USART1_WriteDec(uint32_t);
void USART1_NewLine(void);
void Write_AMW037(const char*);
void WriteCR_AMW037(const char*);
void CMD_AMW037(const char*);
void Get_CMDresponse(void);
