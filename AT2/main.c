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
char Leitura_Teclado();

int main(void)
{
	PLL_Init();
	SysTick_Init();
	GPIO_Init();
	
	LCD_EnviaComando(0x38); // Modo 2 linhas
	LCD_EnviaComando(0x06); // Cursor direita
	LCD_EnviaComando(0x0E); // Ligar
	LCD_EnviaComando(0x01); // Limpar
	
	LCD_EnviaDado('U');
	LCD_EnviaDado('T');
	LCD_EnviaDado('F');
	LCD_EnviaDado('P');
	LCD_EnviaDado('R');
	
	LCD_EnviaComando(0xC0); // Início Segunda Linha
	LCD_EnviaDado('G');
	LCD_EnviaDado('L');
	LCD_EnviaDado('Y');
	LCD_EnviaDado('C');
	LCD_EnviaDado('O');
	LCD_EnviaDado('N');
	LCD_EnviaDado(' ');
	LCD_EnviaDado('E');
	LCD_EnviaDado('L');
	LCD_EnviaDado('E');
	LCD_EnviaDado('N');
	LCD_EnviaDado(' ');
	LCD_EnviaDado('H');
	LCD_EnviaDado('Y');
	LCD_EnviaDado('O');
	LCD_EnviaDado('N');
	
}

