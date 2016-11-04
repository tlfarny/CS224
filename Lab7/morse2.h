#include <msp430.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/*
 * morse2.h
 *
 *  Created on: Mar 24, 2015
 *      Author: Travis Farnsworth
 */

#ifndef MORSE2_H_
#define MORSE2_H_
#define myCLOCK  1050000                	//; 1.2 Mhz clock
#define WDT_CTL  WDT_MDLY_0_5            //; WD: Timer, SMCLK, 0.5 ms
#define WDT_CPI  500                     //; WDT Clocks Per Interrupt (@1 Mhz)
#define WDT_IPS  ( myCLOCK / WDT_CPI )       //; WDT Interrupts Per Second
#define STACK  0x0600                  //; top of stack
#define END  0
#define DOT  1
#define DASH  2
#define ELEMENT  ( ( WDT_IPS * 240 ) / 1000 )    //; (WDT_IPS * 6 / WPM) / 5
#define DEBOUNCE 10

extern int timer_cnt;
extern int beep_cnt;
extern int debounce_cnt;
extern int message_length;
extern int delay_cnt;
extern char message;
extern void doEND();
extern void doDOT();
extern void doDASH();
extern void doSPACE();
extern void doNULL();
extern void orangeTOGGLE();
extern char* letters[];
extern char* numbers[];

#endif /* MORSE2_H_ */
