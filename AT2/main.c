// main.c
// Desenvolvido para a placa EK-TM4C1294XL
// LAB 2 - Controle de Posicao do Servomotor
// Prof. Guilherme Peron

#include <stdint.h>
#include <stdio.h>
#include "tm4c1294ncpdt.h"

#define LIMPAR         0x01
#define PRIMEIRA_LINHA 0x80
#define SEGUNDA_LINHA  0xC0
#define SIMBOLO_GRAU   0xDF 

void PLL_Init(void);
void SysTick_Init(void);
void SysTick_Wait1ms(uint32_t delay);
void SysTick_Wait1us(uint32_t delay);
void GPIO_Init(void);
void LCD_EnviaComando(uint32_t comando);
void LCD_EnviaDado(uint32_t dado);
void LCD_EnviaString(const char *string);
void Configura_Timers(void);
void Inicializa_Timer0(int dutyCycle);
void Inicializa_Timer2(void);
char Leitura_Teclado(void);
void TrocarEstado(char tecla);
void AtualizaLCDPosicao(int angulo, float dutyCycle_ms);

typedef enum {
	Idle    = 0,
	Posicao = 1,
	Scan    = 2,
} Estado;

Estado EstadoAtual = Idle;
int    AnguloAtual = 90;

// acessando
extern uint32_t DutyCycle;

int main(void)
{
	PLL_Init();
	SysTick_Init();
	GPIO_Init();
	Configura_Timers();

	// Inicializar LCD
	LCD_EnviaComando(0x38); 
	LCD_EnviaComando(0x06);
	LCD_EnviaComando(0x0E);
	LCD_EnviaComando(LIMPAR);
	LCD_EnviaComando(PRIMEIRA_LINHA);
	LCD_EnviaString("Modo Idle");

	// Posicionar servo em 90° no Idle inicial
	Inicializa_Timer0(1500); 

	uint32_t ultimoDutyCycle = 0; 

	while (1)
	{
		char tecla = Leitura_Teclado();

		TrocarEstado(tecla);

		if (EstadoAtual == Scan && DutyCycle != ultimoDutyCycle)
		{
			ultimoDutyCycle = DutyCycle;
			float dutyCycle_ms = (float)DutyCycle / 1000.0f;
			AtualizaLCDPosicao(AnguloAtual, dutyCycle_ms);
			// Derivar angulo a partir do DutyCycle atual
			// dc = 500 + (angulo/180)*2000  =>  angulo = (dc-500)/2000 * 180
			AnguloAtual = (int)(((float)(DutyCycle - 500) / 2000.0f) * 180.0f);
			AtualizaLCDPosicao(AnguloAtual, dutyCycle_ms);
		}
	}

	return 0;
}

// Escreve na segunda linha: "Pos:XXX°/YYYYus"
void AtualizaLCDPosicao(int angulo, float dutyCycle_ms)
{
	char buf[17];
	int dutyCycle_us = (int)(dutyCycle_ms * 1000.0f);

	LCD_EnviaComando(SEGUNDA_LINHA);
	snprintf(buf, sizeof(buf), "Pos:%3d", angulo);
	LCD_EnviaString(buf);
	LCD_EnviaDado(SIMBOLO_GRAU);                       
	snprintf(buf, sizeof(buf), "/%4dus", dutyCycle_us); 
	LCD_EnviaString(buf);
}

void TrocarEstado(char tecla)
{
	if (tecla == ' ')
		return; // Nenhuma tecla pressionada

	if (tecla == '*')
	{
		TIMER2_CTL_R &= ~TIMER_CTL_TAEN; // Para o Timer2 (modo Scan)
		EstadoAtual = Idle;
		AnguloAtual = 90;
		Inicializa_Timer0(1500);          // Centralizar servo em 90 graus
		LCD_EnviaComando(LIMPAR);
		LCD_EnviaComando(PRIMEIRA_LINHA);
		LCD_EnviaString("Modo Idle");
	}
	else if (tecla >= '0' && tecla <= '9')
	{
		if (EstadoAtual != Posicao)
        {
            // So reescreve a primeira linha ao entrar no modo pela primeira vez
            TIMER2_CTL_R &= ~TIMER_CTL_TAEN; // Para o Timer2 se vinha do Scan
            EstadoAtual = Posicao;
            LCD_EnviaComando(LIMPAR);
            LCD_EnviaComando(PRIMEIRA_LINHA);
            LCD_EnviaString("Modo Posicao");
        }

        if (tecla != '0')
            AnguloAtual = (tecla - '1') * 20; 
        else
            AnguloAtual = 180;

        float dutyCycle_ms = 0.5f + ((float)AnguloAtual / 180.0f) * 2.0f;
        Inicializa_Timer0((uint32_t)(dutyCycle_ms * 1000));
        AtualizaLCDPosicao(AnguloAtual, dutyCycle_ms);
	}
	else if (tecla == 'A')
	{
		if (EstadoAtual != Scan)
		{
			EstadoAtual = Scan;
			AnguloAtual = 0;
			LCD_EnviaComando(LIMPAR);
			LCD_EnviaComando(PRIMEIRA_LINHA);
			LCD_EnviaString("Modo Scan");
			Inicializa_Timer2(); // Iniciar varredura de 500ms
		}
	}
}