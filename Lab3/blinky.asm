;*******************************************************************************
;   CS/ECEn 124 Lab 3 - blinky.asm: Software Toggle P1.0
;
;   Description: Quickly blink P1.0 every 10 seconds.  Calculate MCLK, CPI, MIPS
;        Author: Travis Farnsworth-I did everything on my own.
;
;             MSP430G5223
;             -----------
;            |       P1.0|-->LED1-RED LED
;            |       P1.3|<--S2
;            |       P1.6|-->LED2-GREEN LED
;
; Show all calculations:
;
;   MCLK = 10908525 cycles / 10 sec interval = 1.0908 Mhz
;    CPI = 10908525 cycles/ 7272208 instructions = 1.5000 Cycles/Instruction
;   MIPS = 1.0908 / 1.5000 = 0.7272 MIPS

;
;*******************************************************************************
           .cdecls  C,"msp430.h"            ; MSP430

COUNT      .equ     0                       ; delay count
HUNDRED	   .equ		100						; 100 loop
THOUSAND   .equ		10000					;
DELAY	   .equ		36000					;
;------------------------------------------------------------------------------
            .text                           ; beginning of executable code
;------------------------------------------------------------------------------
;											Cycles										Instructions
start:      mov.w   #0x0280,SP              ; 2   init stack pointer			|1
            mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; 5   stop WDT				|1
            bis.b   #0x41,&P1DIR            ; 4   set P1.0 as output			|1
	    bic.b   #04x01,&P1OUT	    ; 4											|1
mainloop:   bis.b   #0x41,&P1OUT	    ; 4											|1
	    mov.w   #DELAY,r13		    ; 1											|1
blink:	    sub.w   #1,r13		    ; 1*36000 = 36000				|1*36000
	    jne	    blink   		    ; 2*36000 = 72000				|1*36000
	    bic.b   #0x41,&P1OUT	    ; 1						|1

	    mov.w   #HUNDRED,r14	    ; 2						|1
outerloop:  mov.w   #DELAY,r15        	    ; | 2*100 = 200  use R15 as delay counter   |100
delayloop:  sub.w   #1,r15                  ; | | 1*36000*100 = 3600000    delay over?	|36000*100 = 3600000
              jne   delayloop               ; | | 2*36000*100 = 7200000  n		|36000*100 = 3600000
            sub.w   #1,r14		    ; | 1*100 = 100				|100
            jne	    outerloop		    ; | 2*100 = 200				|100
            jmp     mainloop                ; 2   y, toggle led				|1
;											Total cycles = 10908525						Total Instr = 7272208
;------------------------------------------------------------------------------
;           Interrupt Vectors
;------------------------------------------------------------------------------
            .sect   ".reset"                ; MSP430 RESET Vector
            .word   start                   ; start address
            .end
