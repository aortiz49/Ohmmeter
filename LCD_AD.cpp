

/***********************************************************************/
#define		F_CPU 1000000UL
#define		CLR_DISP_HOME		0x01	// Clear display, cursor home
#define		CURSOR_HOME			0x02	// Cursor home
#define		CURSOR_DEC_SOFF		0x03	// Decrement cursor, shift off
#define		CURSOR_DEC_SON		0X04	// Decrement cursor, shift on
#define		CURSOR_INC_SOFF		0x05	// Increment cursor, shift off
#define		CURSOR_INC_SON		0X06	// Increment cursor, shift on
#define		BASE_CGRAM_ADDR		0x40	// 1st address in CGRAM
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

void LCD_custom_char(const char custom[],uint16_t cgram_addr, uint16_t lcd_addr, int char_num );



int main(void){
	const char box[8] = {0x0, 0xe, 0x1b, 0x1e, 0x1c, 0x1e, 0x1f, 0xe};
	const char ghost[8] = {0x0, 0xe, 0xe, 0x1f, 0x15, 0x1f, 0x1f, 0x11};
	const char heart[8] = {0x0, 0xa, 0x1e, 0x1f, 0xe, 0x4, 0x0, 0x0};
	const char sword[8] = {0xe, 0x1b, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f};
	const char shield[8] ={0x4, 0xe, 0xe, 0xe, 0x1f, 0x1f, 0x0, 0x4};
	const char dog[8] = {0xa, 0xa, 0x1f, 0x11, 0x11, 0xe, 0x4, 0x4};
	const char song[8] = {0x3, 0x5, 0x9, 0x9, 0xb, 0xb, 0x18, 0x18};
	const char hey[8] = {0x1f, 0x1f, 0x00, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f};
	
	DDRD |= 0xFF;
	lcd_init();
	PORTD= 0xFF;
	
	LCD_custom_char(box,BASE_CGRAM_ADDR,0x80,0);
	LCD_custom_char(ghost,BASE_CGRAM_ADDR+1*8,0x82,1);
	LCD_custom_char(heart,BASE_CGRAM_ADDR+2*8,0x84,2);
	LCD_custom_char(sword,BASE_CGRAM_ADDR+3*8,0x86,3);
	LCD_custom_char(shield,BASE_CGRAM_ADDR+4*8,0x88,4);
	LCD_custom_char(dog,BASE_CGRAM_ADDR+5*8,0x8a,5);
	LCD_custom_char(song,BASE_CGRAM_ADDR+6*8,0x8c,6);
	LCD_custom_char(hey,BASE_CGRAM_ADDR+7*8,0x8e,7);
	lcd_command(0xc0);
	lcd_msg("custom char gen!");
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


void LCD_custom_char(const char custom[],uint16_t cgram_addr, uint16_t lcd_addr, int char_num ){
	lcd_command(cgram_addr);  // Address where customized character is to be stored
	for(int i = 0; i < 8; i++)
		lcd_char(custom[i]);
	
	lcd_command(lcd_addr); // Location of LCD where the character is to be displayed (0x80 - 0x8F) row 1 and (0xC0 - 0xCF) row 2
	// Sets RS = 1 and address 0-7
	lcd_char(char_num);
	_delay_ms(10);
	
}



