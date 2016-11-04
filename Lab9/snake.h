//------------------------------------------------------------------------------
// snake.h (04/06/2015)
//------------------------------------------------------------------------------
#ifndef SNAKE_H_
#define SNAKE_H_

//-- system constants ----------------------------------------------------------
#define myCLOCK		12000000			// clock speed
#define CLOCK		_12MHZ

//-- watchdog constants --------------------------------------------------------
#define WDT_CLK		32000				// 32 Khz WD clock (@1 Mhz)
#define	WDT_CTL		WDT_MDLY_32			// WDT SMCLK, ~32ms
#define	WDT_CPS		(myCLOCK/WDT_CLK)	// WD clocks / second count

#if WDT_CPS/50
#define DEBOUNCE_CNT	(WDT_CPS/50)	// 20 ms debounce count
#else
#define DEBOUNCE_CNT	1				// 20 ms debounce count
#endif

#define WDT_LCD		(WDT_CPS/4)			// 250 ms
#define WDT_MOVE1	(WDT_CPS/4)			// 250 ms
#define WDT_MOVE2	(WDT_CPS/8)			// 125 ms
#define WDT_MOVE3	(WDT_CPS/16)		// 62 ms
#define WDT_MOVE4	(WDT_CPS/32)		// 31 ms

//-- sys_events ----------------------------------------------------------------
#define SWITCH_1	0x0001
#define SWITCH_2	0x0002
#define SWITCH_3	0x0004
#define SWITCH_4	0x0008

#define START_LEVEL	0x0010
#define NEXT_LEVEL	0x0020
#define END_GAME	0x0040
#define NEW_GAME	0x0080

#define MOVE_SNAKE	0x0100
#define LCD_UPDATE	0x0200

//-- service routine events ----------------------------------------------------
void SWITCH_1_event(void);
void SWITCH_2_event(void);
void SWITCH_3_event(void);
void SWITCH_4_event(void);

void START_LEVEL_event(void);
void NEXT_LEVEL_event(void);
void END_GAME_event(void);
void NEW_GAME_event(void);

void MOVE_SNAKE_event(void);
void LCD_UPDATE_event(void);

//-- snake game equates --------------------------------------------------------
#define START_SCORE			10
#define X_MAX	24						// columns
#define Y_MAX	23						// rows

#define MAX_SNAKE			128			// max snake length (make power of 2)
#define MAX_FOOD			10			// max # of foods

#define TIME_1_LIMIT		30
#define LEVEL_1_FOOD		5			// 10

#define TIME_2_LIMIT		30
#define LEVEL_2_FOOD		MAX_FOOD

#define TIME_3_LIMIT		30
#define LEVEL_3_FOOD		MAX_FOOD

#define TIME_4_LIMIT		60
#define LEVEL_4_FOOD		MAX_FOOD

enum DIRECTION {RIGHT, UP, LEFT, DOWN};		// movement constants
enum MODE {IDLE, SETUP, PLAY, NEXT, EOG};	// player modes

typedef struct							// POINT struct
{
	uint8 x;
	uint8 y;
} POINT;

typedef union							// snake segment object
{
	uint16 xy;
	POINT point;
} SNAKE;

typedef struct food_struct
{  POINT point;
   uint8 value, size;
   void (*draw)(struct food_struct* food);
} FOOD;


#define COL(x)	((x)*6+7+3)				// grid x value to LCD column
#define ROW(y)	((y)*6+10+3)			// grid y value to LCD row

int watchdogtimer_init(uint16);
int port1_init(void);
void draw_square_food(FOOD* food);
void draw_circle_food(FOOD* food);
void draw_triangle_food(FOOD* food);
void draw_star_food(FOOD* food);
void draw_diamond_food(FOOD* food);

#endif /* SNAKE_H_ */
