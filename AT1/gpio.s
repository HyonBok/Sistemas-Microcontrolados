; gpio.s
; Desenvolvido para a placa EK-TM4C1294XL
; Prof. Guilherme Peron
; 19/03/2018

; -------------------------------------------------------------------------------
        THUMB                        ; Instruções do tipo Thumb-2
; -------------------------------------------------------------------------------
; Declarações EQU - Defines
; ========================
; Definições dos Registradores Gerais
SYSCTL_RCGCGPIO_R	 EQU	0x400FE608
SYSCTL_PRGPIO_R		 EQU    0x400FEA08
    
; ========================
; NVIC
NVIC_EN1_R           EQU    0xE000E104
NVIC_PRI12_R		 EQU    0xE000E430        
; ========================
; Definições dos Ports
; Portas utilizadas: A, B, J, N, P, Q

; -------------------------------------------------------------------------------
; Push buttons - Aumentar e diminuir setpoint
; Utiliza PJ0 e PJ1
; PORT J
GPIO_PORTJ_IS_R      	EQU    0x40060404
GPIO_PORTJ_IBE_R      	EQU    0x40060408
GPIO_PORTJ_IEV_R      	EQU    0x4006040C
GPIO_PORTJ_IM_R      	EQU    0x40060410
GPIO_PORTJ_RIS_R      	EQU    0x40060414
GPIO_PORTJ_ICR_R      	EQU    0x4006041C    
GPIO_PORTJ_LOCK_R    	EQU    0x40060520
GPIO_PORTJ_CR_R      	EQU    0x40060524
GPIO_PORTJ_AMSEL_R   	EQU    0x40060528
GPIO_PORTJ_PCTL_R    	EQU    0x4006052C
GPIO_PORTJ_DIR_R     	EQU    0x40060400
GPIO_PORTJ_AFSEL_R   	EQU    0x40060420
GPIO_PORTJ_DEN_R     	EQU    0x4006051C
GPIO_PORTJ_PUR_R     	EQU    0x40060510	
GPIO_PORTJ_DATA_R    	EQU    0x400603FC
GPIO_PORTJ              EQU    0x00000100

; -------------------------------------------------------------------------------
; LEDS - Representam se a quantidade está menor, maior ou igual ao setpoint
; Utiliza PN0 e PN1
; PORT N
GPIO_PORTN_LOCK_R    	EQU    0x40064520
GPIO_PORTN_CR_R      	EQU    0x40064524
GPIO_PORTN_AMSEL_R   	EQU    0x40064528
GPIO_PORTN_PCTL_R    	EQU    0x4006452C
GPIO_PORTN_DIR_R     	EQU    0x40064400
GPIO_PORTN_AFSEL_R   	EQU    0x40064420
GPIO_PORTN_DEN_R     	EQU    0x4006451C
GPIO_PORTN_PUR_R     	EQU    0x40064510	
GPIO_PORTN_DATA_R    	EQU    0x400643FC
GPIO_PORTN              EQU    0x00001000	

; -------------------------------------------------------------------------------
; Transistor multiplexador dos LEDS de setpoint
; Utiliza PP5
; PORT P
GPIO_PORTP_IS_R      	EQU    0x40065404
GPIO_PORTP_IBE_R      	EQU    0x40065408
GPIO_PORTP_IEV_R      	EQU    0x4006540C
GPIO_PORTP_IM_R      	EQU    0x40065410
GPIO_PORTP_RIS_R      	EQU    0x40065414
GPIO_PORTP_ICR_R      	EQU    0x4006541C    
GPIO_PORTP_LOCK_R    	EQU    0x40065520
GPIO_PORTP_CR_R      	EQU    0x40065524
GPIO_PORTP_AMSEL_R   	EQU    0x40065528
GPIO_PORTP_PCTL_R    	EQU    0x4006552C
GPIO_PORTP_DIR_R     	EQU    0x40065400
GPIO_PORTP_AFSEL_R   	EQU    0x40065420
GPIO_PORTP_DEN_R     	EQU    0x4006551C
GPIO_PORTP_PUR_R     	EQU    0x40065510	
GPIO_PORTP_DATA_R    	EQU    0x400653FC
GPIO_PORTP              EQU    0x00002000

; -------------------------------------------------------------------------------
; Transistor multiplexador do display 7 segmentos - representam o valor atual do tanque
; Utiliza PB4 e PB5
; PORT B
GPIO_PORTB_IS_R      	EQU    0x40059404
GPIO_PORTB_IBE_R      	EQU    0x40059408
GPIO_PORTB_IEV_R      	EQU    0x4005940C
GPIO_PORTB_IM_R      	EQU    0x40059410
GPIO_PORTB_RIS_R      	EQU    0x40059414
GPIO_PORTB_ICR_R      	EQU    0x4005941C    
GPIO_PORTB_LOCK_R    	EQU    0x40059520
GPIO_PORTB_CR_R      	EQU    0x40059524
GPIO_PORTB_AMSEL_R   	EQU    0x40059528
GPIO_PORTB_PCTL_R    	EQU    0x4005952C
GPIO_PORTB_DIR_R     	EQU    0x40059400
GPIO_PORTB_AFSEL_R   	EQU    0x40059420
GPIO_PORTB_DEN_R     	EQU    0x4005951C
GPIO_PORTB_PUR_R     	EQU    0x40059510	
GPIO_PORTB_DATA_R    	EQU    0x400593FC
GPIO_PORTB              EQU    0x00000002

; -------------------------------------------------------------------------------
; Display 7 segmentos e LEDS
; Utiliza PQ0-PQ3 e PA4-PA7
; PORT Q
GPIO_PORTQ_IS_R      	EQU    0x40066404
GPIO_PORTQ_IBE_R      	EQU    0x40066408
GPIO_PORTQ_IEV_R      	EQU    0x4006640C
GPIO_PORTQ_IM_R      	EQU    0x40066410
GPIO_PORTQ_RIS_R      	EQU    0x40066414
GPIO_PORTQ_ICR_R      	EQU    0x4006641C    
GPIO_PORTQ_LOCK_R    	EQU    0x40066520
GPIO_PORTQ_CR_R      	EQU    0x40066524
GPIO_PORTQ_AMSEL_R   	EQU    0x40066528
GPIO_PORTQ_PCTL_R    	EQU    0x4006652C
GPIO_PORTQ_DIR_R     	EQU    0x40066400
GPIO_PORTQ_AFSEL_R   	EQU    0x40066420
GPIO_PORTQ_DEN_R     	EQU    0x4006651C
GPIO_PORTQ_PUR_R     	EQU    0x40066510	
GPIO_PORTQ_DATA_R    	EQU    0x400663FC
GPIO_PORTQ              EQU    0x00004000

; PORT A
GPIO_PORTA_IS_R      	EQU    0x40058404
GPIO_PORTA_IBE_R      	EQU    0x40058408
GPIO_PORTA_IEV_R      	EQU    0x4005840C
GPIO_PORTA_IM_R      	EQU    0x40058410
GPIO_PORTA_RIS_R      	EQU    0x40058414
GPIO_PORTA_ICR_R      	EQU    0x4005841C    
GPIO_PORTA_LOCK_R    	EQU    0x40058520
GPIO_PORTA_CR_R      	EQU    0x40058524
GPIO_PORTA_AMSEL_R   	EQU    0x40058528
GPIO_PORTA_PCTL_R    	EQU    0x4005852C
GPIO_PORTA_DIR_R     	EQU    0x40058400
GPIO_PORTA_AFSEL_R   	EQU    0x40058420
GPIO_PORTA_DEN_R     	EQU    0x4005851C
GPIO_PORTA_PUR_R     	EQU    0x40058510	
GPIO_PORTA_DATA_R    	EQU    0x400583FC
GPIO_PORTA              EQU    0x00000001


; -------------------------------------------------------------------------------
; Área de Código - Tudo abaixo da diretiva a seguir será armazenado na memória de 
;                  código
        AREA    |.text|, CODE, READONLY, ALIGN=2

		; Se alguma função do arquivo for chamada em outro arquivo	
        EXPORT GPIO_Init            ; Permite chamar GPIO_Init de outro arquivo
		;EXPORT PortA_Output			; Permite chamar PortN_Output de outro arquivo
		;EXPORT PortB_Output
		EXPORT PortN_Output
		;EXPORT PortP_Output
		;EXPORT PortQ_Output
        EXPORT GPIOPortJ_Handler    
        IMPORT EnableInterrupts
        IMPORT DisableInterrupts
		IMPORT SysTick_Wait1ms
									

;--------------------------------------------------------------------------------
; Função GPIO_Init
; Parâmetro de entrada: Não tem
; Parâmetro de saída: Não tem
GPIO_Init
;=====================
; 1. Ativar o clock para a porta setando o bit correspondente no registrador RCGCGPIO,
; após isso verificar no PRGPIO se a porta está pronta para uso.
; enable clock to GPIOF at clock gating register
            LDR     R0, =SYSCTL_RCGCGPIO_R  		;Carrega o endereço do registrador RCGCGPIO
			MOV		R1, #GPIO_PORTA                 ;Seta o bit da porta A
			ORR     R1, #GPIO_PORTB					;Seta o bit da porta B, fazendo com OR
			ORR     R1, #GPIO_PORTJ
			ORR     R1, #GPIO_PORTN
			ORR     R1, #GPIO_PORTP
			ORR     R1, #GPIO_PORTQ
            STR     R1, [R0]						;Move para a memória os bits das portas no endereço do RCGCGPIO

            LDR     R0, =SYSCTL_PRGPIO_R			;Carrega o endereço do PRGPIO para esperar os GPIO ficarem prontos
EsperaGPIO  LDR     R1, [R0]						;Lê da memória o conteúdo do endereço do registrador
			MOV     R2, #GPIO_PORTA                 ;Seta os bits correspondentes às portas para fazer a comparação
			ORR     R2, #GPIO_PORTB                 ;Seta o bit da porta B, fazendo com OR
			ORR     R2, #GPIO_PORTJ
			ORR     R2, #GPIO_PORTN
			ORR     R2, #GPIO_PORTP
			ORR     R2, #GPIO_PORTQ
            TST     R1, R2							;ANDS de R1 com R2
            BEQ     EsperaGPIO					    ;Se o flag Z=1, volta para o laço. Senão continua executando
 
; 2. Limpar o AMSEL para desabilitar a analógica
            MOV     R1, #0x00						;Colocar 0 no registrador para desabilitar a função analógica
            LDR     R0, =GPIO_PORTA_AMSEL_R     	;Carrega o R0 com o endereço do AMSEL para a porta A
            STR     R1, [R0]						;Guarda no registrador AMSEL da porta A da memória
			LDR     R0, =GPIO_PORTB_AMSEL_R     	
            STR     R1, [R0]						
            LDR     R0, =GPIO_PORTJ_AMSEL_R		
            STR     R1, [R0]		
            LDR     R0, =GPIO_PORTN_AMSEL_R     	
            STR     R1, [R0]						
            LDR     R0, =GPIO_PORTP_AMSEL_R		
            STR     R1, [R0]					  
            LDR     R0, =GPIO_PORTQ_AMSEL_R     	
            STR     R1, [R0]									   
 
; 3. Limpar PCTL para selecionar o GPIO
            MOV     R1, #0x00					    ;Colocar 0 no registrador para selecionar o modo GPIO
            LDR     R0, =GPIO_PORTA_PCTL_R			;Carrega o R0 com o endereço do PCTL para a porta A
            STR     R1, [R0]                        
			LDR     R0, =GPIO_PORTB_PCTL_R			
            STR     R1, [R0]                        
            LDR     R0, =GPIO_PORTJ_PCTL_R          
            STR     R1, [R0]       
            LDR     R0, =GPIO_PORTN_PCTL_R          
            STR     R1, [R0]       
            LDR     R0, =GPIO_PORTP_PCTL_R          
            STR     R1, [R0]                 
            LDR     R0, =GPIO_PORTQ_PCTL_R          
            STR     R1, [R0]      

; 4. DIR para 0 se for entrada, 1 se for saída
			LDR     R0, =GPIO_PORTA_DIR_R		    ;Carrega o R0 com o endereço do DIR para a porta A
			MOV     R1, #2_11110000					;Saídas PA4-PA7
            STR     R1, [R0]						;Guarda no registrador	
            LDR     R0, =GPIO_PORTB_DIR_R
			MOV     R1, #2_00110000                 ;Saídas PB4 e PB5
            STR     R1, [R0]					
			LDR     R0, =GPIO_PORTJ_DIR_R		    
			MOV     R1, #0x00					    ;Entradas PJ0 e PJ1
            STR     R1, [R0]								
            LDR     R0, =GPIO_PORTN_DIR_R		    
			MOV     R1, #2_00000011					;Saídas PN0 e PN1
            STR     R1, [R0]						
            LDR     R0, =GPIO_PORTP_DIR_R		    
			MOV     R1, #2_00100000					;Saída PP5
            STR     R1, [R0]		
            LDR     R0, =GPIO_PORTQ_DIR_R		    
			MOV     R1, #2_00001111					;Saídas PQ0-PQ3 
            STR     R1, [R0]											
; 5. Limpar os bits AFSEL para 0 para selecionar GPIO 
;    Sem função alternativa
            MOV     R1, #0x00						;Colocar o valor 0 para não setar função alternativa
            LDR     R0, =GPIO_PORTA_AFSEL_R         ;Carrega o endereço do AFSEL da porta AFSEL
            STR     R1, [R0]                   
            LDR     R0, =GPIO_PORTB_AFSEL_R		
            STR     R1, [R0]					
            LDR     R0, =GPIO_PORTJ_AFSEL_R    
            STR     R1, [R0]     
            LDR     R0, =GPIO_PORTN_AFSEL_R    
            STR     R1, [R0]      
            LDR     R0, =GPIO_PORTP_AFSEL_R    
            STR     R1, [R0]      
            LDR     R0, =GPIO_PORTQ_AFSEL_R    
            STR     R1, [R0]                    
; 6. Setar os bits de DEN para habilitar I/O digital
            LDR     R0, =GPIO_PORTA_DEN_R			;Carrega o endereço do DEN
            LDR     R1, [R0]                        ;Ler da memória o registrador GPIO_PORTA_DEN_R
			MOV     R2, #2_11110000                 
            ORR     R1, R2                          
            STR     R1, [R0]                        ;Escreve no registrador da memória funcionalidade digital
			
            LDR     R0, =GPIO_PORTB_DEN_R		
            LDR     R1, [R0]					   
			MOV     R2, #2_00110000		
            ORR     R1, R2
            STR     R1, [R0]					
 
            LDR     R0, =GPIO_PORTJ_DEN_R		
            LDR     R1, [R0]                       
			MOV     R2, #2_00000011                
            ORR     R1, R2                         
            STR     R1, [R0]       

            LDR     R0, =GPIO_PORTN_DEN_R		
            LDR     R1, [R0]                       
			MOV     R2, #2_00000011                
            ORR     R1, R2                         
            STR     R1, [R0]   

            LDR     R0, =GPIO_PORTP_DEN_R		
            LDR     R1, [R0]                       
			MOV     R2, #2_00100000                
            ORR     R1, R2                         
            STR     R1, [R0]   

            LDR     R0, =GPIO_PORTQ_DEN_R		
            LDR     R1, [R0]                       
			MOV     R2, #2_00001111                
            ORR     R1, R2                         
            STR     R1, [R0]                   
			
; 7. Para habilitar resistor de pull-up interno, setar PUR para 1
			LDR     R0, =GPIO_PORTJ_PUR_R			    ;Carrega o endereço do PUR para a porta J
			MOV     R1, #2_11						;Habilitar funcionalidade digital de resistor de pull-up 
            STR     R1, [R0]							;Escreve no registrador da memória do resistor de pull-up

;Interrupções
; 8. Desabilitar a interrupção no registrador IM(para depois habilitar novamente)
			LDR     R0, =GPIO_PORTJ_IM_R			    ;Carrega o endereço do IM para a porta J
			MOV     R1, #2_00							;Desabilitar as interrupções  
            STR     R1, [R0]							;Escreve no registrador
            
; 9. Configurar o tipo de interrupção por borda no registrador IS
			LDR     R0, =GPIO_PORTJ_IS_R			    ;Carrega o endereço do IS para a porta J
			MOV     R1, #2_00							;Por Borda
            STR     R1, [R0]							;Escreve no registrador

; 10. Configurar borda única no registrador IBE
			LDR     R0, =GPIO_PORTJ_IBE_R				;Carrega o endereço do IBE para a porta J
			MOV     R1, #2_00							;Borda Única  
            STR     R1, [R0]							;Escreve no registrador

; 11. Configurar borda de descida(botão pressionado) no registrador IEV
			LDR     R0, =GPIO_PORTJ_IEV_R				;Carrega o endereço do IEV para a porta J
			MOV     R1, #2_00							;Borda Única  
            STR     R1, [R0]							;Escreve no registrador
  
; 12. Habilitar a interrupção no registrador IM
			LDR     R0, =GPIO_PORTJ_IM_R				;Carrega o endereço do IM para a porta J
			MOV     R1, #2_11							;Habilitar as interrupções  
            STR     R1, [R0]							;Escreve no registrador
            
;Interrupção número 72            
; 13. Setar a prioridade no NVIC
			LDR     R0, =NVIC_PRI12_R           		;Carrega do NVIC para o grupo que tem o J entre 48 e 51
			MOV     R1, #3  		                    ;Prioridade 3
			LSL     R1, #29                             ;
            STR     R1, [R0]							;Escreve no registrador da memória
; 14. Habilitar a interrupção no NVIC
			LDR     R0, =NVIC_EN1_R           			;Carrega o do NVIC para o grupo que tem o J entre 32 e 63
			MOV     R1, #1
			LSL     R1, #19								;Desloca 19 bits para a esquerda já que o J é a interrupção do bit 19 no EN1
            STR     R1, [R0]							;Escreve no registrador da memória

			BX  LR

; -------------------------------------------------------------------------------
; Função PortN_Output
; Parâmetro de entrada: R0 --> se os BIT1 e BIT0 estão ligado ou desligado
; Parâmetro de saída: Não tem
PortN_Output
	LDR	R1, =GPIO_PORTN_DATA_R		    ;Carrega o valor do offset do data register
	;Read-Modify-Write para escrita
	LDR R2, [R1]
	BIC R2, #2_00000011                     ;Primeiro limpamos os dois bits lido da porta R2 = R2 & 11111100
	ORR R0, R0, R2                          ;Fazer o OR do valor lido pela porta com o parâmetro de entrada
	STR R0, [R1]                            ;Escreve na porta N o barramento de dados dos pinos [N5-N0]
	BX LR									;Retorno




; -------------------------------------------------------------------------------
; Função ISR GPIOPortJ_Handler (Tratamento da interrupção)
GPIOPortJ_Handler
    PUSH {R0, R1, R2, LR}
    
    LDR R0, =0x40060418      ; Endereço do GPIO_PORTJ_MIS_R
    LDR R1, [R0]             ; Lê quem causou a interrupção
    
    ; Testar Botão 1 (PJ0) - Incrementa
    TST R1, #0x01            ; Verifica se o bit 0 está alto
    BEQ TestaBotao2          ; Se não, vai pro próximo
    CMP R5, #99              ; Já está no limite?
    BHS LimpaInterrupcao     ; Se sim, não aumenta
    ADD R5, R5, #1           ; R5 = Setpoint++
    B LimpaInterrupcao

TestaBotao2
    ; Testar Botão 2 (PJ1) - Decrementa
    TST R1, #0x02            ; Verifica se o bit 1 está alto
    BEQ LimpaInterrupcao
    CMP R5, #10              ; Já está no mínimo?
    BLS LimpaInterrupcao     ; Se sim, não diminui
    SUB R5, R5, #1           ; R5 = Setpoint--

LimpaInterrupcao
    LDR R0, =0x4006041C      ; Endereço do GPIO_PORTJ_ICR_R
    MOV R1, #0x03            ; Limpa ambos os bits 0 e 1
    STR R1, [R0]
    
    POP {R0, R1, R2, PC}

    ALIGN                           
    END                             
        
        
        