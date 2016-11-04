#include "morse2.h"
/*
 * Name:  Travis Farnsworth
 * 		I certify that I produced this code on my own.
 */
#pragma vector=PORT1_VECTOR
__interrupt void Port_1_ISR(void)
{                         // P1_ISR:
   P1IFG &= ~0x0f;        //   					  bic.b   #0x0f,&P1IFG           ; acknowledge (put hands down)
   debounce_cnt = DEBOUNCE;   //              	  mov.w   #DEBOUNCE,&debounce_cnt ; reset debounce count
   return;                //   reti

}
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	WDTCTL = WDT_CTL;			//    main_asm:   mov.w   #WDT_CTL,&WDTCTL        ; set WD timer interval
	__enable_interrupt();		//                mov.b   #WDTIE,&IE1             ; enable WDT interrupt
	IE1 = WDTIE;
	P4DIR |= 0x4f;				//                bis.b   #0x4f,&P4DIR            ; set P4.3 as output
	P3DIR |= 0x10;				//    			  bis.b   #0x10,&P3DIR			; set P3.2 as output
	timer_cnt = WDT_IPS;
	P4DIR |= 0x20;				//                bis.b   #0x20,&P4DIR            ; set P4.5 as output (speaker)
	beep_cnt = 0;				//                clr.w   &beep_cnt               ; clear counters
	delay_cnt = 0;				//                clr.w   &delay_cnt
	timer_cnt = WDT_IPS;		//                mov.w	  #WDT_IPS,&timer_cnt
	P4OUT &= ~0x4f;				//                bic.w	  #0x4f,&P4OUT			; clear red light
	P3OUT &= ~0x10;				//                bic.w	  #0x10,&P3OUT
	__bis_SR_register(GIE);		//                bis.w   #GIE,SR                 ; enable interrupts
	P1SEL &= ~0x0f;				//                bic.b   #0x0f,&P1SEL           	; RBX430-1 push buttons
	P1DIR &= ~0x0f;				//                bic.b   #0x0f,&P1DIR           	; Configure P1.0-3 as Inputs
	P1OUT |= 0x0f;				//                bis.b   #0x0f,&P1OUT           	; pull-ups
	P1IES |= 0x0f;				//                bis.b   #0x0f,&P1IES           	; h to l
	P1REN |= 0x0f;				//                bis.b   #0x0f,&P1REN           	; enable pull-ups
	P1IE  |= 0x0f;				//                bis.b   #0x0f,&P1IE            	; enable switch interrupts
	char message[] = "HELLO CS 224 WORLD";
	while(1){
		char c;
		char* cptr;
		char* mptr = message;	//    loop:       mov.w  	#message,r4            	; point to message
		char code;
		while(c = *mptr++){					//    mov.b   @r4+,r5                	; get character
			if(isspace(c)){// 			  cmp.b   #32,r5					; check if space
				doSPACE();//	  	  findSpace:  call	  #doSPACE						;should be 7 DOTS long
			}
			else if (isalpha(c) || isdigit(c)){
				if(isalpha(c)){
					cptr = letters[toupper(c) - 'A'];//cmp.b	  #65,r5					; n, check if is letter (ASCII>65)
				}
									//findLetter: sub.w   #'A',r5                	; make index 0-25;
					//		   					  mov.w  	letters(r5),r5         		; get pointer to; letter codes
				else if (isdigit(c)){//  		  cmp.b	  #48,r5					; n, check if this is number
					cptr = numbers[c - '0'];//findNumber: sub.w	  #'0',r5					; makes index 0-9
					//							  mov.w	  numbers(r5),r5				; get pointer to number codes
				}
				while(code = *cptr++){//	  loop10:     mov.b   @r5+,r6                	; get DOT, DASH, or END
					if(code==DOT) doDOT();//	  cmp.b   #DOT,r6                		; dot?
					//				   			  jne	  goDASH							;
					//				   			  call	  #doDOT						;
					//				   			  jmp     loop10							; get next DOT/DASH
					else if(code==DASH) doDASH();//	  goDASH:	   	cmp.b	#DASH,r6				; dash?
					//				   			  jne     goEND							;
					//				   			  call	  #doDASH						;;
					//				   			  jmp     loop10							;; ;get next DOT/DASH
					//else if(cptr==END) doEND();//	  goEND:	   	cmp.b	#END, r6					;; end?
					//							  call	  #doEND;
					//				   			  jmp     loop02;
				}
				doEND();
			}
		}
		doNULL();
	}
	return 0;
}
