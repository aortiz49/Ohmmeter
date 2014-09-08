

/***********************************************************************/
#define		F_CPU 1000000UL
#define		CLR_DISP_HOME		0x01	// Clear display, cursor home
#define		CURSOR_HOME			0x02	// Cursor home
#define		CURSOR_DEC_SOFF		0x03	// Decrement cursor, shift off
#define		CURSOR_DEC_SON		0X04	// Decrement cursor, shift on
#define		CURSOR_INC_SOFF		0x05	// Increment cursor, shift off
#define		CURSOR_INC_SON		0X06	// Increment cursor, shift on
/***********************************************************************/

#include <avr/io.h>
#include <string.h>
#include <stdlib.h>
#include <util/delay.h>


void lcd_command(char cmd);

void lcd_char(char data);

void lcd_msg(char msg[]); //went fron char *msg to char msg[]

void toggle_E();

void lcd_init();


int main(void){
	DDRD |= 0xFF;
	lcd_init();
	lcd_msg("problem");
	PORTD= 0xFF;
	
	return 0;
}


void lcd_init(){

	DDRA |= 0xCF; //Change DDRA --> DDRB and PORTA --> PORTB if LCD

	lcd_command(0x33); //Initialize LCD Driver
	lcd_command(0x32); //Four bit mode
	lcd_command(0x28); //2 Line Mode
	lcd_command(0x0f); //Display On, Cursor Off, Blink Off Change to 0x0F
	lcd_command(0x01); //Clear Screen, Cursor Home
	/// This initialized the 4-vit mode of the LCD
}

void toggle_E(uint16_t code){
	_delay_ms(2);
	code ^= 0x80; //E --> 0
	PORTA = code;
	_delay_ms(5);
}

void lcd_command(char cmd){

	char temp = cmd;
	PORTA = 0X00;
	_delay_ms(2);
	cmd = ( (cmd & 0xF0) >> 4) | 0x80; //Write Upper Nibble (RS=0) E --> 1
	PORTA = cmd;
	
	toggle_E(cmd);
	
	cmd = temp;
	cmd = ( (cmd & 0x0F) ) | 0x80; //Write Lower Nibble (RS=0) E --> 1
	PORTA = cmd;
	
	toggle_E(cmd);
}


void lcd_char(char data){

	char temp = data;
	PORTA = 0x40;
	_delay_ms(2);
	data = ( (data & 0xF0) >> 4) | 0xC0; //Write Upper Nibble (RS=1) E --> 1
	PORTA = data;
	
	toggle_E(data);
	
	data = temp;
	data = ( (data & 0x0F) ) | 0xC0; //Write Lower Nibble (RS=1) E --> 1
	PORTA = data;
	
	toggle_E(data);
	
}

void lcd_msg(char msg[]){	// Display chars of string one a time until message is there
	//unsigned char length = strlen(msg)
	for(int i = 0; msg[i] != NULL;i++) //for(int i = 0; i < length; i++)
	lcd_char(msg[i]);
	
}





