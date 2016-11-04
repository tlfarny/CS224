				.title "traffic.asm"
;*******************************************************************************
;   Lab 5b - traffic.asm
;
;   Description:  1. Turn the large green LED and small red LED on and
;                    delay 20 seconds while checking for orange LED.
;                    (If orange LED is on and 10 seconds has expired, immediately
;                    skip to next step.)
;                 2. Turn large green LED off and yellow LED on for 5 seconds.
;                 3. Turn yellow LED off and large red LED on.
;                 4. If orange LED is on, turn small red LED off and small green
;                    LED on.  After 5 seconds, toggle small green LED on and off
;                    for 6 seconds at 1 second intervals.  Finish by toggling
;                    small green LED on and off for 4 seconds at 1/5 second
;                    intervals.
;                    Else, turn large red LED on for 5 seconds.
;                 5. Repeat the stoplight cycle.
;
;   I certify this to be my source code and not obtained from any student, past
;   or current.
;			Travis Farnsworth
;*******************************************************************************
;                            MSP430F2274
;                  .-----------------------------.
;            SW1-->|P1.0^                    P2.0|<->LCD_DB0
;            SW2-->|P1.1^                    P2.1|<->LCD_DB1
;            SW3-->|P1.2^                    P2.2|<->LCD_DB2
;            SW4-->|P1.3^                    P2.3|<->LCD_DB3
;       ADXL_INT-->|P1.4                     P2.4|<->LCD_DB4
;        AUX INT-->|P1.5                     P2.5|<->LCD_DB5
;        SERVO_1<--|P1.6 (TA1)               P2.6|<->LCD_DB6
;        SERVO_2<--|P1.7 (TA2)               P2.7|<->LCD_DB7
;                  |                             |
;         LCD_A0<--|P3.0                     P4.0|-->LED_1 (Green)
;        i2c_SDA<->|P3.1 (UCB0SDA)     (TB1) P4.1|-->LED_2 (Orange) / SERVO_3
;        i2c_SCL<--|P3.2 (UCB0SCL)     (TB2) P4.2|-->LED_3 (Yellow) / SERVO_4
;         LCD_RW<--|P3.3                     P4.3|-->LED_4 (Red)
;   TX/LED_5 (G)<--|P3.4 (UCA0TXD)     (TB1) P4.4|-->LCD_BL
;             RX-->|P3.5 (UCA0RXD)     (TB2) P4.5|-->SPEAKER
;           RPOT-->|P3.6 (A6)          (A15) P4.6|-->LED 6 (R)
;           LPOT-->|P3.7 (A7)                P4.7|-->LCD_E
;                  '-----------------------------'
;
;*******************************************************************************
;*******************************************************************************
            .cdecls  C,LIST,"msp430.h"      ; MSP430

            .asg    "bis.b #0x08,&P4OUT",RED_ON
            .asg    "bic.b #0x08,&P4OUT",RED_OFF
            .asg    "xor.b #0x08,&P4OUT",RED_TOGGLE
            .asg    "bit.b #0x08,&P4OUT",RED_TEST

            .asg	"bis.b #0x04,&P4OUT",YELLOW_ON
            .asg	"bic.b #0x04,&P4OUT",YELLOW_OFF
            .asg	"xor.b #0x04,&P4OUT",YELLOW_TOGGLE
            .asg	"bit.b #0x04,&P4OUT",YELLOW_TEST

            .asg	"bis.b #0x02,&P4OUT",ORANGE_ON
            .asg	"bic.b #0x02,&P4OUT",ORANGE_OFF
            .asg	"xor.b #0x02,&P4OUT",ORANGE_TOGGLE
            .asg	"bit.b #0x02,&P4OUT",ORANGE_TEST

            .asg	"bis.b #0x01,&P4OUT",GREEN_ON
            .asg	"bic.b #0x01,&P4OUT",GREEN_OFF
            .asg	"xor.b #0x01,&P4OUT",GREEN_TOGGLE
            .asg	"bit.b #0x01,&P4OUT",GREEN_TEST

            .asg	"bis.b #0x10,&P3OUT",PED_GREEN_ON
            .asg	"bic.b #0x10,&P3OUT",PED_GREEN_OFF
            .asg	"xor.b #0x10,&P3OUT",PED_GREEN_TOGGLE
            .asg	"bit.b #0x10,&P3OUT",PED_GREEN_TEST

            .asg	"bis.b #0x40,&P4OUT",PED_RED_ON
            .asg	"bic.b #0x40,&P4OUT",PED_RED_OFF
            .asg	"xor.b #0x40,&P4OUT",PED_RED_TOGGLE
            .asg	"bit.b #0x40,&P4OUT",PED_RED_TEST



DELAY		.equ	363500
TWENTY		.equ	200
TEN			.equ	100
FIVE		.equ	50
ONE			.equ	10
FIFTH		.equ	2
DEBOUNCE	.equ	10

			.bss	debounce_cnt,2
			.bss	timer_cnt,2

;------------------------------------------------------------------------------
            .text                           ; beginning of executable code
            .retain                         ; Override ELF conditional linking
;-------------------------------------------------------------------------------
start:		mov.w   #__STACK_END,SP         ; init stack pointer
            mov.w   #WDTPW+WDTHOLD,&WDTCTL  ; stop WDT
            bis.b   #0x4f,&P4DIR            ; set P4.3 as output
            bis.b	#0x10,&P3DIR			;
            bic.b	#0x4f,&P4OUT			;
            bic.b	#0x10,&P3OUT			;
            bis.w   #GIE,SR                 ; enable interrupts
            bic.b   #0x0f,&P1SEL           	; RBX430-1 push buttons
           	bic.b   #0x0f,&P1DIR           	; Configure P1.0-3 as Inputs
           	bis.b   #0x0f,&P1OUT           	; pull-ups
           	bis.b   #0x0f,&P1IES           	; h to l
           	bis.b   #0x0f,&P1REN           	; enable pull-ups
           	bis.b   #0x0f,&P1IE            	; enable switch interrupts
           	PED_RED_OFF
           	PED_GREEN_OFF
           	GREEN_OFF
           	YELLOW_OFF
           	RED_OFF
           	ORANGE_OFF

greenstart: RED_OFF
			GREEN_ON                   	; toggle red led
			PED_RED_ON						;
			mov.w	#TEN,r14				; counter timer
			jmp		gdelayloop1				;
cont:		mov.w	#TEN,r14
			jmp 	gdelayloop2

yellowstart:
			GREEN_OFF					;
			YELLOW_ON					;
			mov.w	#FIVE,r14				;
			jmp 	ydelayloop				;
			ORANGE_TEST
			jeq		redped
			jmp		redstart

redstart:	YELLOW_OFF					;
			RED_ON						;
			mov.w	#FIVE,r14				;
			jmp 	rdelayloop				;
			RED_OFF						;
			jmp		greenstart

redped:		YELLOW_OFF
			ORANGE_OFF
			RED_ON
			PED_RED_TOGGLE
			PED_GREEN_TOGGLE
			mov.w	#FIVE,r14
			add.w	#ONE,r14
			call	#rpedloop
			mov.w	#5, r13
rpedsec:	PED_GREEN_TOGGLE
			mov.w	#ONE,r14
			call	#rpedloop
			dec.w	r13
			jnz		rpedsec
			mov.w	#20,r13
rped5th:	PED_GREEN_TOGGLE
			mov.w	#FIFTH,r14
			call	#rpedloop
			dec.w	r13
			jnz		rped5th
			PED_GREEN_OFF
			jmp		greenstart


gdelayloop1:
			push	r15						;
			push	r14
repeat1:	mov.w   #DELAY,r15        	    ;
ginnerloop1:
			sub.w   #1,r15                  ;
              jne   ginnerloop1               ;
            sub.w	#1,r14					;
            jne		repeat1				;
            pop		r14
            pop 	r15
            jmp		cont

gdelayloop2:
			push	r15
			push	r14
repeat2:	mov.w	#DELAY,r15
ginnerloop2:
			dec.w	r15
			jne		ginnerloop2
			ORANGE_TEST
			jnz		yellowstart
			dec.w	r14
			jne		repeat2
			pop		r14
			pop		r15
			jmp		yellowstart

ydelayloop:	push	r15						;
			push	r14
repeaty:	mov.w   #DELAY,r15        	    ;
yinnerloop: sub.w   #1,r15                  ;
              jne   yinnerloop              ;
            sub.w	#1,r14					;
            jne		repeaty					;
            pop		r14
            pop 	r15
            ORANGE_TEST
            jnz		redped
            jmp		redstart

rdelayloop:	push	r15						;
			push	r14
repeatr:	mov.w   #DELAY,r15        	    ;
rinnerloop: sub.w   #1,r15                  ;
              jne   rinnerloop              ;
            sub.w	#1,r14					;
            jne		repeatr					;
            pop		r14
            pop 	r15
            jmp		greenstart

rpedloop:	push	r15
			push	r14
repeatrp:	mov.w	#DELAY,r15
rpedinner:	sub.w	#1,r15
			jne		rpedinner
			sub.w	#1,r14
			jne		repeatrp
			pop		r14
			pop		r15
			ret
s
;------------------------------------------------------------------------------

P1_ISR:    	bic.b   #0x0f,&P1IFG           ; acknowledge (put hands down)
			ORANGE_ON						;SPEAKER TOGGLE
			reti

;-------------------------------------------------------------------------------
;           Stack Pointer definition
;-------------------------------------------------------------------------------
            .global __STACK_END
            .sect 	.stack

;------------------------------------------------------------------------------
;           Interrupt Vectors
;------------------------------------------------------------------------------
            .sect  ".int02"              ; P1 interrupt vector
 			.word  P1_ISR



 			.sect   ".reset"                ; MSP430 RESET Vector
            .word   start                   ; start address
            .end
