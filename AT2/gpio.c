// gpio.c
// Desenvolvido para a placa EK-TM4C1294XL

#include <stdint.h>
#include "tm4c1294ncpdt.h"

#define GPIO_PORTK  (0x0200) //bit 9
#define GPIO_PORTM  (0x0800) //bit 11
#define GPIO_PORTL  (0x0400) //bit 10

char teclado[4][4] =
{
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

void SysTick_Wait1us(uint32_t delay);
void SysTick_Wait1ms(uint32_t delay);
// -------------------------------------------------------------------------------
// Função GPIO_Init
// Inicializa os ports
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
	GPIO_PORTL_DIR_R = 0x00;
		
	// 5. Limpar os bits AFSEL para 0 para selecionar GPIO sem função alternativa	
	GPIO_PORTK_AFSEL_R = 0x00;
	GPIO_PORTM_AFSEL_R = 0x00;
	GPIO_PORTL_AFSEL_R = 0x00;
		
	// 6. Setar os bits de DEN para habilitar I/O digital	
	GPIO_PORTK_DEN_R = 0xFF;
	GPIO_PORTM_DEN_R = 0xF7;
	GPIO_PORTL_DEN_R = 0x0F;
	
	// 7. Habilitar resistor de pull-up interno, setar PUR para 1
	GPIO_PORTL_PUR_R = 0x0F;
}	

void LCD_EnviaComando(uint32_t comando) {
    GPIO_PORTK_DATA_R = comando;
    GPIO_PORTM_DATA_R &= ~0x03; // PM0 e PM1 vão pra 0
    GPIO_PORTM_DATA_R |= 0x04;  // PM2 para 1
    SysTick_Wait1us(10); 
    GPIO_PORTM_DATA_R &= ~0x04;
		
		if (comando == 0x01)
		{
			SysTick_Wait1ms (2);
		}
		else
		{
			SysTick_Wait1us (40);
		}
}

void LCD_EnviaDado(uint32_t dado) {
    GPIO_PORTK_DATA_R = dado;
    GPIO_PORTM_DATA_R &= ~0x02; // PM0 vai pra 0
    GPIO_PORTM_DATA_R |= 0x01;  // PM1 para 1
		GPIO_PORTM_DATA_R |= 0x04;	//LCD ON (PM2)
    SysTick_Wait1us(10); 
		GPIO_PORTM_DATA_R &= ~0x04; // LCD OFF (PM2)
		SysTick_Wait1us(40);
}

void LCD_EnviaString (const char *string) {
	while (*string != '\0') {
		LCD_EnviaDado (*string);
		string ++;
	}
}

void Inicializa_Timer() {
	SYSCTL_RCGCTIMER_R = 0x0;
	
	while(SYSCTL_RCGCTIMER_R != 0x0) {};
		
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN;

  TIMER0_CFG_R = TIMER_CFG_32_BIT_TIMER;
  TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;

  TIMER0_TAILR_R = 1600000 - 1;

  TIMER0_TAPR_R = 0;

  TIMER0_ICR_R = TIMER_ICR_TATOCINT;

  TIMER0_IMR_R |= TIMER_IMR_TATOIM;

  NVIC_PRI4_R |= (2 << 29);
  NVIC_EN0_R |= (1 << 19);
}


char Leitura_Teclado()
{
    uint32_t coluna;
		uint32_t linhas;
		uint8_t linha;

		for(coluna = 0; coluna < 4; coluna++)
		{
			// Tenho que ter cuidado para não mexer nos valores da PORTM do display LCD(PM0-PM2)
			// Mexendo a partir da coluna PM4
			GPIO_PORTM_DATA_R = ~(1 << (coluna + 4));
			
			linhas = GPIO_PORTL_DATA_R & 0x0F;
			
			// Se as linhas estão 1111, quer dizer que não há nada pressionado, caso contrário entra no if
			if(linhas != 0x0F)
			{
				// For para descobrir qual linha
				for(linha = 0; linha < 4; linha++)
				{
					if((linhas & ~(1 << linha)) == 0)
					{
						return teclado[linha][coluna];
					}
				}
			}
    }
		
		// Retornando espaço caso não tenha nada(ver outro caracter talve?)
		return ' ';
}

void Timer0A_Handler()
{
	// Limpar o flag de interrupção
	TIMER0_ICR_R = 0x01;
	
	/* Fazer o que tem que fazer */
}