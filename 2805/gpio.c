// gpio.c
// Desenvolvido para a placa EK-TM4C1294XL

#include <stdint.h>
#include "tm4c1294ncpdt.h"

#define GPIO_PORTK  (0x0200) //bit 9
#define GPIO_PORTM  (0x0800) //bit 11
#define GPIO_PORTL  (0x0400) //bit 10

void SysTick_Wait1us(uint32_t delay);
void SysTick_Wait1ms(uint32_t delay);
void UART_Init(void);

void GPIO_Init(void)
{
	//1a. Ativar o clock para a porta setando o bit correspondente no registrador RCGCGPIO
	SYSCTL_RCGCGPIO_R = (GPIO_PORTK | GPIO_PORTM | GPIO_PORTL);

	//1b.   após isso verificar no PRGPIO se a porta está pronta para uso.
  while((SYSCTL_PRGPIO_R & 
		(GPIO_PORTK | GPIO_PORTM | GPIO_PORTL) ) != 
		(GPIO_PORTK | GPIO_PORTM | GPIO_PORTL) ){};
	
	// 2. Limpar o AMSEL para desabilitar a analógica
	GPIO_PORTK_AMSEL_R = 0x00;
	GPIO_PORTM_AMSEL_R = 0x00;
	GPIO_PORTL_AMSEL_R = 0x00;
		
	// 3. Limpar PCTL para selecionar o GPIO
	GPIO_PORTK_PCTL_R = 0x00;
	GPIO_PORTM_PCTL_R = 0x00;
	GPIO_PORTL_PCTL_R = 0x00;

	// 4. DIR para 0 se for entrada, 1 se for saída
	GPIO_PORTK_DIR_R = 0xFF; 
	GPIO_PORTM_DIR_R = 0xF7;
	// Saída para motor
	GPIO_PORTL_DIR_R = 0x10;
		
	// 5. Limpar os bits AFSEL para 0 para selecionar GPIO sem função alternativa	
	GPIO_PORTK_AFSEL_R = 0x00;
	GPIO_PORTM_AFSEL_R = 0x00;
	GPIO_PORTL_AFSEL_R = 0x00;
		
	// 6. Setar os bits de DEN para habilitar I/O digital	
	GPIO_PORTK_DEN_R = 0xFF;
	GPIO_PORTM_DEN_R = 0xF7;
	// Habilidando digital para linhas PL0-PL3 e digital do motor PL4
	GPIO_PORTL_DEN_R = 0x1F;
	
	// 7. Habilitar resistor de pull-up interno, setar PUR para 1
	GPIO_PORTL_PUR_R = 0x0F;
}	



void UART_Init(void)
{
    
    SYSCTL_RCGCUART_R |= 0x01;   
    SYSCTL_RCGCGPIO_R |= 0x01;   
    
    while((SYSCTL_PRUART_R & 0x01) == 0) {};
    while((SYSCTL_PRGPIO_R & 0x01) == 0) {};
			
    UART0_CTL_R &= ~0x00000001; 

    UART0_IBRD_R = 86;
    UART0_FBRD_R = 52;
    UART0_LCRH_R = 0x00000070;

    UART0_CC_R = 0;

    GPIO_PORTA_AHB_AMSEL_R &= ~0x03; 
    GPIO_PORTA_AHB_AFSEL_R |= 0x03;
    GPIO_PORTA_AHB_PCTL_R = (GPIO_PORTA_AHB_PCTL_R & 0xFFFFFF00) | 0x00000011;
    GPIO_PORTA_AHB_DEN_R |= 0x03;

    UART0_CTL_R |= 0x00000301;
}

void LED_Init(void) {
    // Ligar clock do Port N (bit 12) e Port F (bit 5)
    SYSCTL_RCGCGPIO_R |= 0x1020; 
    while((SYSCTL_PRGPIO_R & 0x1020) != 0x1020) {};

    // Configurar LED 1 e 2 (PN1 e PN0)
    GPIO_PORTN_DIR_R |= 0x03;
    GPIO_PORTN_DEN_R |= 0x03;

    // Configurar LED 3 e 4 (PF4 e PF0)
    GPIO_PORTF_AHB_DIR_R |= 0x11;
    GPIO_PORTF_AHB_DEN_R |= 0x11;
}

