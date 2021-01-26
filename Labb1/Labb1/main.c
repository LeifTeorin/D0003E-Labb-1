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
	0x1551,		// 0 = 1551, A = 0F51
	0x0110,		// 1
	0x1e11,		// 2
	0x1B11,		// 3
	0x0B50,		// 4
	0x1B41,		// 5
	0x1F41,		// 6
	0x0111,		// 7
	0x1F51,		// 8
	0x1B51,		// 9
	0x0f50,		// H
	0x1641,		// E
	0x1510		// J
};

int main(void)
{
	CLKPR = 0x80;	//(1<<CLKPCE);
	CLKPR =	0x00;	//(0<<CLKPS3);
	
	// Device Initialization values:
	
	LCD_init();
	
	PORTB = (1 << PORTB7);			//Button Setting.
    /* Replace with your application code */
	
    while (1) 
    {
		writeChar(4, 0);
		writeChar(2, 1);
		writeChar(0, 2);
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
		character = (character>>4);
		*ptr = ((*ptr & mask_reg)|currbyte);
		ptr += 5;
	}
	
	
}


