#include "LCD.h"
#include "stm32l476xx.h"

#define E 0x2 //|=E on , &=~E off
#define RS 0x1

void delay_ms(unsigned int ms) {
	// Delay 10 X n Millisecond program 
	volatile unsigned int i,j; 
	for(i=0; i<ms; i++){ 
    for(j=0; j<100; j++);
  } 
}

void Debounce() {
	delay_ms(100);
}


void LCD_WriteCom(unsigned char com) {
  unsigned char com1,com2; 
	
  com1 = com & 0xF0; 
  com2 = (com<<4) & 0xF0; 
	
  GPIOB->ODR &= ~E; //	E=0; 
	GPIOB->ODR &= ~RS; // 	RS=0; 
	
	GPIOB->ODR &= 0xFFFFFF0F; //Set wanted bits to 0
	GPIOB->ODR |= com1; // 	DB07=com1; 
	
	GPIOB->ODR |= E; //	E=1; 
	delay_ms(100);  //Delay 100 milliseconds
	GPIOB->ODR &= ~E; //	E=0; 
	GPIOB->ODR &= ~RS; // 	RS=0;  
	
	GPIOB->ODR &= 0xFFFFFF0F; //Set wanted bits to 0
	GPIOB->ODR |= com2; // 	DB07=com2;  
	
	GPIOB->ODR |= E; //	E=1; 
	delay_ms(100);  //Delay 100 milliseconds 
	GPIOB->ODR &= ~E; //	E=0; 
	delay_ms(100);  //Delay 100 milliseconds 
}


void LCD_WriteData(unsigned char dat) {
	unsigned char dat1,dat2; 
  dat1 = dat & 0xf0; 
  dat2 = (dat<<4) & 0xf0; 
	
  GPIOB->ODR &= ~E; //	E=0; 
	GPIOB->ODR |= RS; // 	RS=1; 
	
	GPIOB->ODR &= 0xFFFFFF0F; //Set wanted bits to 0
	GPIOB->ODR |= dat1; // 	DB07=com1; 
	
	GPIOB->ODR |= E; //	E=1; 
	delay_ms(100);  //Delay 100 milliseconds
	GPIOB->ODR &= ~E; //	E=0; 
	GPIOB->ODR |= RS; // 	RS=1;  
	
	GPIOB->ODR &= 0xFFFFFF0F; //Set wanted bits to 0
	GPIOB->ODR |= dat2; // 	DB07=com1;  
	
	GPIOB->ODR |= E; //	E=1; 
	delay_ms(100);  //Delay 100 milliseconds 
	GPIOB->ODR &= ~E; //	E=0; 
	delay_ms(100);  //Delay 100 milliseconds
}

void LCD_Init(void){
	RCC->CR |= RCC_CR_HSION;
	while((RCC->CR & RCC_CR_HSIRDY) == 0);  //HSI clock enable
	
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN; //GPIOB clock enable
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN; //GPIOC clock enable
	
	
	GPIOB->MODER &= 0xFFFF00F0; 
	GPIOB->MODER |= 0x00005505;  //Sets GPIOB[7:4] and GPIOB[1:0] MODER to 01/output mode for LCD
	
  GPIOC->MODER &= 0xFFFFFC00; 
	GPIOC->MODER |= 0x00000000;  //Sets GPIOC[4:0] MODER to 00/input mode for buttons
	
	GPIOC->PUPDR &= 0xFFFFFC00; 
	GPIOC->PUPDR |= 0x00000155;  //Sets GPIOC[4:0] PUPDR to 01/pull-up mode buttons 
	
	
	LCD_WriteCom(0x28); //4 bit mode, 2-line display mode, 5x8 dot matrix
	
	LCD_WriteCom(0x01); //Clear display
	
	LCD_WriteCom(0x06);//Cursor moves left to right, no display scrolling
	
	LCD_WriteCom(0x0C); //Display on, cursor off, cursor blink off
	
	LCD_WriteCom(0x80); //Set up DDRAM address to start at 0
}


void LCD_Clear(void){
  LCD_WriteCom(0x01); //Clear display
}


void LCD_DisplayString(unsigned int line, unsigned char *ptr) { //Take in string as a pointer to an array
	 unsigned char i; 
	   
	if (line == 0){                           //Go to line 1 and read in string, start at index 00, write data to LCD           
   LCD_WriteCom(0x80); 
   for(i=0; ptr[i]!='\0'; i++){ 
      LCD_WriteData(ptr[i]);
	}
}
	else
	{
	 LCD_WriteCom(0xC0);                   //Go to line 2 and read in string, start at index 40 write data to LCD 
    for(i=0; ptr[i]!='\0'; i++){
      LCD_WriteData(ptr[i]);
   }
  }
 }


