//******************************************************************************
//	snake_events.c  (04/06/2015)
//
//  Author:			Paul Roper, Brigham Young University
//  Revisions:		1.0		11/25/2012	RBX430-1
//
//******************************************************************************
//
#include "msp430.h"
#include <stdlib.h>
#include "RBX430-1.h"
#include "RBX430_lcd.h"
#include "snake.h"
#include "snakelib.h"

extern volatile uint16 sys_event;			// pending events

volatile enum MODE game_mode;			// 0=idle, 1=play, 2=next
volatile uint16 score;					// current score
volatile uint16 level;					// current level (1-4)
volatile uint16 direction;				// current move direction
volatile uint16 move_cnt;				// snake speed
extern volatile uint16 timer = 0;

volatile uint8 head;					// head index into snake array
volatile uint8 tail;					// tail index into snake array
SNAKE snake[MAX_SNAKE];					// snake segments

extern const uint16 snake_text_image[];		// snake text image
extern const uint16 snake1_image[];			// snake image

static uint8 move_right(SNAKE* head);		// move snake head right
static uint8 move_up(SNAKE* head);			// move snake head up
static uint8 move_left(SNAKE* head);		// move snake head left
static uint8 move_down(SNAKE* head);		// move snake head down
static void new_snake(uint16 length, uint8 dir);
static void delete_tail(void);
static void add_head(void);
static void rock_col(void);
static void self_col(void);
static FOOD* newFood(uint16 x, uint16 y, uint8 value, uint8 size, void (*draw)(FOOD*));
static FOOD* replace_food(int n);
static void new_food_check(FOOD* f);
static void fill_food(void);

FOOD* food[10];

int foodCounter = 0;
int snakeSize = 2;
int Score = 0;
int currentScore = 0;
int hitSomething = 0;				//bool
int hitFood = 0;					//bool
int timer_up = 0;					//bool

//-- switch #1 event -----------------------------------------------------------
//
void SWITCH_1_event(void)
{
	switch (game_mode)
	{
		case IDLE:
			level = 1;
			sys_event |= START_LEVEL;

			break;

		case PLAY:
			if (direction != LEFT)
			{
				if (snake[head].point.x < X_MAX)
				{
					direction = RIGHT;
					sys_event |= MOVE_SNAKE;
				}
			}
			break;

		case NEXT:
			sys_event |= NEW_GAME;
			break;
	}
	return;
} // end SWITCH_1_event


//-- switch #2 event -----------------------------------------------------------
//
void SWITCH_2_event(void){
	switch (game_mode){
	case IDLE:
		level = 1;
			sys_event |= START_LEVEL;
			break;

		case PLAY:
			if (direction != UP)
			{
				if (snake[head].point.x < X_MAX)
				{
					direction = DOWN;
					sys_event |= MOVE_SNAKE;
				}
			}
			break;

		case NEXT:
			sys_event |= NEW_GAME;
			break;
	}
	return;
} // end SWITCH_2_event


//-- switch #3 event -----------------------------------------------------------
//
void SWITCH_3_event(void)
{
	switch (game_mode)
		{
case IDLE:

	level = 1;
			sys_event |= START_LEVEL;
			break;

		case PLAY:
			if (direction != DOWN)
			{
				if (snake[head].point.x < X_MAX)
				{
					direction = UP;
					sys_event |= MOVE_SNAKE;
				}
			}
			break;

		case NEXT:
			sys_event |= NEW_GAME;
			break;
	}
	return;
} // end SWITCH_3_event


//-- switch #4 event -----------------------------------------------------------
//
void SWITCH_4_event(void)
{
	switch (game_mode)
		{
case IDLE:

	level = 1;
			sys_event |= START_LEVEL;
			break;

		case PLAY:
			if (direction != RIGHT)
			{
				if (snake[head].point.x < X_MAX)
				{
					direction = LEFT;
					sys_event |= MOVE_SNAKE;
				}
			}
			break;

		case NEXT:
			sys_event |= NEW_GAME;
			break;
	}
	return;
} // end SWITCH_4_event


//-- next level event -----------------------------------------------------------
//
void NEXT_LEVEL_event(void)
{
	//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	//	Add code here to handle NEXT_LEVEL event
	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
} // end NEXT_LEVEL_event


//-- update LCD event -----------------------------------------------------------
//
void LCD_UPDATE_event(void)
{
	lcd_clear();
	lcd_cursor(7,0);
	lcd_printf("LEFT    UP   DOWN   RIGHT");
	lcd_cursor(7,152);
	lcd_printf("Score XX  Level XX   0:");
	lcd_rectangle(7,10,146,140,1);
} // end LCD_UPDATE_event


//-- end game event -------------------------------------------------------------
//
void END_GAME_event(void)
{
	//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	//	Add code here to handle END_GAME event
	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
} // end END_GAME_event


//-- move snake event ----------------------------------------------------------
//
void MOVE_SNAKE_event(void){
	if (level > 0){
		add_head();						// add head
		lcd_point(COL(snake[head].point.x), ROW(snake[head].point.y), PENTX);
		if(timer_up){
			timer_up = 0;
			rasberry();
			sys_event = END_GAME;
			return;
		}
		self_col();
		if(level > 1) rock_col();
		if(hitSomething){
			hitSomething = 0;
			sys_event = END_GAME;
		}
		if(hitFood){
			hitFood = 0;
			if(foodCounter == 3){
				game_mode = NEXT;
				charge();
			}
			return;
		}
//		if (snake[head].xy == ((0 << 8) + 12)) { beep(); blink();}
//		lcd_square(COL(12), ROW(0), 2, 1 + FILL);
//		//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//		delete_tail();					// delete tail
	}
	return;
} // end MOVE_SNAKE_event


//-- start level event -----------------------------------------------------------
//
void START_LEVEL_event(void){
//	int foodCounter = 0;
//		int snakeSize = 2;
//		int currentScore = 0;
//		int hitSomething = 0;
//		int hitFood = 0;
	if(level == 1){
		snakeSize = 2;
		currentScore = 0;
		hitSomething = 0;
		hitFood = 0;
		score = 0;
		foodCounter = 0;
		move_cnt = WDT_MOVE1;				// level 1, speed 1
		new_snake(snakeSize,UP);
		LCD_UPDATE_event();
	}
	if(level == 2){
		move_cnt = WDT_MOVE2;				// level 2, speed 2
	}
	if(level == 3){

	}
	if(level == 4){

	}

	//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	//	Add code here to setup playing board for next level
	//	Draw snake, foods, reset timer, etc
						// ***demo only***
	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	game_mode = PLAY;					// start level
	return;
} // end START_LEVEL_event


//-- new game event ------------------------------------------------------------
//
void NEW_GAME_event(void)
{
	lcd_clear();						// clear lcd
	lcd_backlight(1);					// turn on backlight
	lcd_wordImage(snake1_image, (159-60)/2, 60, 1);
	lcd_wordImage(snake_text_image, (159-111)/2, 20, 1);


	// example foods
	lcd_diamond(COL(16), ROW(20), 2, 1);	// ***demo only***
	lcd_star(COL(17), ROW(20), 2, 1);
	lcd_circle(COL(18), ROW(20), 2, 1);
	lcd_square(COL(19), ROW(20), 2, 1);
	lcd_triangle(COL(20), ROW(20), 2, 1);

	game_mode = IDLE;
	return;
} // end NEW_GAME_event


//-- new snake -----------------------------------------------------------------
//
void new_snake(uint16 length, uint8 dir)
{
	int i;
	head = 0;
	tail = 0;
	snake[head].point.x = 0;
	snake[head].point.y = 0;
	direction = dir;

	// build snake
	score = length;
	for (i = score - 1; i > 0; --i)
	{
		add_head();
	}
	return;
} // end new_snake


//-- delete_tail  --------------------------------------------------------------
//
void delete_tail(void)
{
	lcd_point(COL(snake[tail].point.x), ROW(snake[tail].point.y), PENTX_OFF);
	tail = (tail + 1) & (~MAX_SNAKE);
} // end delete_tail


//-- add_head  -----------------------------------------------------------------
//
void add_head(void)
{
	static uint8 (*mFuncs[])(SNAKE*) =	// move head function pointers
	             { move_right, move_up, move_left, move_down };
	uint8 new_head = (head + 1) & (~MAX_SNAKE);
	snake[new_head] = snake[head];		// set new head to previous head
	head = new_head;
	// iterate until valid move
	while ((*mFuncs[direction])(&snake[head]));
} // end add_head


//-- move snake head right -----------------------------------------------------
//
uint8 move_right(SNAKE* head)
{
	if ((head->point.x + 1) < X_MAX)		// room to move right?
	{
		++(head->point.x);					// y, move right
		return FALSE;
	}
	if (level != 2)							// n, right fence
	{
		if (level > 2) sys_event = END_GAME;
		head->point.x = 0;					// wrap around
		return FALSE;
	}
	if (head->point.y) direction = DOWN;	// move up/down
	else direction = UP;
	return TRUE;
} // end move_right


//-- move snake head up --------------------------------------------------------
//
uint8 move_up(SNAKE* head)
{
	if ((head->point.y + 1) < Y_MAX)
	{
		++(head->point.y);					// move up
		return FALSE;
	}
	if (level != 2)							// top fence
	{
		if (level > 2) sys_event = END_GAME;
		head->point.y = 0;					// wrap around
		return FALSE;
	}
	if (head->point.x) direction = LEFT;	// move left/right
	else direction = RIGHT;
	return TRUE;
} // end move_up


//-- move snake head left ------------------------------------------------------
//
uint8 move_left(SNAKE* head)
{
	if (head->point.x)
	{
		--(head->point.x);					// move left
		return FALSE;
	}
	if (level != 2)							// left fence
	{
		if (level > 2) sys_event = END_GAME;
		head->point.x = X_MAX - 1;			// wrap around
		return FALSE;
	}
	if (head->point.y) direction = DOWN;	// move down/up
	else direction = UP;
	return TRUE;
} // end move_left


//-- move snake head down ------------------------------------------------------
//
uint8 move_down(SNAKE* head)
{
	if (head->point.y)
	{
		--(head->point.y);					// move down
		return FALSE;
	}
	if (level != 2)							// bottom fence
	{
		if (level > 2) sys_event = END_GAME;
		head->point.y = Y_MAX - 1;			// wrap around
		return FALSE;
	}
	if (head->point.x) direction = LEFT;	// move left/right
	else direction = RIGHT;
	return TRUE;
} // end move_down

//---------------------------FOOD Stuff-------------------------------------------

FOOD* newFood(uint16 x, uint16 y, uint8 value, uint8 size, void (*draw)(FOOD*))
{  FOOD* food = (FOOD*)malloc(sizeof(FOOD));
   if (!food) ERROR2(SYS_ERR_MALLOC);
   food->point.x = x;
   food->point.y = y;
   food->value = value;
   food->size = size;
   food->draw = draw;
   (food->draw)(food);
   return food;
} // end newFood

void food_fill(){
	int i;
	for(i = 0; i < 10; i++){
		food[i] = replace_food(rand() % 5);
	}
}

void food_col(){
	if(level < 3){
		int i;
		for(i = 0; i < 10; i++){
			if(snake[head].point.x == food[i]->point.x){
				if(snake[head].point.y == food[i]->point.y){
					hitFood = 1;
					currentScore += food[i]->value;
					if(level == 1){
						free(food[i]);
						food[i] = replace_food(i % 5);
					}
					else{
						food[i]->point.x = 50;
					}
					snakeSize++;
					foodCounter += 1;
					beep();
					blink();
					LCD_UPDATE_event();
				}
			}
		}
	}
	else{
		if(snake[head].point.x == food[0]->point.x){
			if(snake[head].point.y == food[0]->point.y){
				hitFood = 1;
				currentScore += food[0]->value;
				free(food[0]);
				food[0] = replace_food(rand()%5);
				snakeSize++;
				foodCounter+=1;
				beep();
				blink();
				LCD_UPDATE_event();
			}
		}
	}
}

FOOD* replace_food(int n){
	FOOD* f;
	if(n==0){
		f = newFood(rand() % X_MAX, rand() % Y_MAX, level, 2, draw_square_food);
		new_food_check(f);
		return f;
	}
	if(n==1){
		f = newFood(rand() % X_MAX, rand() % Y_MAX, level, 2, draw_triangle_food);
				new_food_check(f);
				return f;
	}
	if(n==2){
			f = newFood(rand() % X_MAX, rand() % Y_MAX, level, 2, draw_circle_food);
					new_food_check(f);
					return f;
		}
	if(n==3){
			f = newFood(rand() % X_MAX, rand() % Y_MAX, level, 2, draw_star_food);
					new_food_check(f);
					return f;
		}
	if(n==4){
			f = newFood(rand() % X_MAX, rand() % Y_MAX, level, 2, draw_diamond_food);
					new_food_check(f);
					return f;
		}
	return NULL;
}

void self_col(){
	int i;
	for(i = head-1; i >= tail; i--){
		if(snake[head].point.x == snake[i].point.x){
			if(snake[head].point.y == snake[i].point.y){
				hitSomething = 1;
			}
		}
	}
	return;
}

void new_food_check(FOOD* f){
	int i;
	for(i = 0; i < 10; i++){
		if(snake[i].point.x == f->point.x){
			if(snake[i].point.y == f->point.y){
				f = replace_food(rand() % 5);
			}
		}
	}
}

void rock_col(){

}

void draw_square_food(FOOD* food)
{  lcd_square(COL(food->point.x), ROW(food->point.y),
              food->size, SINGLE);
}

void draw_triangle_food(FOOD* food){
	lcd_triangle(COL(food->point.x), ROW(food->point.y), food->size, SINGLE);
}

void draw_circle_food(FOOD* food){
	lcd_circle(COL(food->point.x), ROW(food->point.y), food->size, SINGLE);
}

void draw_star_food(FOOD* food){
	lcd_star(COL(food->point.x), ROW(food->point.y), food->size, SINGLE);
}

void draw_diamond_food(FOOD* food){
	lcd_diamond(COL(food->point.x), ROW(food->point.y), food->size, SINGLE);
}
