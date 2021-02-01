/*
 * Labb1.c
 *
 * Created: 2021-01-25 11:22:42
 * Author : Hjalmar
 */ 

#include <avr/io.h>
#include <avr/portpins.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#define true	1
#define false	0

int characters[13] =
{
	0x1551,		// 0
	0x0118,		// 1
	0x1e11,		// 2
	0x1b11,		// 3
	0x0b50,		// 4
	0x1b41,		// 5
	0x1f41,		// 6
	0x4009,		// 7
	0x1f51,		// 8
	0x1b51,		// 9
	0x0f50,		// H
	0x1641,		// E
	0x1510		// J
};

int lastValue = 0;
int main(void)
{
	CLKPR = 0x80;	
	CLKPR =	0x00;	
	
	// Device Initialization values:
	
	LCD_init();
	PORTB = (1<<PORTB7);
	
    /* Replace with your application code */
	
	//blink();
	//writeChar(7, 0);
	//writeLong(700000);
    while (1) 
    {
		button();
    }
}

void button(void)
{
	if ((1<<PINB7) == 0 && lastValue == 0) //1<<PINB7 = Intryckt då 0, right?
	{
		lastValue = 1;
	}
	if(((1 << PINB7) == 1 && lastValue == 1))
	{
		lastValue = 0;
	}

	//He av skärmen då knappen är itryckt
	if (lastValue == 1)
	{
		LCDDR3 = LCDDR3 & 0x99 // slår av
	}
	else
	{
		LCDDR3 = LCDDR3 | 0x66 //slår på
	}
}


void LCD_init(void){
	LCDDC0 = 0;
	LCDDC1 = 0;
	LCDDC2 = 0; // 300 mikrosekunder
	LCDMUX0 = 1;
	LCDMUX1 = 1; // 1/3 bias och 1/4 duty
	LCDCRA = (1<<LCDEN) | (1<<LCDAB) | (1<<LCDIE); // 
	LCDCRB = (1<<LCDCS)  | (3<<LCDMUX0) | (7<<LCDPM0); // 
	LCDFRR = (0<<LCDPS0) | (7<<LCDCD0);	// sätter prescalern och ger framerate 32 Hz	
	LCDCCR |= 15; // sätter kontrastkontrollen till 3,35 V							
}

void writeChar(char ch, int pos){
	if(pos>5 | pos<0){
		return;
	}
	int shift;
	char mask_reg;
	int character = characters[(int)ch];
	char currbyte = 0x00;
	char *ptr;
	ptr  = &LCDDR0;
	
	if(pos & 0x01){
		mask_reg = 0x0f;
		shift = 4;
	}else{
		mask_reg = 0xf0;
		shift = 0;
	}
	ptr = ptr + (pos>>1);
	
	for(int i = 0; i < 4; i++){
		currbyte = (character & 0x0f);
		currbyte = currbyte << shift;
		*ptr = ((*ptr & mask_reg)|currbyte);
		character = (character>>4);
		ptr += 5;
	}
	
	
}

void writeLong(long i){
	long y = i;
	char ch;
	int cntr = 5;
	while(y>0){
		if(cntr<0){
			return;
		}
		ch = y%10;
		writeChar(ch, cntr);
		y = y/10;
		cntr--;
	}
	
}

int is_prime(long i){
	for(int x; x < i; x++){
		if(i%x == 0){
			return 0;
		}
	}
	return 1;
}

void primes(void){
	for(int i = 2; i <27001; i++){
		if(is_prime(i)){
			writeLong(i);
		}
	}
}

void blink(void){
	TCCR1B = TCCR1B|0x04; // detta ändrar CS12 till 1, vilket ändrar prescaling till 256
	int light = 0;
	unsigned int time = 31250;
	TCNT1 = 0x0000;
	
	while(1){
		
		if(TCNT1 == time){
			if(light){
				LCDDR0 = LCDDR0 & 0x99;
			}else{
				LCDDR0 = LCDDR0 | 0x66;
			}
			light = ~light;
			time += 31250;
		}
		if(TCNT1 > 62500){
			time = 31250;
		}
		
	}
	
}

void button(void)
{
	int lastValue = 0;
	while(1){
		if ((PINB7) == 1 && lastValue == 0) //PINB7 = Intryckt då 1
		{
			lastValue = 1;
		}
		if(((PINB7) == 0 && lastValue == 1))
		{
			lastValue = 0;
		}

		if (lastValue == 1)
		{
			LCDDR3 = LCDDR3 & 0x99 // slår av
		}
		else
		{
			LCDDR3 = LCDDR3 | 0x66 //slår på
		}
	}
}

void partFour(void){ //då två av funktionerna har busy-wait måste vi slå ihop koden från de två i en while-loop
	long num = 2;
	TCCR1B = 0x04;
	TCNT1 = 0x0000;
	int lastValue = 0;
	int light = 0;
	int time = 31250;
	
	while(1){
		
		if(is_prime(num)){
			writeLong(num);
		}
		
		if ((PINB7) == 1 && lastValue == 0) //1<<PINB7 = Intryckt d? 0, right?
		{
			lastValue = 1;
		}
		if(((PINB7) == 0 && lastValue == 1))
		{
			lastValue = 0;
		}

		//He av sk?rmen d? knappen ?r itryckt
		if (lastValue == 1)
		{
			LCDDR3 = LCDDR3 & 0x99 // sl?r av
		}
		else
		{
			LCDDR3 = LCDDR3 | 0x66 //sl?r p?
		}
		
		if(TCNT1 >= time){
			if(light){
				LCDDR0 = LCDDR0 & 0x99;
				}else{
				LCDDR0 = LCDDR0 | 0x66;
			}
			light = ~light;
			time += 31250;
		}
		if(TCNT1 < 31250 && time > 62500){
			time = 31250;
		}
		num += 1;
	}
}