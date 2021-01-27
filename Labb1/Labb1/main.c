/*
 * Labb1.c
 *
 * Created: 2021-01-25 11:22:42
 * Author : Hjalmar
 */ 

#include <avr/io.h>
#include <avr/portpins.h>
#include <stdbool.h>
#define true	1
#define false	0


int characters[13] =
{
	0x1551,		// 0
	0x0110,		// 1
	0x1e11,		// 2
	0x1b11,		// 3
	0x0b50,		// 4
	0x1b41,		// 5
	0x1f41,		// 6
	0x0111,		// 7
	0x1f51,		// 8
	0x1b51,		// 9
	0x0f50,		// H
	0x1641,		// E
	0x1510		// J
};

int main(void)
{
	CLKPR = 0x80;	
	CLKPR =	0x00;	
	
	// Device Initialization values:
	
	LCD_init();
	
	PORTB = (1 << PORTB7);			//Button Setting.
    /* Replace with your application code */
	int thing = (19%10);
	writeLong(123456789);
    while (1) 
    {
		
    }
}

void LCD_init(void){
	LCDCRA = 0xc0; // (1<<LCDEN);						
	LCDCRB = 0xb7; //(1<<LCDCS)  | (1<<LCDMUX1) | (1<<LCDPM2);		
	LCDFRR = 0x07; //(1<<LCDCD1) | (1<<LCDCD2);					
	LCDCCR = 0x0f; //(1<<LCDDC1) | (1<<LCDCC3) | (1<<LCDCC2) | (1<<LCDCC1);								
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
	int ch;
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
	int x = 2;
	while(x < i){
		if(i%x == 0){
			return 0;
		}
	}
	return 1;
}