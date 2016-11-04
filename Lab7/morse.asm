			.title	"morse.asm"
;*******************************************************************************
;     Project:  morse.asm
;      Author:   Travis Farnsworth
;
; Description:  Outputs a message in Morse Code using a LED and a transducer
;               (speaker).  The watchdog is configured as an interval timer.
;               The watchdog interrupt service routine (ISR) toggles the green
;               LED every second and pulse width modulates (PWM) the speaker
;               such that a tone is produced.
;
;	Morse code is composed of dashes and dots:
;
;        1. A dot is equal to an element of time.
;        2. One dash is equal to three dots.
;        3. The space between parts of the letter is equal to one dot.
;        4. The space between two letters is equal to three dots.
;        5. The space between two words is equal to seven dots.
;
;    5 WPM = 60 sec / (5 * 50) elements = 240 milliseconds per element.
;    element = (WDT_IPS * 6 / WPM) / 5
;
;			I completed this code on my own.
;
;		H	****
;		E	*
;		L	*-**
;		L	*-**
;		O	---
;
;		C	-*-*
;		S	***
;
;		2	**---
;		2	**---
;		4	****-
;
;		W	*--
;		O	---
;		R	*-*
;		L	*-**
;		D	-**
;
;
;
;
;******************************************************************************
; System equates --------------------------------------------------------------
            .cdecls C,"msp430.h"            ; include c header
            .cdecls C,"morse2.h"
            .def	debounce_cnt
            .def	beep_cnt
            .def	timer_cnt
            .def	delay_cnt
            .def	message_length
            .def	doEND
            .def	doDOT
            .def	doDASH
            .def	doSPACE
            .def	doNULL
            .def	orangeTOGGLE
            .def	message

;myCLOCK     .equ    1050000                 ; 1.2 Mhz clock
;WDT_CTL     .equ    WDT_MDLY_0_5            ; WD: Timer, SMCLK, 0.5 ms
;WDT_CPI     .equ    500                     ; WDT Clocks Per Interrupt (@1 Mhz)
;WDT_IPS     .equ    (myCLOCK/WDT_CPI)       ; WDT Interrupts Per Second
;STACK       .equ    0x0600                  ; top of stack

; Morse Code equates ----------------------------------------------------------
;END         .equ    0
;DOT         .equ    1
;DASH        .equ    2
;ELEMENT     .equ    ((WDT_IPS*240)/1000)    ; (WDT_IPS * 6 / WPM) / 5
;DEBOUNCE   	.equ    10

; External references ---------------------------------------------------------
            .ref    numbers                 ; codes for 0-9
            .ref    letters                 ; codes for A-Z
                            
; Global variables ------------------------------------------------------------
            .bss    beep_cnt,2              ; beeper flag
            .bss    delay_cnt,2             ; delay flag
            .bss	timer_cnt,2
            .bss    debounce_cnt,2         ; debounce count
            .bss	message_length,2
											;may need to create one or two
; Program section -------------------------------------------------------------
            .text                           ; program section
message:    .string "HELLO CS 224 WORLD"                 ; PARIS message
            .byte   0
            .align  2                       ; align on word boundary

; Lights Initiation ----------------------------------------------------------
			.asg	"bis.b #0x10,&P3OUT",PED_GREEN_ON
            .asg	"bic.b #0x10,&P3OUT",PED_GREEN_OFF
            .asg	"xor.b #0x10,&P3OUT",PED_GREEN_TOGGLE
            .asg	"bit.b #0x10,&P3OUT",PED_GREEN_TEST

            .asg	"bis.b #0x40,&P4OUT",PED_RED_ON
            .asg	"bic.b #0x40,&P4OUT",PED_RED_OFF
            .asg	"xor.b #0x40,&P4OUT",PED_RED_TOGGLE
            .asg	"bit.b #0x40,&P4OUT",PED_RED_TEST

            .asg	"bis.b #0x01,&P4OUT",GREEN_ON
            .asg	"bic.b #0x01,&P4OUT",GREEN_OFF
            .asg	"xor.b #0x01,&P4OUT",GREEN_TOGGLE
            .asg	"bit.b #0x01,&P4OUT",GREEN_TEST

            .asg	"bis.b #0x02,&P4OUT",ORANGE_ON
            .asg	"bic.b #0x02,&P4OUT",ORANGE_OFF
            .asg	"xor.b #0x02,&P4OUT",ORANGE_TOGGLE
            .asg	"bit.b #0x02,&P4OUT",ORANGE_TEST

; Switches Initiation --------------------------------------------------------
;init_switches:

;           ret

; power-up reset --------------------------------------------------------------
;deleted

; start main function vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv	
;main_asm:      mov.w   #WDT_CTL,&WDTCTL        ; set WD timer interval
;               mov.b   #WDTIE,&IE1             ; enable WDT interrupt
;               bis.b   #0x4f,&P4DIR            ; set P4.3 as output
;			    bis.b	#0x10,&P3DIR			; set P3.2 as output;
;               bis.b   #0x20,&P4DIR            ; set P4.5 as output (speaker)
;               clr.w   &beep_cnt               ; clear counters
;               clr.w   &delay_cnt
;               mov.w	#WDT_IPS,&timer_cnt
;               bic.w	#0x4f,&P4OUT			; clear red light
;               bic.w	#0x10,&P3OUT
;               bis.w   #GIE,SR                 ; enable interrupts
;               bic.b   #0x0f,&P1SEL           	; RBX430-1 push buttons
;           	bic.b   #0x0f,&P1DIR           	; Configure P1.0-3 as Inputs
;           	bis.b   #0x0f,&P1OUT           	; pull-ups
;           	bis.b   #0x0f,&P1IES           	; h to l
;           	bis.b   #0x0f,&P1REN           	; enable pull-ups
;            	bis.b   #0x0f,&P1IE            	; enable switch interrupts;

;loop:      	mov.w  	#message,r4            	; point to message
;
;loop02:    	mov.b  	@r4+,r5                	; get character
;			    cmp.b	#0,r5					; check null
;    			jeq		findnull				; y, go reset message
;    			cmp.b 	#32,r5					; check if space
;	    		 jeq 	findSpace				; y, go to space
;	    		cmp.b	#65,r5					; n, check if is letter (ASCII>65)
;		    	 jge	findLetter				; y, go to find which letter
;		    	cmp.b	#48,r5					; n, check if this is number
;		    	 jge	findNumber				; y
;------------------------------------------------------------------------------------------
;findnull:   	ORANGE_TOGGLE					; signify done
;			    call	#doSPACE				; space break
 ;   			call	#doSPACE				; space break x 2
;	    		ret								;reset the message
;
;findSpace:	call #doSPACE						;should be 7 DOTS long
;			ret
;
;
;findLetter:	sub.w  	#'A',r5                	; make index 0-25;
;		   	add.w  	r5,r5                  		; make word inde;x
;		   	mov.w  	letters(r5),r5         		; get pointer to; letter codes
;		   	ret									;;
;
;findNumber:	sub.w	#'0',r5					; makes index 0-9
;			add.w 	r5,r5						; make word index
;			mov.w	numbers(r5),r5				; get pointer to number codes
;			ret;;
;
;;-------------------------------------------------------------------------------------------;
;
;loop10:    	mov.b  	@r5+,r6                	; get DOT, DASH, or END
;		   	cmp.b  	#DOT,r6                		; dot?
;		   	jne	goDASH							;
;		   	call	#doDOT						;
;		   	jmp loop10							; get next DOT/DASH
;goDASH:	   	cmp.b	#DASH,r6				; dash?
;		   	jne goEND							;
;		   	call	#doDASH						;;
;		   	jmp loop10							;; ;get next DOT/DASH
;goEND:	   	cmp.b	#END, r6					;; end?
;			call	#doEND;
;		   	jmp    loop02;
;------------------------;-------------------------------------------------------------------------
doEND:		push	r15
			mov.w	#ELEMENT*2,r15
			call	#delay
			pop		r15
			ret

doDOT:      push	r15
			mov.w   #ELEMENT,r15            ; output DOT
            call    #beep
            mov.w   #ELEMENT,r15            ; delay 1 element
            call    #delay
            pop		r15
            ret

doDASH:     push	r15
			mov.w   #ELEMENT*3,r15          ; output DASH
            call    #beep
            mov.w   #ELEMENT,r15            ; delay 1 element
            call    #delay
            pop		r15
            ret

doSPACE:    push	r15
 			mov.w   #ELEMENT*4,r15          ; output space
            call    #delay                  ; delay
            pop		r15
            ret

doNULL:		push 	r15
			ORANGE_TOGGLE
			call	#doSPACE
			call	#doSPACE
			pop 	r15


orangeTOGGLE:
			ORANGE_TOGGLE


; end main function ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


; beep (r15) ticks subroutine -------------------------------------------------
beep:       mov.w   r15,&beep_cnt           ; start beep

beep02:     tst.w   &beep_cnt               ; beep finished?
              jne   beep02                  ; n
            ret                             ; y


; delay (r15) ticks subroutine ------------------------------------------------
delay:      mov.w   r15,&delay_cnt          ; start delay

delay02:    tst.w   &delay_cnt              ; delay done?
              jne   delay02                 ; n
            ret                             ; y


; Watchdog Timer ISR ----------------------------------------------------------
WDT_ISR:   	tst.w   &beep_cnt               ; beep on?
              jeq   WDT_02                  ; n
            dec.w   &beep_cnt               ; y, decrement count
            xor.b   #0x20,&P4OUT            ; beep using 50% PWM
            PED_RED_TOGGLE

WDT_02:     tst.w   &delay_cnt              ; delay?
              jeq   WDT_11                  ; n
            dec.w   &delay_cnt              ; y, decrement count

WDT_11:		dec.w	&timer_cnt
			  jne	WDT_12
			PED_GREEN_TOGGLE
			mov.w	#WDT_IPS,&timer_cnt
											; toggling green light

WDT_12:		tst.w   &debounce_cnt           ; debouncing?
             jz	   	WDT_30                 ; n

; debounce switches & process

           	dec.w   &debounce_cnt           ; y, decrement count, done?
           	jne		WDT_30
           	push    r15                    ; y
           	mov.b   &P1IN,r15              ; read switches
           	and.b   #0x0f,r15
           	xor.b   #0x0f,r15              ; any switches?
             jz   WDT_20                 ; n
			xor.b	#0x20,&P4DIR


; process switch inputs (r15 = switches)

WDT_20:    	;xor.b	#0x20,&P4DIR
			pop     r15

WDT_30:
           	reti


; Interrupt Vectors -----------------------------------------------------------
            .sect   ".int10"                ; Watchdog Vector
            .word   WDT_ISR                 ; Watchdog ISR

            ;deleted
            .end
