// main.c
// Desenvolvido para a placa EK-TM4C1294XL
// Verifica o estado das chaves USR_SW1 e USR_SW2, acende os LEDs 1 e 2 caso estejam pressionadas independentemente
// Caso as duas chaves estejam pressionadas ao mesmo tempo pisca os LEDs alternadamente a cada 500ms.
// Prof. Guilherme Peron

#include <stdint.h>
#include <stdio.h>

#define LIMPAR 0x01
#define PRIMEIRA_LINHA 0x80
#define SEGUNDA_LINHA 0xC0

void PLL_Init(void);
void SysTick_Init(void);
void SysTick_Wait1ms(uint32_t delay);
void SysTick_Wait1us(uint32_t delay);
void GPIO_Init(void);
void Pisca_leds(void);
void LCD_EnviaComando(uint32_t comando);
void LCD_EnviaDado(uint32_t dado);
void LCD_EnviaString (const char *string);
void Configura_Timers();
void Inicializa_Timer0(float dutyCycle);
void Inicializa_Timer2();
char Leitura_Teclado();
void TrocarEstado(char tecla);

typedef enum {
	Idle = 0,
	Posicao = 1,
	Scan = 2,
} Estado;

Estado EstadoAtual = Idle;
int AnguloAtual = 0;

int main(void)
{
	PLL_Init();
	SysTick_Init();
	GPIO_Init();
	Configura_Timers();
	
	LCD_EnviaComando(0x38); // Modo 2 linhas
	LCD_EnviaComando(0x06); // Cursor direita
	LCD_EnviaComando(0x0E); // Ligar
	LCD_EnviaComando(PRIMEIRA_LINHA); // Início Primeira Linha
	
	LCD_EnviaString("Modo Idle");

	while(1)
	{
		char tecla = Leitura_Teclado();
		
		TrocarEstado(tecla);
		
		if(EstadoAtual == Idle){
			// Fixo angulo de 90 graus
			// dutyCycle = 0.5 + 90 / 180 * 2
			float dutyCycle = 1.5;
				
			Inicializa_Timer0((uint32_t)(dutyCycle * 1000));
		}
		else if(EstadoAtual == Posicao){
			LCD_EnviaComando(LIMPAR); 
			LCD_EnviaComando(PRIMEIRA_LINHA); 
			LCD_EnviaString("Modo Posicao");
			LCD_EnviaComando(SEGUNDA_LINHA); 

			if(tecla != '0'){
				// Intervalo de 0º até 160º
				AnguloAtual = (tecla - '1') * 20;
			}
			else{
				AnguloAtual = 180;
			}
			
			float dutyCycle = 0.5 + AnguloAtual / 180 * 2;

			char Mensagem[50];
			snprintf(Mensagem, sizeof(Mensagem), "Pos: %d / %.1fus", AnguloAtual, dutyCycle);

			// Timer que decide PWM do motor
			Inicializa_Timer0((uint32_t)(dutyCycle * 1000));
			
			LCD_EnviaString(Mensagem);
		}
		else if(EstadoAtual == Scan){
			LCD_EnviaComando(LIMPAR); 
			LCD_EnviaComando(PRIMEIRA_LINHA); 
			LCD_EnviaString("Modo Scan");
			LCD_EnviaComando(SEGUNDA_LINHA); 

			/* Adicionar angulo na interrupção */
			
			
		}
	}

	return 0;
}

void TrocarEstado(char tecla){
	if(tecla == ' '){
		// Caso não seja pressionado nada, vai continuar apenas no mesmo estado
		return;
	}
	else if(tecla == '*'){
		EstadoAtual = Idle;
		LCD_EnviaComando(LIMPAR); 
		LCD_EnviaComando(PRIMEIRA_LINHA);
		LCD_EnviaString("Modo Idle");
	}
	else if(tecla >= '0' && tecla <= '9'){
		EstadoAtual = Posicao;
	}
	else if(tecla == 'A'){
		EstadoAtual = Scan;
		AnguloAtual = 0;
		Inicializa_Timer2();
	}
}

