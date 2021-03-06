/*
 * Labb1.c
 *
 * Created: 2021-01-25 11:22:42
 * Author : Hjalmar
 */ 

#include <avr/io.h>
#include <avr/portpins.h>
#include <stdint.h>


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


int main(void)
{
	CLKPR = 0x80;	
	CLKPR = 0x00;	
	
	LCD_init();
	//PORTB = (1<<PORTB7);
	
    /* Replace with your application code */

	primes(30000);
	//button();
	//blink();
	//partFour();
    while (1) 
    {
		
    }
}

void LCD_init(void){
	LCDCRA |= 0x80; // LCD enable
	LCDCRB = 0xb7; // 1/3 bias och 1/4 duty, asynk-klockan anv�nds och 25 segment anv�nds
	LCDCCR |= 15; // s�tter kontrastkontrollen till 3,35 V
	LCDFRR = 7;	// s�tter prescalern och ger framerate 32 Hz								
}

void writeChar(char ch, int pos){
	if((pos>5) | (pos<0)){
		return;
	}
	int shift;
	char mask_reg;
	char currbyte = 0x00;
	int character = 0;
	char *ptr;
	ptr  = &LCDDR0; // pekaren b�rjar p� lcddr0
	
	if((int)ch > -1 && (int)ch < 10){
		character = characters[(int)ch];
	}
	
	
	if(pos & 0x01){ // om pos �r udda finns det en etta p� slutet, d� ska den skrivas p� de fyra bitarna till v�nster p� registren
		mask_reg = 0x0f;
		shift = 4; // bitarna som ska skrivas beh�ver �ven flyttas f�r att skrivas p� v�nster sida
	}else{
		mask_reg = 0xf0;
		shift = 0;
	}
	ptr = ptr + (pos>>1); // pekaren hamnar p� lcddr0, 1 eller 2 beroende p� pos
	
	for(int i = 0; i < 4; i++){
		currbyte = (character & 0x0f); // vi tar de 4 bittarna l�ngst till h�ger
		currbyte = currbyte << shift; // bittarna shiftas antingen 0 eller 4 steg �t v�nster
		*ptr = ((*ptr & mask_reg)|currbyte); // registret maskas och fylls sedan in med de bittarna vi h�mtat
		character = (character>>4); // vi tar bort de 4 bittarna till h�ger
		ptr += 5; // pekaren g�r fem register fram f�r n�sta 4 bittar
	}
}

void writeLong(long i){
	long y = i;
	char ch;
	int cntr = 5;
	while(y>0){ // s� l�nge talet �r st�rrer �n noll eller om vi inte skrivit 6 siffror ska en siffra skrivas
		if(cntr<0){
			return;
		}
		ch = y%10; // modulo 10 f�r att f� ut siffran till h�ger
		writeChar(ch, cntr); // skriv siffran till h�ger
		y = y/10; // vi tar bort siffran l�ngst till h�ger
		cntr--; // vi b�rjar till h�ger och g�r sedan �t v�nster d�rifr�n
	}
	
}

int is_prime(long i){
	for(int x = 2; x < i; x++){
		if(i%x == 0){
			return 0;
		}
	}
	return 1;
}

void primes(long i){
	for(int x = 2; x < i; x++){
		if(is_prime(x)){
			writeLong(x);
		}
	}
}

void blink(void){
	TCCR1B = TCCR1B|0x04; // detta �ndrar CS12 till 1, vilket �ndrar prescaling till 256
	int light = 0; // light best�mmer om lampan �r av eller p�
	unsigned short time = 15625; // 8000000/256 = 31250, f�r en sekund, allts� 15625 f�r en blinkning
	// short �r 2 byte, precis som timern, allts� kommer den att b�rja om p� noll lika fort som timerregistret
	//TCNT1 = 0x0000;
	
	while(1){
		
		if(TCNT1 == time){
			if(light){
				LCDDR0 = LCDDR0 & 0x99; // om den �r p� sl�r vi av den
			}else{
				LCDDR0 = LCDDR0 | 0x60; // annars sl�r vi p� den
			}
			light = ~light; // vi �ndrar light f�r att indikera att lampan �r av/p�
			time += 15625;
		}
		
	}
	
}

void button(void)
{
	int buttonpress = 0;
	LCDDR0 |= 0x06;
	while(1)
	{
		if (!(PINB&0x80) && buttonpress == 0) // PINB7 = 0, n�r den �r intryckt
		{									  // vi byter l�ge endast d� knappen �r nedtryckt och den nyss inte var det
			buttonpress = 1;
			if ((LCDDR0&0x06)) // vi ser om det ena l�get �r p�
			{
				LCDDR0 = LCDDR0 & 0xf9; // sl�r av
				LCDDR1 = LCDDR1 | 0x60; // sl�r p�
			}
			else // annars �r det ju det andra l�get
			{
				LCDDR0 = LCDDR0 | 0x06; // sl�r p�
				LCDDR1 = LCDDR1 & 0x9f; // sl�r av
			}
		}
		if((PINB&0x80) && buttonpress == 1) // om den inte �r nedtryckt blir buttonpress noll
		{
			buttonpress = 0;
		}

		
		
	}
}

void partFour(void){ //d� tv� av funktionerna har busy-wait m�ste vi sl� ihop koden fr�n de tv� i en while-loop
	long num = 25000;
	TCCR1B = 0x04;
//	TCNT1 = 0x0000;
	int buttonpress = 0;
	int light = 0;
	unsigned short time = 15625;
	LCDDR0 |= 0x06;
	
	while(1){
		
		if(is_prime(num)){	// vi tar primtalen f�rst
			writeLong(num);
		}
		
		if (!(PINB&0x80) && buttonpress == 0) 
		{
			if ((LCDDR0&0x06))
			{
				LCDDR0 = LCDDR0 & 0xf9; 
				LCDDR3 = 1;
			}
			else
			{
				LCDDR0 = LCDDR0 | 0x06; 
				LCDDR3 = 0;
			}
			buttonpress = 1;
		}
		if((PINB&0x80)) 
		{
			buttonpress = 0;
		}
		
		if(TCNT1 >= time){	// sen timern
			if(light){
				LCDDR0 = LCDDR0 & 0x9f; //sl�r av
				}else{
				LCDDR0 = LCDDR0 | 0x60; // sl�r p�
			}
			light = ~light;
			time += 15625;
		}
		
		num += 1;
	}
}