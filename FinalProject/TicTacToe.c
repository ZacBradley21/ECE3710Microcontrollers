#include "stm32l476xx.h"
#include "LCD.h"
#include <string.h>

void USART_Init(void);
void USART_Write(volatile uint8_t *buffer, uint32_t nBytes);
void USART_Read (volatile uint8_t *buffer, uint32_t nBytes);
void printBoard(char board[3][3]);
char checkWinner(void);
void readButtons(void);
void WriteBoard(void);
void writeTicTacToe(void);
void writeGameOver(void);

void USART_Init(void) {

   //Disable USART
   USART2->CR1 &= ~USART_CR1_UE;
   
   RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
   
   RCC->CCIPR &= ~(RCC_CCIPR_USART2SEL);
   RCC->CCIPR |= ~(RCC_CCIPR_USART2SEL_0);
   
   //Set data Length to 8 bits
   //00 = B data bits, 01 = 9 data bits, 10 = 7 data bits
   USART2->CR1 &= ~USART_CR1_M;
   
   //Select 1 stop bit
   //00 = 1 stop bit 01 = 0.5 stop bit
   //10 = 2 Stop bits 11 = 1.5 Stop bit
   USART2->CR2 &= ~USART_CR2_STOP;
   
   //Set parity control as no parity
   //0 = no parity,
   //1 = parity enabled (then, program PS bit to select Even or Odd parity)
   USART2->CR1 &= ~USART_CR1_PCE;
   
   //Oversampling by 16
   //0 =oversampling by 16, 1 =oversampling by 8
   USART2->CR1 &= ~USART_CR1_OVER8;
   
   //Set Baud rate to 9600 using APB frequency (Be MHz)
   //See Example 1 in Section 22.1.2
   USART2->BRR = 0x683;
   
   //Enable transmission and reception
   USART2->CR1 |= (USART_CR1_TE | USART_CR1_RE);
   
   //Enable USART
   USART2->CR1 |= USART_CR1_UE;
   
   //Verify that USART is ready for transmission
   //TEACK: Transmit enable acknowledge flag. Hardware sets or resets it.
   while ((USART2->ISR & USART_ISR_TEACK) == 0);
   
   
   //Verify that USART is ready for reception
   //REACK: Receive enable acknowledge flag. Hardware sets or resets it.
      while ((USART2->ISR & USART_ISR_REACK) == 0);
}

void USART_Write(volatile uint8_t *buffer, uint32_t nBytes) {
   int i;
   for(i = 0; i < nBytes; i++) {
      while (!(USART2->ISR & USART_ISR_TXE));
      USART2->TDR = buffer[i] & 0xFF;
      
      while (!(USART2->ISR & USART_ISR_TC));
      USART2->ICR |= USART_ICR_TCCF;
      
   }
}

void USART_Read(volatile uint8_t *buffer, uint32_t nBytes){
   unsigned int i;
   for(i = 0; i < nBytes; i++) {
      while (!(USART2->ISR & USART_ISR_RXNE));
      buffer[i] = USART2->RDR;
   }
}

char board[3][3] = {
     {' ', ' ', ' '},
     {' ', ' ', ' '},
     {' ', ' ', ' '}
};

	int player = 1;
	int cursorx = 0;
	int cursory = 0;
	char temp = 0;
	char winner = 0;


void readButtons() {
	
    // Check if the LEFT button (GPIOC 0) is pressed
    if (!(GPIOC->IDR & 4)) {
			if (cursory != 0) {
					//board[cursorx][cursory] = ' ';
					board[cursorx][cursory] = temp;
					cursory -= 1; // Move left
					Debounce();
			}
    }

    // Check if the RIGHT button (GPIOC 1) is pressed
    if (!(GPIOC->IDR & 8)) {
			if (cursory != 2) {
					board[cursorx][cursory] = temp;
					cursory += 1; // Move right
					Debounce();
			}
    }

    // Check if the UP button (GPIOC 2) is pressed
    if (!(GPIOC->IDR & 1)) {
			if (cursorx != 0) {
					board[cursorx][cursory] = temp;
					cursorx -= 1; // Move up
					Debounce();
			}
    }

    // Check if the DOWN button (GPIOC 3) is pressed
    if (!(GPIOC->IDR & 2)) {
			if (cursorx != 2) {
					board[cursorx][cursory] = temp;
					cursorx += 1; // Move down
					Debounce();
			}
    }
		
		// Check if the SELECT button (GPIOC 4) is pressed
    if (!(GPIOC->IDR & 16)) {
			// Check if the selected cell is empty
            if (player == 1) {
							if (temp == ' '){
                board[cursorx][cursory] = 'X'; // Player 1 marks 'X'
								winner = checkWinner();
								if (winner == 'X'){
									LCD_DisplayString(0, "Player X wins");
									writeGameOver();
								}
								else if (winner == 'D'){
									LCD_DisplayString(0, "It's a Draw");
									writeGameOver();
								}
                player = 2;           // Switch to Player 2
							}
						} 
						else {
              if (temp == ' '){  
								board[cursorx][cursory] = 'O'; // Player 2 marks 'O'
								winner = checkWinner();
								if (winner == 'O'){
									LCD_DisplayString(0, "Player O wins");
									writeGameOver();
								}
								else if (winner == 'D'){
									LCD_DisplayString(0, "It's a Draw");
									writeGameOver();
								}
                player = 1;           // Switch to Player 1
            }
					}
        Debounce();
				Debounce();
		}
}

	void WriteBoard() {
		char Row1[12] =  {' ', 'X', ' ', '|', ' ', 'X', ' ', '|',' ', 'X', ' ', '\n'};
		char Row2[12] =  {' ', 'X', ' ', '|', ' ', 'X', ' ', '|',' ', 'X', ' ', '\n'};
		char Row3[12] =  {' ', 'X', ' ', '|', ' ', 'X', ' ', '|',' ', 'X', ' ', '\n'};
		char Row4[12] =  {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', ' ', ' ', '\n'};
		char Row5[12] =  {'-', '-', '-', '+', '-', '-', '-', '+','-', '-', '-', '\n'};
			
		
		if (player == 1) {
			if(board[cursorx][cursory] != 'x'){
				temp = board[cursorx][cursory];
			}
			board[cursorx][cursory] = 'x'; //This just shows player 1's x blinking on the screen to show what will be placed
		}//Check X before placing next o, same with O, check O before placing next x
		//The cursor will affect the game logic if we do it any other way
		//winner = checkWinner();
		//LCD_DisplayString(0, winner);
		if (player == 2) {
			if(board[cursorx][cursory] != 'o'){
				temp = board[cursorx][cursory];
			}
			board[cursorx][cursory] = 'o'; //This just shows player 2's o blinking on the screen to show what will be placed
		}
		//winner = checkWinner();
		//LCD_DisplayString(0, winner);
		
		Row1[1] = board[0][0];
		Row1[5] = board[0][1];
		Row1[9] = board[0][2];
		USART_Write(Row1, 12);
		USART_Write(Row5, 12);		
		Row2[1] = board[1][0];
		Row2[5] = board[1][1];
		Row2[9] = board[1][2];
		USART_Write(Row2, 12);
		USART_Write(Row5, 12);
		Row3[1] = board[2][0];
		Row3[5] = board[2][1];
		Row3[9] = board[2][2];
		USART_Write(Row3, 12);
		USART_Write(Row4, 12);
		USART_Write(Row4, 12);
		USART_Write(Row4, 12);
		USART_Write(Row4, 12);
		USART_Write(Row4, 12);
		USART_Write(Row4, 12);
		USART_Write(Row4, 12);
		USART_Write(Row4, 12);
		USART_Write(Row4, 12);
	}
	
	void writeTicTacToe() {
		char Tic[25] =  {'8', '8', '8', '8', '8', '8', '8', '8', '8', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\n'};
		char Tic1[25] = {'"', '8', '8', '8', '8', '8', '8', '8', '"', ' ', 'o', 'o', 'o', 'o', 'o', ' ', ' ', 'o', 'o', 'o', 'o', 'o', ' ', ' ', '\n'};
		char Tic2[25] = {' ', ' ', ' ', '8', '8', '8', ' ', ' ', ' ', ' ', ' ', '8', '8', '8', ' ', ' ', '8', '8', ' ', ' ', ' ', '8', ' ', ' ', '\n'};
		char Tic3[25] = {' ', ' ', ' ', '8', '8', '8', ' ', ' ', ' ', ' ', ' ', '8', '8', '8', ' ', ' ', '8', '8', ' ', ' ', ' ', ' ', ' ', ' ', '\n'};
		char Tic4[25] = {' ', ' ', ' ', '8', '8', '8', ' ', ' ', ' ', ' ', ' ', '8', '8', '8', ' ', ' ', '8', '8', ' ', ' ', ' ', ' ', '8', '8', '\n'};
		char Tic5[25] = {' ', ' ', ' ', '8', '8', '8', ' ', ' ', ' ', ' ', '8', '8', '8', '8', '8', ' ', ' ', '8', '8', '8', '8', '8', '8', '"', '\n'};
		char newline[1] = {'\n'};
			
		char Tac[25] =  {'8', '8', '8', '8', '8', '8', '8', '8', '8', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\n'};
		char Tac1[25] = {'"', '8', '8', '8', '8', '8', '8', '8', '"', ' ', 'o', 'o', 'o', ' ', ' ', ' ', ' ', 'o', 'o', 'o', 'o', 'o', ' ', ' ', '\n'};
		char Tac2[25] = {' ', ' ', ' ', '8', '8', '8', ' ', ' ', ' ', ' ', '8', '8', '8', ' ', ' ', ' ', '8', '8', ' ', ' ', ' ', '8', ' ', ' ', '\n'};
		char Tac3[25] = {' ', ' ', ' ', '8', '8', '8', ' ', ' ', ' ', '8', 'o', 'o', 'o', '8', '8', ' ', '8', '8', ' ', ' ', ' ', ' ', ' ', ' ', '\n'};
		char Tac4[25] = {' ', ' ', ' ', '8', '8', '8', ' ', ' ', ' ', '8', '8', ' ', ' ', '8', '8', ' ', '8', '8', ' ', ' ', ' ', ' ', '8', '8', '\n'};
		char Tac5[25] = {' ', ' ', ' ', '8', '8', '8', ' ', ' ', ' ', '8', '8', ' ', ' ', '8', '8', ' ', ' ', '8', '8', '8', '8', '8', '8', '"', '\n'};
			
		char Toe[27] =  {'8', '8', '8', '8', '8', '8', '8', '8', '8', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\n'};
		char Toe1[27] = {'"', '8', '8', '8', '8', '8', '8', '8', '"', ' ', ' ', 'o', 'o', 'o', ' ', ' ', ' ', ' ', '8', '8', '8', '8', '8', '8', '8', ' ', '\n'};
		char Toe2[27] = {' ', ' ', ' ', '8', '8', '8', ' ', ' ', ' ', '8', '8', ' ', ' ', ' ', '8', '8', ' ', ' ', '8', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\n'};
		char Toe3[27] = {' ', ' ', ' ', '8', '8', '8', ' ', ' ', ' ', '8', '8', ' ', ' ', ' ', '8', '8', ' ', ' ', '8', 'o', 'o', 'o', 'o', 'o', ' ', ' ', '\n'};
		char Toe4[27] = {' ', ' ', ' ', '8', '8', '8', ' ', ' ', ' ', '8', '8', ' ', ' ', ' ', '8', '8', ' ', ' ', '8', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\n'};
		char Toe5[27] = {' ', ' ', ' ', '8', '8', '8', ' ', ' ', ' ', '"', '8', '8', '8', '8', '8', '"', ' ', ' ', '8', '8', '8', '8', '8', '8', '8', '8', '\n'};
			
		USART_Write(Tic, 25);
		USART_Write(Tic1, 25);
		USART_Write(Tic2, 25);
		USART_Write(Tic3, 25);
		USART_Write(Tic4, 25);
		USART_Write(Tic5, 25);
		USART_Write(newline, 1);
		USART_Write(newline, 1);
			
		delay_ms(10000);
			
		USART_Write(Tac, 25);
		USART_Write(Tac1, 25);
		USART_Write(Tac2, 25);
		USART_Write(Tac3, 25);
		USART_Write(Tac4, 25);
		USART_Write(Tac5, 25);
		USART_Write(newline, 1);
		USART_Write(newline, 1);
			
	  delay_ms(10000);
			
		USART_Write(Toe, 27);
		USART_Write(Toe1, 27);
		USART_Write(Toe2, 27);
		USART_Write(Toe3, 27);
		USART_Write(Toe4, 27);
		USART_Write(Toe5, 27);
		USART_Write(newline, 1);
		USART_Write(newline, 1);
		
		delay_ms(10000);
	}
	
	void writeGameOver() {
    char Game[33] =  {'8', '8', '8', '8', '8', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\n'};
    char Game1[33] = {'8', ' ', ' ', ' ', ' ', ' ', ' ', 'o', 'o', 'o', ' ', ' ', ' ', '8', 'o', 'o', 'o', ' ', 'o', 'o', 'o', ' ', ' ', ' ', '8', '8', '8', '8', '8', '8', '8', ' ', '\n'};
    char Game2[33] = {'8', ' ', ' ', ' ', ' ', ' ', ' ', '8', '8', '8', ' ', ' ', ' ', '8', ' ', ' ', ' ', '8', ' ', ' ', ' ', '8', ' ', ' ', '8', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\n'};
    char Game3[33] = {'8', ' ', '8', '8', '8', ' ', '8', 'o', 'o', 'o', '8', '8', ' ', '8', ' ', ' ', ' ', '8', ' ', ' ', ' ', '8', ' ', ' ', '8', 'o', 'o', 'o', 'o', 'o', ' ', ' ', '\n'};
    char Game4[33] = {'8', ' ', ' ', ' ', '8', ' ', '8', '8', ' ', ' ', '8', '8', ' ', '8', ' ', ' ', ' ', '8', ' ', ' ', ' ', '8', ' ', ' ', '8', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\n'};
		char Game5[33] = {'8', '8', '8', '8', '8', ' ', '8', '8', ' ', ' ', '8', '8', ' ', '8', ' ', ' ', ' ', '8', ' ', ' ', ' ', '8', ' ', ' ', '8', '8', '8', '8', '8', '8', '8', '8', '\n'};
    char newline[1] = {'\n'};
		
		char Over[33] =  {'8', '8', '8', '8', '8', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\n'};
    char Over1[33] = {'8', ' ', ' ', ' ', '8', ' ', '8', ' ', ' ', ' ', '8', ' ', '8', '8', '8', '8', '8', '8', '8', ' ', ' ', ' ', '8', '8', '8', '8', ' ', ' ', ' ', ' ', ' ', ' ', '\n'};
    char Over2[33] = {'8', ' ', ' ', ' ', '8', ' ', '8', 'o', ' ', 'o', '8', ' ', '8', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '8', ' ', ' ', ' ', '8', ' ', ' ', ' ', ' ', ' ', '\n'};
    char Over3[33] = {'8', ' ', ' ', ' ', '8', ' ', ' ', '8', ' ', '8', ' ', ' ', '8', 'o', 'o', 'o', 'o', 'o', ' ', ' ', ' ', ' ', '8', '8', '8', '8', ' ', ' ', ' ', ' ', ' ', ' ', '\n'};
    char Over4[33] = {'8', ' ', ' ', ' ', '8', ' ', ' ', 'o', ' ', 'o', ' ', ' ', '8', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '8', ' ', ' ', ' ', '8', ' ', ' ', ' ', ' ', ' ', '\n'};
		char Over5[33] = {'8', '8', '8', '8', '8', ' ', ' ', ' ', '8', ' ', ' ', ' ', '8', '8', '8', '8', '8', '8', '8', '8', ' ', ' ', '8', ' ', ' ', ' ', '8', ' ', ' ', ' ', ' ', ' ', '\n'};
   
		USART_Write(Game, 33);
    USART_Write(Game1, 33);
    USART_Write(Game2, 33);
    USART_Write(Game3, 33);
    USART_Write(Game4, 33);
		USART_Write(Game5, 33);
    USART_Write(newline, 1);
			
		delay_ms(8000);
			
    USART_Write(Over, 33);
    USART_Write(Over1, 33);
    USART_Write(Over2, 33);
    USART_Write(Over3, 33);
    USART_Write(Over4, 33);
		USART_Write(Over5, 33);
    USART_Write(newline, 1);
		
		delay_ms(1000000000);
		
		
}
	
char checkWinner() {
    int i;

    // Check rows
    for (i = 0; i < 3; i++) {
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != ' ') {
            return board[i][0]; // Row win
        }
    }

    // Check columns
    for (i = 0; i < 3; i++) {
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != ' ') {
            return board[0][i]; // Column win
        }
    }

    // Check diagonals
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != ' ') {
        return board[0][0]; // Main diagonal
    }
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != ' ') {
        return board[0][2]; // Secondary diagonal
    }

    // Check for draw
    for (i = 0; i < 3; i++) {
        int j;
        for (j = 0; j < 3; j++) {
            if (board[i][j] == ' ') {
                return ' '; // Game still in progress
            }
        }
    }
	
    return 'D'; // Draw
}


int main(void){
   RCC->CR |= RCC_CR_HSION;
	 while((RCC->CR & RCC_CR_HSIRDY) == 0);  //HSI clock enable
   RCC->CFGR |= RCC_CFGR_SW_HSI; //switch to HSI clock
   RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN; //GPIOA clock enable
	 //bits 2 and 3 need to be alternate function mode GPIOA (10)
   GPIOA->MODER &= 0xFFFFFF0F; //Set wanted bits to 0
	 GPIOA->MODER |= 0x000000A0; //Bits 2 and 3 are now 10 which is alternate function mode
   GPIOA->AFR[0] |= 0x77 << (4*2);
   //GPIOA->OPEEDR |= 0xF << (2*2);
   USART_Init();
	 LCD_Init();
	 LCD_DisplayString(0, "Tic-Tac-Toe");
	 writeTicTacToe();
	 WriteBoard();
	
	 while (1) {
		 if (GPIOC->IDR != 0x1F) {
		 readButtons();
		 WriteBoard();
	 }
 } 
}



