// main.c
// Desenvolvido para a placa EK-TM4C1294XL
// LAB 3 - SISTEMA DE POSICIONAMENTO COM MOTOR DE PASSO
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
void UART_Init(void);
char UART_InChar(void);
void UART_OutChar(char data);
void UART_OutString(char *pt);
void ADC_Init(void);
uint32_t ADC0_ReadSS3(void);
void Timer0A_Init(void);
void Timer0A_Handler(void);
char UART_InCharNonBlocking(void);
void Motor_Output(uint32_t valor);
void EnableInterrupts(void);
void UART_PrintUInt(uint32_t valor);

// mapeamento para ativação das bobinas
const uint8_t seq_passo_completo[4] = {0x03, 0x06, 0x0C, 0x09}; 
const uint8_t seq_meio_passo[8]     = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0C, 0x08, 0x09};

volatile uint8_t flag_timer_200ms = 0;

// definição dos estados
typedef enum {
    ESTADO_IDLE,
    ESTADO_POTENCIOMETRO,
    ESTADO_TERMINAL
} EstadosSistema;


int main(void)
{
	int32_t posicao_atual_passos = 0;
  int32_t passos_alvo = 0;
  uint8_t indice_motor_meiopasso = 0;
	static uint8_t contador_1s = 0;
	
	// inicialização
	PLL_Init();
	SysTick_Init();
	GPIO_Init();
	UART_Init();
	ADC_Init();
	Timer0A_Init();
	EnableInterrupts();
	
	UART_OutString("Sistema em IDLE. Esperando comando...\r\n");
	
	EstadosSistema estado_atual = ESTADO_IDLE;
	while(1) {
		
		// verifica qual tecla foi apertada sem travar o motor
		char tecla = UART_InCharNonBlocking();    
		// seleção de modos
    if (tecla == 'p' || tecla == 'P') {
			estado_atual = ESTADO_POTENCIOMETRO;
    }
		else if (tecla == 't' || tecla == 'T') {
      estado_atual = ESTADO_TERMINAL;
    }
		
    // MODO POTENCIÔMETRO
    if (estado_atual == ESTADO_POTENCIOMETRO) {
			// a cada 200ms ativa a flag e lê o valor ADC (que varia de 0 a 4095)
      if (flag_timer_200ms) {
				flag_timer_200ms = 0;
				uint32_t adc_val = ADC0_ReadSS3();
				// o valor ADC é armazenado aqui
				passos_alvo = adc_val;
				contador_1s++;
				// print periódico no terminal
				if (contador_1s >= 5) {
					contador_1s = 0;
					int32_t graus_x10 = (posicao_atual_passos * 3600) / 4095;
					UART_OutString("[POT] ADC: ");
					UART_PrintUInt(adc_val);
					UART_OutString(" | Posicao: ");
					UART_PrintUInt(graus_x10 / 10);
					UART_OutChar(',');
					UART_PrintUInt(graus_x10 % 10);
					UART_OutString(" graus\r\n");
				}
			}
      // se o valor lido no ADC for diferente da posição atual, move o motor
      if (posicao_atual_passos != passos_alvo) {
				// se a posição for maior
				if (passos_alvo > posicao_atual_passos) {
					indice_motor_meiopasso = (indice_motor_meiopasso + 1) % 8; // 8 passos por ciclo
          posicao_atual_passos++;
				// se a posição for menor
        } else {
					indice_motor_meiopasso = (indice_motor_meiopasso - 1);
          if (indice_motor_meiopasso > 7) indice_motor_meiopasso = 7; // caso tenha overflow, reseta o índice
          posicao_atual_passos--;
				}
				// envia o valor para o motor
        Motor_Output(seq_meio_passo[indice_motor_meiopasso]); 
				// tempo de espera entre passos
        SysTick_Wait1ms(5); 
      }
		}
        
		// MODO TERMINAL
		else if (estado_atual == ESTADO_TERMINAL) {
			static char cmd_buf[5] = {0};
			static uint8_t cmd_idx = 0;
			if (tecla != 0) {
				UART_OutChar(tecla); // eco
				// assim que aperta enter processa o comando
				if (tecla == '\r' || tecla == '\n') {
					// verifica se tem 4 dígitos e se começa com + ou -
					if (cmd_idx == 4 && (cmd_buf[0] == '+' || cmd_buf[0] == '-')) {
						// converte o comando de graus para passos
						int32_t graus = (cmd_buf[1]-'0')*100 + (cmd_buf[2]-'0')*10 + (cmd_buf[3]-'0');
						int32_t passos = (int32_t)((graus * 2048L) / 360);
						if (cmd_buf[0] == '-') passos = -passos;
							passos_alvo = posicao_atual_passos + passos;
					}
					cmd_idx = 0;
				} else if (cmd_idx < 4) {
					cmd_buf[cmd_idx++] = tecla;
				}
			}
			// print periodico a cada 1 segundo
			if (flag_timer_200ms) {
				flag_timer_200ms = 0;
				contador_1s++;
				if (contador_1s >= 5) {
					contador_1s = 0;
					int32_t graus_x10 = (posicao_atual_passos * 3600) / 2048;
					UART_OutString("[TERM] Posicao: ");
					UART_PrintUInt(graus_x10 / 10);
					UART_OutChar(',');
					UART_PrintUInt(graus_x10 % 10);
					UART_OutString(" graus\r\n");
				}
			}

			// se a posição atual é diferente da desejada, a cada loop anda um passo inteiro
			if (posicao_atual_passos != passos_alvo) {
				if (passos_alvo > posicao_atual_passos) {
					indice_motor_meiopasso = (indice_motor_meiopasso + 1) % 4;
					posicao_atual_passos++;
				} else {
					indice_motor_meiopasso = (indice_motor_meiopasso + 3) % 4;
					posicao_atual_passos--;
				}
				// envia para o motor 
				Motor_Output(seq_passo_completo[indice_motor_meiopasso % 4]);
				SysTick_Wait1ms(5);
				if (posicao_atual_passos == passos_alvo) {
					int32_t graus_x10 = (posicao_atual_passos * 3600) / 2048;
					UART_OutString("[TERM] Posicao: ");
					UART_PrintUInt(graus_x10 / 10);
					UART_OutChar(',');
					UART_PrintUInt(graus_x10 % 10);
					UART_OutString(" graus\r\n");
				}
			}
		}
  }
}

// lê um caractere vindo do terminal 
char UART_InChar(void) {
    while((UART0_FR_R & 0x0010) != 0);
    return (char)(UART0_DR_R & 0xFF);
}

// envia um caractere para o terminal
void UART_OutChar(char data) {
    while((UART0_FR_R & 0x0020) != 0);
    UART0_DR_R = data;
}

// envia uma string para o terminal
void UART_OutString(char *pt) {
    while(*pt) {
        UART_OutChar(*pt);
        pt++;
    }
}

// lÊ um caractere vindo do terminal sem bloquear o motor
char UART_InCharNonBlocking(void) {
    if((UART0_FR_R & 0x0010) == 0) {
        return (char)(UART0_DR_R & 0xFF);
    }
    return 0;
}

void UART_PrintUInt(uint32_t valor)
{
    uint8_t digits[10];
    uint8_t count = 0;

    if (valor == 0) {
        UART_OutChar('0');
        return;
    }

    while (valor > 0) {
        digits[count++] = valor % 10;
        valor /= 10;
    }

    for (int8_t i = count - 1; i >= 0; i--) {
        UART_OutChar('0' + digits[i]);
    }
}