// main.c
// Desenvolvido para a placa EK-TM4C1294XL
// LAB 2 - Controle de Posicao do Servomotor
// Prof. Guilherme Peron

#include <stdint.h>
#include <stdio.h>
#include "tm4c1294ncpdt.h"

void PLL_Init(void);
void SysTick_Init(void);
void SysTick_Wait1ms(uint32_t delay);
void SysTick_Wait1us(uint32_t delay);
void GPIO_Init(void);
void LCD_EnviaComando(uint32_t comando);
void LCD_EnviaDado(uint32_t dado);
void LCD_EnviaString(const char *string);
void LED_Init(void);
void UART_Init(void);
char UART_InChar(void);
void UART_OutChar(char data);
void UART_OutString(char *pt);

int main(void)
{
	char comando;
	PLL_Init();
	SysTick_Init();
	GPIO_Init();
	LED_Init();
	UART_Init();
	
	while(1) {
		comando = UART_InChar(); 

    switch(comando) {
			case '1':
				GPIO_PORTN_DATA_R ^= 0x02; //inverte o estado
        if(GPIO_PORTN_DATA_R & 0x02) 
					UART_OutString("led 1 aceso\r\n");
        else 
          UART_OutString("led 1 apagado\r\n");
			  break;

			case '2':
				GPIO_PORTN_DATA_R ^= 0x01;
        if(GPIO_PORTN_DATA_R & 0x01) 
					UART_OutString("led 2 aceso\r\n");
        else 
          UART_OutString("led 2 apagado\r\n");
        break;

			case '3':
				GPIO_PORTF_AHB_DATA_R ^= 0x10; 
        if(GPIO_PORTF_AHB_DATA_R & 0x10) 
					UART_OutString("led 3 aceso\r\n");
        else 
					UART_OutString("led 3 apagado\r\n");
        break;

      case '4':
				GPIO_PORTF_AHB_DATA_R ^= 0x01; 
        if(GPIO_PORTF_AHB_DATA_R & 0x01) 
					UART_OutString("led 4 aceso\r\n");
        else 
          UART_OutString("led 4 apagado\r\n");
        break;
		}
	}
}

// Lê um caractere vindo do terminal (Bloqueante)
char UART_InChar(void) {
    // Espera até que o flag RXFE (Receive FIFO Empty) seja 0 (ou seja, chegou dado)
    while((UART0_FR_R & 0x0010) != 0);
    return (char)(UART0_DR_R & 0xFF);
}

// Envia um caractere para o terminal
void UART_OutChar(char data) {
    // Espera até que o flag TXFF (Transmit FIFO Full) seja 0 (ou seja, FIFO tem espaço)
    while((UART0_FR_R & 0x0020) != 0);
    UART0_DR_R = data;
}

// Envia uma string completa para o terminal
void UART_OutString(char *pt) {
    while(*pt) {
        UART_OutChar(*pt);
        pt++;
    }
}
