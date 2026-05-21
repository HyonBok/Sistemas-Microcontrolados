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

uint32_t DutyCycle = 500;
uint8_t Pino = 0; // Flag HIGH/LOW PWM
int8_t DirecaoScan = 1; // Flag de direção para modo scan: 1 se tiver somando 20 e -1 se tiver subtraindo

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

void LCD_EnviaComando(uint32_t comando)
{
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

void LCD_EnviaDado(uint32_t dado) 
{
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

// Configura timer0 e timer2 32bits periódico
void Configura_Timers() 
{
	// Inicializar timer0 e timer 2
	SYSCTL_RCGCTIMER_R = 0x05;
	
	while(SYSCTL_RCGCTIMER_R != 0x05) {};
		
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN;
  TIMER2_CTL_R &= ~TIMER_CTL_TAEN;

  TIMER0_CFG_R = TIMER_CFG_32_BIT_TIMER;
  TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;

	TIMER2_CFG_R = TIMER_CFG_32_BIT_TIMER;
	TIMER2_TAMR_R = TIMER_TAMR_TAMR_PERIOD;

  TIMER0_ICR_R = TIMER_ICR_TATOCINT;
  TIMER2_ICR_R = TIMER_ICR_TATOCINT;

  TIMER0_IMR_R |= TIMER_IMR_TATOIM;
  TIMER2_IMR_R |= TIMER_IMR_TATOIM;

	TIMER0_TAPR_R = 0;
		
	TIMER2_TAPR_R = 0;
		
	// Prioridade 2
  NVIC_PRI4_R |= (2 << 29);
  NVIC_EN0_R |= (1 << 19);

	// Prioridade 1
	NVIC_PRI5_R |= (1 << 29);
  NVIC_EN2_R |= (1 << 23);
}

// Inicializa timer0
// Timer0 é que define o PWM do servo motor, então valor de tempo vai variar
void Inicializa_Timer0(int dutyCycle)
{
	// Primeiro verifica se o dutyCycle mudou, caso contrário, não faz sentido continuar
	if(dutyCycle == DutyCycle)
		return;
	
	// Atualiza o DutyCycle
	DutyCycle = dutyCycle;
	
	// Inicia o timer
	uint32_t x = (uint32_t)dutyCycle * 80000;
	
	TIMER0_TAILR_R = x - 1;
}

// Inicializa timer2
// Timer de 500ms fixo
void Inicializa_Timer2()
{
	/*
	Tempo = 500ms
	X = 500ms * 80M = 40M
	contagem = X - 1
	*/
	TIMER2_TAILR_R = 40000000 - 1;
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
						return teclado[coluna][linha];
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
	
	// Tempo HIGH
	uint32_t tempoHigh = (uint32_t)(DutyCycle * 80000);
	
	if(Pino == 0)
	{
		// Ligar pino
		GPIO_PORTL_DATA_R |= 0x10; // PL4 = HIGH

		TIMER0_TAILR_R = tempoHigh - 1;

		Pino = 1;
	}
	else
	{
		// Desligar pino
		GPIO_PORTL_DATA_R &= ~0x10; // PL4 = LOW

		// Completa os 20ms
		uint32_t tempoLow = 1600000 - tempoHigh;

		TIMER0_TAILR_R = tempoLow - 1;

		Pino = 0;
	}

	// Reinicia contador
	TIMER0_TAV_R = TIMER0_TAILR_R;
}


void Timer2A_Handler()
{
	// Limpar o flag de interrupção
	TIMER2_ICR_R = 0x01;
	
	// Adicionar ou subtrair dutyCycle enquivalente a um angulo de 20º (multiplicado por 1000 pela conversao de ms)
	// DutyCycle = (0.5 + 20 / 180 * 2) * 1000 = 722
	DutyCycle += 722 * DirecaoScan;
	
	// Servo estará com DutyCycle no máximo quando o ângulo for 180º e minimo quando for 0º
	// DutyCycleMaximo = (0.5 + 180 / 180 * 2) * 1000 = 2500
	// DutyCycleMinimo = (0.5 + 0 / 180 * 2) * 1000 = 500
	if(DutyCycle <= 500 || DutyCycle >= 2500){
		DirecaoScan *= -1;
	}
}