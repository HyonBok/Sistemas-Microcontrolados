; Exemplo.s
; Desenvolvido para a placa EK-TM4C1294XL
; Prof. Guilherme Peron
; 12/03/2018

; -------------------------------------------------------------------------------
        THUMB                        ; Instruções do tipo Thumb-2
; -------------------------------------------------------------------------------
; Declarações EQU - Defines
;<NOME>         EQU <VALOR>
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

; -------------------------------------------------------------------------------
; Função main()
RAM EQU 0x20000400
MEDIA EQU 0x20000500
MAIOR EQU 0x20000501
MENOR EQU 0x20000502
NOTNULL EQU 0x20000503
DESVIO EQU 0x20000504

; Tamanho do vetor
TAMANHO EQU 20
; Valor máximo byte
MAX EQU 0xFF

Start  
; Comece o código aqui <======================================================

; Definindo registradores
; R0: Tamanho vetor
; R1: Posição RAM
; R2: Menor valor (começando em MAX)
; R3: Maior valor (començando em 0)
; R4: Contador de não nulos
; R5: Soma total
; R6: Iterador e offset
; R7: Valor atual carregado da RAM
; R8: Media
; R9: Desvio

	MOV R0, #TAMANHO
	LDR R1, =RAM
	MOV R2, #MAX
	
; Loop principal
for
	LDRB R7, [R1, R6]
	CMP R7, #0
	BEQ invalido
	ADDNE R4, #1
	; Compara se é menor
	CMP R7, R2
	IT CC
		MOVCC R2, R7
	; Compara se é maior
	CMP R7, R3
	IT CS
		MOVCS R3, R7

invalido
	; Soma total
	ADD R5, R7
	; Adiciona iterador (i++)
	ADD R6, #1
	; Duas comparações, quando for menor ou igual
	CMP R6, R0
	BCC for
	CMP R6, R0
	BEQ for
	B media
	
media
	UDIV R8, R5, R4
	B store
	
store
	LDR R12, =MEDIA
	STRB R8, [R12]
	LDR R12, =MAIOR
	STRB R3, [R12]
	LDR R12, =MENOR
	STRB R2, [R12]
	LDR R12, =NOTNULL
	STRB R4, [R12]
	LDR R12, =DESVIO
	STRB R9, [R12]
	
	NOP

    ALIGN                           ; garante que o fim da seção está alinhada 
    END                             ; fim do arquivo
