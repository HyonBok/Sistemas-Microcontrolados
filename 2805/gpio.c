// gpio.c
// Desenvolvido para a placa EK-TM4C1294XL

#include <stdint.h>
#include "tm4c1294ncpdt.h"

#define GPIO_PORTK  (0x0200) //bit 9
#define GPIO_PORTM  (0x0800) //bit 11
#define GPIO_PORTL  (0x0400) //bit 10
#define GPIO_PORTJ  (0x0100)
#define GPIO_PORTE  (0x0010)
#define GPIO_PORTH  (0x0080)

void SysTick_Wait1us(uint32_t delay);
void SysTick_Wait1ms(uint32_t delay);
void UART_Init(void);

extern volatile uint8_t flag_timer_200ms;

// configura os pinos de UART (PA), ADC (PE4) e motor (PH0-PH3)
void GPIO_Init(void)
{
	// liga o clock
	SYSCTL_RCGCGPIO_R |= (GPIO_PORTK | GPIO_PORTM | GPIO_PORTL | GPIO_PORTJ | GPIO_PORTE | GPIO_PORTH);
	while((SYSCTL_PRGPIO_R & (GPIO_PORTK | GPIO_PORTM | GPIO_PORTL | GPIO_PORTJ | GPIO_PORTE | GPIO_PORTH)) == 0){};
		
	
	GPIO_PORTH_AHB_AMSEL_R &= ~0x0F;		// desabilita função analógica
  GPIO_PORTH_AHB_AFSEL_R &= ~0x0F;    // desabilita função alternativa
  GPIO_PORTH_AHB_PCTL_R  &= ~0xFFFF;  // limpa PCTL
  GPIO_PORTH_AHB_DIR_R   |= 0x0F;     // configura PH0 a PH3 como saída
  GPIO_PORTH_AHB_DEN_R   |= 0x0F;     // habilita I/O digital
	
	GPIO_PORTK_AMSEL_R = 0x00;
	GPIO_PORTM_AMSEL_R = 0x00;
	GPIO_PORTL_AMSEL_R = 0x00;
	GPIO_PORTK_PCTL_R = 0x00;
	GPIO_PORTM_PCTL_R = 0x00;
	GPIO_PORTL_PCTL_R = 0x00;

	GPIO_PORTK_DIR_R = 0xFF; 
	GPIO_PORTM_DIR_R = 0xF7;
	GPIO_PORTL_DIR_R = 0x10;
		
	GPIO_PORTK_AFSEL_R = 0x00;
	GPIO_PORTM_AFSEL_R = 0x00;
	GPIO_PORTL_AFSEL_R = 0x00;

	GPIO_PORTK_DEN_R = 0xFF;
	GPIO_PORTM_DEN_R = 0xF7;
	GPIO_PORTL_DEN_R = 0x1F;
	
	GPIO_PORTL_PUR_R = 0x0F;
	
	GPIO_PORTE_AHB_AMSEL_R &= ~0x0F;
	GPIO_PORTE_AHB_AFSEL_R &= ~0x0F;
	GPIO_PORTE_AHB_PCTL_R  &= ~0x0000FFFF;
	GPIO_PORTE_AHB_DIR_R   |=  0x0F;
	GPIO_PORTE_AHB_DEN_R   |=  0x0F;
	GPIO_PORTE_AHB_AMSEL_R |= 0x10;   
	GPIO_PORTE_AHB_AFSEL_R |= 0x10;   
	GPIO_PORTE_AHB_DEN_R   &= ~0x10; 
}	

// inicializa UART0 a 57600 baud, paridade impar, 1 stop bit, 8 bits de dados
void UART_Init(void)
{
	SYSCTL_RCGCUART_R |= 0x01;   
  SYSCTL_RCGCGPIO_R |= 0x01;   
    
  while((SYSCTL_PRUART_R & 0x01) == 0) {};
  while((SYSCTL_PRGPIO_R & 0x01) == 0) {};
			
  UART0_CTL_R &= ~0x00000001; 
 
  UART0_IBRD_R = 17; // valor inteiro para 57600 
  UART0_FBRD_R = 23; // valor fracionário para 57600
  UART0_LCRH_R = 0x0000006A; // paridade ímpar com 1 stop bit

  UART0_CC_R = 0;
  GPIO_PORTA_AHB_AMSEL_R &= ~0x03; 
  GPIO_PORTA_AHB_AFSEL_R |= 0x03;
  GPIO_PORTA_AHB_PCTL_R = (GPIO_PORTA_AHB_PCTL_R & 0xFFFFFF00) | 0x00000011;
  GPIO_PORTA_AHB_DEN_R |= 0x03;

  UART0_CTL_R |= 0x00000301;
}

// configura ADC0 SS3 para leitura do potenciometro no canal AIN9 (PE4) por software
void ADC_Init(void)
{
	SYSCTL_RCGCADC_R |= 0x01; 
  while((SYSCTL_PRADC_R & 0x01) == 0) {};

  ADC0_PC_R = 0x07; 
	ADC0_SSPRI_R = 0x0123; 
	ADC0_ACTSS_R &= ~0x0008; 
  ADC0_EMUX_R &= ~0xF000; 
  ADC0_SSMUX3_R = (ADC0_SSMUX3_R & ~0x000F) | 0x0009; 
  ADC0_SSCTL3_R = 0x0006; 
	ADC0_ACTSS_R |= 0x0008; 
}

// dispara uma conversao AD e retorna o resultado de 12 bits (0 a 4095)
uint32_t ADC0_ReadSS3(void)
{
	uint32_t valor_convertido;
  ADC0_PSSI_R = 0x0008;
	
	while((ADC0_RIS_R & 0x0008) == 0) {};
  valor_convertido = ADC0_SSFIFO3_R & 0x0FFF;

  ADC0_ISC_R = 0x0008;

  return valor_convertido; 
}

// envia o padrao de bits para as bobinas do motor via PH0-PH3 com logica invertida (ULN2003)
void Motor_Output(uint32_t valor) {
    uint32_t temp = GPIO_PORTH_AHB_DATA_R & ~0x0F; // mantém os bits 4 a 7 intactos
    temp = temp | (~valor & 0x0F);                 // aplica o valor invertido nos bits 0 a 3
    GPIO_PORTH_AHB_DATA_R = temp;
}

// configura Timer0A para gerar interrupcao periodica a cada 200ms
void Timer0A_Init(void) {
    SYSCTL_RCGCTIMER_R |= 0x01;         // clock no Timer0
    while((SYSCTL_PRTIMER_R & 0x01) == 0){};
    TIMER0_CTL_R    &= ~0x01;           // desabilita Timer0A
    TIMER0_CFG_R     = 0x00;            // 32 bits
    TIMER0_TAMR_R    = 0x02;            // periódico
    TIMER0_TAILR_R   = 15999999;    
    TIMER0_IMR_R    |= 0x01;            // habilita interrupção
    NVIC_EN0_R      |= (1 << 19);       // habilita IRQ19 (Timer0A)
    TIMER0_CTL_R    |= 0x01;            // habilita Timer0A
}

// ISR do Timer0A: limpa a flag de interrupcao e sinaliza para a main que passaram 200ms
void Timer0A_Handler(void) {
    TIMER0_ICR_R = 0x01;
    flag_timer_200ms = 1;
}