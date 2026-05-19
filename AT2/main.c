// main.c
// Desenvolvido para a placa EK-TM4C1294XL
// Verifica o estado das chaves USR_SW1 e USR_SW2, acende os LEDs 1 e 2 caso estejam pressionadas independentemente
// Caso as duas chaves estejam pressionadas ao mesmo tempo pisca os LEDs alternadamente a cada 500ms.
// Prof. Guilherme Peron

#include <stdint.h>

void PLL_Init(void);
void SysTick_Init(void);
void SysTick_Wait1ms(uint32_t delay);
void SysTick_Wait1us(uint32_t delay);
void GPIO_Init(void);
void Pisca_leds(void);
void LCD_EnviaComando(uint32_t comando);
void LCD_EnviaDado(uint32_t dado);
void LCD_EnviaString (const char *string);
void Inicializa_Timer();
char Leitura_Teclado();

int main(void)
{
	PLL_Init();
	SysTick_Init();
	GPIO_Init();
	Inicializa_Timer();
	
	LCD_EnviaComando(0x38); // Modo 2 linhas
	LCD_EnviaComando(0x06); // Cursor direita
	LCD_EnviaComando(0x0E); // Ligar
	
	LCD_EnviaComando(0xC0); // Início Segunda Linha

	while(1)
	{
		char tecla = Leitura_Teclado();
		
		/*
		codigo exemplo
		if (tecla == '0')
		{
			LCD_EnviaComando(0x01); // Limpar
		
			char angulo = '1';
		
			LCD_EnviaDado(angulo);
		}
		
		*/
	}
	
}

