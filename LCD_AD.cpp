

/***********************************************************************/
#define		F_CPU 1000000UL
#define		RESISTOR_CONST		33000	// Known resistor value
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
void adc_init();
uint32_t adc_read();
void LCD_custom_char(const char custom[],uint16_t cgram_addr, uint16_t lcd_addr, int char_num );
uint32_t calc_res(uint32_t adc);




int main(void){
	
	const char omega[8] = {0x0, 0x0, 0xe, 0x11, 0x11, 0x11, 0xa, 0x1b};
	char int_buffer1[10];
	char int_buffer2[10];
	char int_buffer3[10];
	volatile double raw;
	volatile int intValue;
	volatile double diffValue;
	volatile int otherDiffValue;
	
	lcd_init();
	adc_init();
	lcd_command(0x89);
	lcd_char('V');
	
	LCD_custom_char(omega,BASE_CGRAM_ADDR,0xc9,0);
/*	
	LCD_custom_char(box,BASE_CGRAM_ADDR,0x80,0);
	LCD_custom_char(ghost,BASE_CGRAM_ADDR+1*8,0x82,1);
	LCD_custom_char(heart,BASE_CGRAM_ADDR+2*8,0x84,2);
	LCD_custom_char(sword,BASE_CGRAM_ADDR+3*8,0x86,3);
	LCD_custom_char(shield,BASE_CGRAM_ADDR+4*8,0x88,4);
	LCD_custom_char(dog,BASE_CGRAM_ADDR+5*8,0x8a,5);
	LCD_custom_char(song,BASE_CGRAM_ADDR+6*8,0x8c,6);
	LCD_custom_char(hey,BASE_CGRAM_ADDR+7*8,0x8d,7);
	lcd_command(0xc0); */
  
	
	for(;;){	
		for(;;){
		lcd_command(0x02);	
		lcd_command(0x0C);
		
		raw = (adc_read()*4.995)/1024.0;
		intValue = (int)raw;
		itoa(intValue, int_buffer1, 10);
		lcd_msg(int_buffer1);
		lcd_char('.');
		
	
		diffValue = raw - (double)intValue;
		otherDiffValue = (int)(diffValue*1000.0);
		utoa(otherDiffValue,int_buffer2,10);
		lcd_msg(int_buffer2);
		lcd_command(0x87);
		
		
		lcd_command(0xC0); 

				
		volatile double coeff = raw/4.995;
		volatile double temp = (coeff*RESISTOR_CONST)/(1-coeff);
		utoa(temp,int_buffer3,10);		
		lcd_msg(int_buffer3);
	
		
		_delay_ms(10);
	}
	
	

	

	_delay_ms(10);
	return 0;
}
	}


void lcd_init(){

	DDRD |= 0xCF; //Change DDRA --> DDRB and PORTD --> PORTB if LCD

	lcd_command(0x33); //Initialize LCD Driver
	lcd_command(0x32); //Four bit mode
	lcd_command(0x28); //2 Line Mode
	lcd_command(0x0f); //Display On, Cursor Off, Blink Off Change to 0x0F
	lcd_command(0x01); //Clear Screen, Cursor Home
	
}

void toggle_E(uint16_t code){
	_delay_ms(2);
	code ^= 0x80; //E --> 0
	PORTD = code;
	_delay_ms(5);
}

void lcd_command(char cmd){

	char temp = cmd;
	PORTD = 0X00;
	_delay_ms(2);
	cmd = ( (cmd & 0xF0) >> 4) | 0x80; //Write Upper Nibble (RS=0) E --> 1
	PORTD = cmd;
	
	toggle_E(cmd);
	
	cmd = temp;
	cmd = ( (cmd & 0x0F) ) | 0x80; //Write Lower Nibble (RS=0) E --> 1
	PORTD = cmd;
	
	toggle_E(cmd);
}


void lcd_char(char data){

	char temp = data;
	PORTD = 0x40;
	_delay_ms(2);
	data = ( (data & 0xF0) >> 4) | 0xC0; //Write Upper Nibble (RS=1) E --> 1
	PORTD = data;
	
	toggle_E(data);
	
	data = temp;
	data = ( (data & 0x0F) ) | 0xC0; //Write Lower Nibble (RS=1) E --> 1
	PORTD = data;
	
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


	void adc_init()
	{
		// AREF = AVcc
		ADMUX = (1<<REFS0);
		
		// ADC Enable and prescaler of 128
		// 16000000/128 = 125000
		ADCSRA = (1<<ADEN)|(1<<ADPS1)|(1<<ADPS0);
	}


uint32_t adc_read()
{

	ADCSRA |= (1<<ADSC);
	
	// wait for conversion to complete
	// ADSC becomes ’0? again
	// till then, run loop continuously
	while(ADCSRA & (1<<ADSC));
	
	return (ADC);
}


uint32_t calc_res(uint32_t adc){
	
}



