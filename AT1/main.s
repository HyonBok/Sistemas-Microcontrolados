; main.s
; Desenvolvido para a placa EK-TM4C1294XL
; Prof. Guilherme Peron
; 15/03/2018
; Este programa espera o usuário apertar a chave USR_SW1 e/ou a chave USR_SW2.
; Caso o usuário pressione a chave USR_SW1, acenderá o LED2. Caso o usuário pressione 
; a chave USR_SW2, acenderá o LED1. Caso as duas chaves sejam pressionadas, os dois 
; LEDs acendem.

; -------------------------------------------------------------------------------
        THUMB                        ; Instruções do tipo Thumb-2
; -------------------------------------------------------------------------------

; -------------------------------------------------------------------------------
; Área de Dados - Declarações de variáveis
		AREA  DATA, ALIGN=2
		; Se alguma variável for chamada em outro arquivo
		;EXPORT  <var> [DATA,SIZE=<tam>]   ; Permite chamar a variável <var> a 
		                                   ; partir de outro arquivo
;<var>	SPACE <tam>                        ; Declara uma variável de nome <var>
                                           ; de <tam> bytes a partir da primeira 
                                           ; posição da RAM		

; -------------------------------------------------------------------------------
; Área de Código - Tudo abaixo da diretiva a seguir será armazenado na memória de 
;                  código
        AREA    |.text|, CODE, READONLY, ALIGN=2

		; Se alguma função do arquivo for chamada em outro arquivo	
        EXPORT Start                ; Permite chamar a função Start a partir de 
			                        ; outro arquivo. No caso startup.s
									
		; Se chamar alguma função externa	
        ;IMPORT <func>              ; Permite chamar dentro deste arquivo uma 
									; função <func>
		IMPORT  PLL_Init
		IMPORT  SysTick_Init
		IMPORT  SysTick_Wait1ms										
		IMPORT  GPIO_Init
        IMPORT  PortN_Output

; -------------------------------------------------------------------------------

Tab7Seg  DCB 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F
; Função main()
Start  		
	BL PLL_Init                  ;Chama a subrotina para alterar o clock do microcontrolador para 80MHz
	BL SysTick_Init              ;Chama a subrotina para inicializar o SysTick
	BL GPIO_Init                 ;Chama a subrotina que inicializa os GPIO
	
	MOV R4, #10 ;nível atual
	MOV R5, #50 ;setpoint
	LDR R6, =83
	
loop
    CMP R4, R5
    BEQ estabilidade
    BHI esvaziando
	
enchendo
	MOV R0, #2_01 ;liga o PN0
	BL PortN_Output
	B mux

esvaziando
	MOV R0, #2_10 ;liga o PN1
    BL PortN_Output
    B mux

estabilidade
	MOV R0, #3
	BL PortN_Output
	
mux
	;dezena
	MOV R0, #10
	UDIV R1, R4, R0 ;R1 guarda a dezena do nível atual
	BL mostrar
	MOV R0, #2_010000
	LDR R2, =0x400593FC
	STR R0, [R2] ;liga o transistor Q1
	MOV R0, #1
	BL SysTick_Wait1ms ;deixa ligado por 1ms
	MOV R0, #0
	STR R0, [R2] ;desliga Q1
	MOV R0, #1
	BL SysTick_Wait1ms
	
	;unidade
	MOV R0, #10
    UDIV R1, R4, R0
	MLS R1, R1, R0, R4 ;R1 guarda a unidade (R1= R4-R1*R0)
	BL mostrar
	MOV R0, #2_100000 ;liga Q2 
    STR R0, [R2]
    MOV R0, #1
    BL SysTick_Wait1ms
    MOV R0, #0
    STR R0, [R2] ;desliga Q2
	MOV R0, #1
	BL SysTick_Wait1ms
	
	;setpoint
	; Pega os 4 bits mais significativos de R5 e joga na PORTA
	AND R2, R5, #0xF0      
	LDR R3, =0x400583FC    
	STR R2, [R3]
	; Pega os 4 bits menos significativos de R5 e joga na PORTQ
	AND R2, R5, #0x0F      
	LDR R3, =0x400663FC    
	STR R2, [R3]
	MOV R0, #2_100000 ;liga Q3 
	LDR R2, =0x400653FC ;DATA PORT P
	STR R0, [R2]
	MOV R0, #1
	BL SysTick_Wait1ms
	MOV R0, #0
	STR R0, [R2] ;desliga Q3
	MOV R0, #1
	BL SysTick_Wait1ms
	SUBS R6, R6, #1
	BNE loop
	
	LDR R6, =83
	CMP R4, R5
	IT LT
		ADDLT R4, R4, #1 ;se menor, aumenta R4 em 1
	IT GT
		SUBGT R4, R4, #1 ;se maior, subtrai R4 em 1
	B loop

mostrar
	PUSH {R2, R3, LR}
    CMP R1, #10
    BHS pula
    LDR R2, =Tab7Seg
    LDRB R1, [R2, R1]
	
pula
    AND R2, R1, #0xF0
    LDR R3, =0x400583FC 
    STR R2, [R3]
    AND R2, R1, #0x0F
    LDR R3, =0x400663FC
    STR R2, [R3]
    POP {R2, R3, PC}

	NOP
	
    ALIGN                        ;Garante que o fim da seção está alinhada 
    END                          ;Fim do arquivo
