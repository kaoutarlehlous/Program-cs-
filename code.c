#include "tm4c123gh6pm.h"

#define LCDDATA (*((volatile uint32_t*)0x400053FC))   // PORTB
#define LCDCMD (*((volatile uint32_t*)0x40004300))    // PA7-PA6

#define lcd_display_data_1 " Microprocessor "
#define lcd_display_data_2 " System LCD Lab "


#define E  0x80 // on PA7
#define RS 0x40 // on PA6


void setup (void);
void LCD_Init(void);
void delay(unsigned long value);
void OutCmd(unsigned char command);
void LCD_OutChar(unsigned char letter);
void LCD_Clear(void);
void lcd_DisplayString (char str [],unsigned int length);


	//		Description : lcd initialization  
//--------------------------------------------------

void LCD_Init()
{
		LCDCMD = 0;           // E=0, R/W=0, RS=0
		delay (500000);       // Wait >30 ms after power is applied
		OutCmd(0x38);         // Function set: 8-bit/2-line
		delay(1000); 					// wait 40us
		OutCmd(0x0C);         // Display ON; Cursor ON
		delay(1000); 					// wait 40us
		OutCmd(0x06);         // Entry mode set
		delay(1000); 					// wait 40us
		LCD_Clear() ;
}	
/*--------------------------------------------------/
      Name : setup
			Description : Configure GPIO pins used to communicate
			with LCD and clock setup for the experiment 
/--------------------------------------------------*/

		// clock setup
	//SYSCTL_RCGC2_R |= 0x00000003;  // 1) activate clock for Ports A and B

  
  GPIO_PORTB_PCTL_R = 0x00000000;   // 4) configure PB7-0 as GPIO   

  GPIO_PORTB_DIR_R = 0xFF;       // 5) set direction register

  GPIO_PORTB_AFSEL_R = 0x00;     // 6) regular port function

  GPIO_PORTB_DEN_R = 0xFF;       // 7) enable digital port
	
  GPIO_PORTB_DR8R_R = 0xFF;      // enable 8 mA drive


 /*--------------------------------------------------/
  Name : lcd data
  Description : sends the data to the lcd controller
 /--------------------------------------------------*/

void LCD_OutChar(unsigned char letter)
{
  LCDDATA = letter;
  LCDCMD = RS;          // E=0, R/W=0, RS=1
  delay(150);   // wait 6us
  LCDCMD = E+RS;        // E=1, R/W=0, RS=1
  delay(150);   // wait 6us
  LCDCMD = RS;          // E=0, R/W=0, RS=1
	delay(1000);   // wait 40us
}

 /*--------------------------------------------------/
			 Name : lcd cmd
			 Description : sends the command to the lcd controller
 /--------------------------------------------------*/
void OutCmd(unsigned char command)
{
  LCDDATA = command;
  LCDCMD = 0;           // E=0, R/W=0, RS=0
	delay (150);
  LCDCMD = E;           // E=1, R/W=0, RS=0
	delay(150);
  LCDCMD = 0;           // E=0, R/W=0, RS=0
	delay(1000);
}
 

 /*--------------------------------------------------/
 Name : clear
 Descripton : clears the entire display and set DDRAM addr ess to 0
 /--------------------------------------------------*/

 // Clear the LCD
// Inputs: none
// Outputs: none
void LCD_Clear(void)
{
  OutCmd(0x01);          // Clear Display
  delay(27000); // wait 1.6ms 
  OutCmd(0x02);          // Cursor to home
  delay(27000); // wait 1.6ms 
}
  
/*--------------------------------------------------/
		Name : lcd Display String
		Description : displays character string on the lcd
/--------------------------------------------------*/

 void lcd_DisplayString ( char str [] , unsigned int length )
 {
 unsigned int i ;

	for ( i =0; i<length ; i++)
	{ 
	LCD_OutChar(str [i]) ;
	}
 }
 

void delay(unsigned long value)
{	
	unsigned long i ;
 for ( i = 0 ; i < value ; i++);
}
 

#define SYSCTL_RCGCGPIO_R (*((volatile unsigned long*)0x400FE608))

#define NVIC_EN0_R (*((volatile unsigned long*)0xE000E100))
#define NVIC_PRI7_R (*((volatile unsigned long*)0xE000E41C))

#define GPIO_PORTF_DATA_R (*((volatile unsigned long*)0x400253FC))
#define GPIO_PORTF_DIR_R (*((volatile unsigned long*)0x40025400))
#define GPIO_PORTF_DEN_R (*((volatile unsigned long*)0x4002551C))
#define GPIO_PORTF_PUR_R (*((volatile unsigned long*)0x40025510))

#define GPIO_PORTF_IS_R (*((volatile unsigned long*)0x40025404))
#define GPIO_PORTF_IBE_R (*((volatile unsigned long*)0x40025408))
#define GPIO_PORTF_IEV_R (*((volatile unsigned long*)0x4002540C))
#define GPIO_PORTF_IM_R (*((volatile unsigned long*)0x40025410))
#define GPIO_PORTF_ICR_R (*((volatile unsigned long*)0x4002541C))

#define NVIC_EN0_INT30 0x40000000          
#define PORTF_CLK_EN 0x20
#define LEDs 0xE
#define SW1 0x10
#define direcPort 0xE
#define INT_PF4 0x10

void EnableInterrupts (void);
void DisableInterrupts(void);
void Init_INT_GPIO (void);
void Delay ( unsigned long value ) ;
void WaitForInterrupt (void);

volatile unsigned long i=0;

void Init_INT_GPIO (void)
{
	volatile unsigned delay_clk;
	SYSCTL_RCGCGPIO_R|=0x20;
	delay_clk= SYSCTL_RCGCGPIO_R;
	
	//gpio
	GPIO_PORTF_DEN_R |= 0xFF;
	GPIO_PORTF_DIR_R=0xE;
	GPIO_PORTF_PUR_R|=0x10;

	//interrupts
	DisableInterrupts();
	GPIO_PORTF_IS_R &= ~INT_PF4;
	GPIO_PORTF_IBE_R &= ~INT_PF4;
	GPIO_PORTF_IEV_R &= ~INT_PF4;
	GPIO_PORTF_ICR_R |= INT_PF4;
	GPIO_PORTF_IM_R |= INT_PF4;

	NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF00FFFF)|0x00A00000 ;
	NVIC_EN0_R=NVIC_EN0_INT30;
	EnableInterrupts();
}

void Delay(unsigned long value)
{
	unsigned long i=0;
	for(i=0; i<value;i++);
}
void GPIOPortF_Handler(void)
{
	int j;                                   
	GPIO_PORTF_ICR_R=INT_PF4;
	if(i==3)
	{
		i=1;
	}
	else
	{
		i++;
	}
	for(j=0;j<2;j++)
	{
		GPIO_PORTF_DATA_R^=1<<i;
		Delay(10000000);
	}
	
}


int main()
{
	//question1
	Init_INT_GPIO();
	int count=0;
	//000, 001,010,...,111
	for (count=0; count<=7; count++)
	{
		GPIO_PORTF_ICR_R=INT_PF4;
		GPIO_PORTF_DATA_R^=count;
		//next binary value
		Delay(10000000);
	}
	//question2
  while(SW1)
	{
		GPIO_PORTF_DATA_R^=1;
		Delay(5000);
	}
	//q3
	int var=0;
	while (SW1)
	{
		var++;
		printf(var);
	
	}
	//Question4
	int in1=inp(0x40004);//port_a
	int in2= inp(0x40005);
	
	
	if (in1==0 && in2==0)
	{
		outp(0x400253FC,0x000);
	}
	else if (in1==0 && in2==1)
	{outp(0x400253FC,0x100);}
	else if(in1==1 && in2==0)
	{ outp(0x400253FC,0x010);}
	else{
	outp(0x400253FC,0x001);
	}
	
	//questions ESD MAIN
//reading power value
	float p;	
	int cost=0;
		while(SW1)
		{
		 p=inp(0x400053FC);
		cost+=(p/1000)*13;
			
		setup() ;
	  LCD_Init();
		OutCmd(0x80);         // line 1
		lcd_DisplayString ("Results ",16) ;
  	OutCmd(0xc0);					// line 2
		lcd_DisplayString (cost,16);
	  delay(10000);
		}
		if (p >=8500)
		{
			outp(0x400026AA,1);//setting up buzzr
		}
}